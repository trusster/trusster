/*Copyright (C) 2004 Mike Mintz

This file is part of the Teal Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "teal.h"
#include <algorithm>

using namespace teal;
//TO do: cobbler's shoes - make a simple mutex_t sentry and use it!!!

namespace {
  vlog local_log ("Teal::synch");
  static bool inhibit_time_read; 
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
vlog& teal::operator<< (vlog& v, const vreg_match& s)
{
  v << s.name << " of signal (at " << (long)&s.the_vreg << ") " << s.the_vreg;
  return v;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//Used in the communication between the at() function and the vcl callback.
//Basically, if the signal matches wack the realease and signal the condition.
//The thread id is used for debug.
//

// added by bih 23Oct06 for determinist thread handling
typedef struct teal_wrap_params 
{
  user_thread thread;
  void        *thread_data;

} teal_wrap_params_t;

typedef struct spawned_thread
{
  pthread_t       id;
  pthread_cond_t *pCond;

} spawned_thread_t;

class teal::thread_release {
public:
  bool really_released; //Because conditions get released prematurely
  pthread_t thread_id; 
  pthread_cond_t condition;
  
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  static pthread_mutex_t                  main_mutex;
  static pthread_mutex_t                  thread_list_mutex;
  static std::map<pthread_t, bool>        threads_waiting;
  static std::map<pthread_t, bool>        threads_completed;
  static std::map<pthread_t, std::string> thread_names;
  static std::string                      thread_being_created;

  // added by bih 23Oct06 for determinist thread handling
  static pthread_cond_t                   new_thread_waiting; // bih
  static std::deque< spawned_thread_t >   spawned_threads_fifo;
  static std::deque< thread_release * >   callback_cleanup;


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  thread_release () :
    really_released (false),
    thread_id (pthread_self ())
  {
    pthread_cond_init (&condition, 0);
    pthread_mutex_init (&thread_list_mutex, 0);
  }

  // atomic routines to determine if threads are waiting or completed
  static bool threadIsWaiting( pthread_t id ) {
    assert( thread_found( id ) );
    bool waiting;
    pthread_mutex_lock (&thread_list_mutex);
    waiting = threads_waiting[ id ];
    pthread_mutex_unlock (&thread_list_mutex);
    return waiting;
  }

  static bool threadIsCompleted( pthread_t id ) {
    assert( thread_found( id ) );
    bool completed;
    pthread_mutex_lock (&thread_list_mutex);
    completed = threads_completed[ id ];
    pthread_mutex_unlock (&thread_list_mutex);
    return completed;
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Called by the acc callback 
  //
  void do_callback () {
    //    log << note << "teal thread_release do_callback get mutex." << endm;
    pthread_mutex_lock (&main_mutex);

    if (!thread_found (thread_id)) {
      local_log << note << "teal thread_release do_callback on killed thread. " << (int)thread_id << " Ignored." << endm;
      pthread_mutex_unlock (&main_mutex);
    }

    really_released = true;
    pthread_cond_signal (&condition);
    //        local_log << note << "teal thread_release thread " << thread_name_ (thread_id) << " do_callback condition signaled." << endm;

    thread_running_ (thread_id);

    // added 24Oct06 by bih 
    // let thread we just signalled run until it blocks... maybe use a condition later?
    pthread_mutex_unlock (&main_mutex);
    do {
      sched_yield( );
    } while ( !thread_release::threadIsWaiting( thread_id ) && !thread_release::threadIsCompleted( thread_id ) );

    // bih - step through each new thread that's been created and unblock them one at a time
    // note that an executing thread may create additional new threads so reinit iterator
    // each time through
    run_new_threads( );


  }


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static bool thread_found (pthread_t id) {
    //have to use the iter so we don't create an entry
    bool returned (false);
    pthread_mutex_lock (&thread_list_mutex);
    for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	 (it != threads_waiting.end ()); ++it) {
      if (it->first == id) {
	returned = true;
	break;
      }
    }
    pthread_mutex_unlock (&thread_list_mutex);
    return returned;
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static void print_threads_ (const std::string& prefix) {
    //    local_log << note << thread_name_ (pthread_self ()) << " " << prefix << endm;
    pthread_mutex_lock (&thread_list_mutex);
    local_log << note << prefix << endm;
    bool one_printed (false);
    for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	 (it != threads_waiting.end ()); ++it) {
      one_printed = true;
      local_log << note << "Thread : " << thread_name_ (it->first) << " (" << hex << (long)it->first << ") is";
      local_log << ((it->second) ? " waiting." : " running.") << endm;
    }
    if (! one_printed) {
      local_log << note << "No active threads." << endm;
    }
    pthread_mutex_unlock (&thread_list_mutex);

  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static std::string thread_name_ (pthread_t id) {
    assert( thread_found (id) );
    std::string name;
    pthread_mutex_lock (&thread_list_mutex);
    name = thread_names[ id ];
    pthread_mutex_unlock (&thread_list_mutex);
    return name;
#ifdef FOO
    //have to use the iterator so we don't create an entry
    pthread_mutex_lock (&thread_list_mutex);
    for (std::map<pthread_t,std::string>::iterator it(thread_names.begin());
	 (it != thread_names.end ()); ++it) {
      if (it->first == id) {
	pthread_mutex_unlock (&thread_list_mutex);
	return it->second;
      }
    }

    pthread_mutex_unlock (&thread_list_mutex);

    std::ostringstream temp;
    temp << "Unknown thread name for id: 0x" << std::hex << id;
    if (thread_being_created != "") {return thread_being_created;}
    return temp.str();
#endif
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the acc callback
  //
  static void thread_running_ (pthread_t id) {
    assert( thread_found (id) );

    pthread_mutex_lock (&thread_list_mutex);
    threads_waiting[id] = false;
    pthread_mutex_unlock (&thread_list_mutex);
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the start_thread() code
  //
  static void thread_created_ (pthread_t id, const std::string& name) {
    pthread_mutex_lock (&thread_list_mutex);
    threads_waiting[id] = false;
    thread_names   [id] = name;
    pthread_mutex_unlock (&thread_list_mutex);
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the at () function
  //
  static void thread_waiting_ (pthread_t id) {
    assert( thread_found ( id ) );

    pthread_mutex_lock (&thread_list_mutex);
    threads_waiting[id] = true;
    pthread_mutex_unlock (&thread_list_mutex);

  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  
  //
  static void thread_completed (pthread_t id) {
    //    local_log << note << "thread_completed: get mutex " << endm;
    assert( thread_found( id ) );

    local_log << note << "thread_completed: Thread " << thread_name_ (id) << endm;

    pthread_mutex_lock (&thread_list_mutex); 
    threads_completed[ id ] = true;
    threads_waiting  [ id ] = false;
    pthread_mutex_unlock (&thread_list_mutex); 

    //      print_threads_ ();
  }

  // added 24Oct06 by bih 
  // step through each new thread that's been created and unblock them one at a time
  // note that an executing thread may create additional new threads so reinit iterator
  // each time through
  static void run_new_threads( void ) {
    pthread_t        new_id;
    spawned_thread_t next_spawn;
    uint32           num_spawned;

    pthread_mutex_lock( &main_mutex );
    num_spawned = spawned_threads_fifo.size( );
    pthread_mutex_unlock (&main_mutex); 

    while ( num_spawned ) {
      pthread_mutex_lock( &main_mutex );
      next_spawn = spawned_threads_fifo.front( );
      pthread_mutex_unlock (&main_mutex); 
      new_id = next_spawn.id;
      //      local_log << note << "Signalling thread " << ( long )new_id << " on condition " << ( long )next_spawn.pCond << endm;
      thread_running_( new_id );
      pthread_cond_signal( next_spawn.pCond );
      do {
	sched_yield( );
      } while ( !threadIsWaiting( new_id ) && !threadIsCompleted( new_id ) );
      //#define FOO
#ifdef FOO
      if ( threadIsCompleted( new_id ) ) {
	pthread_join( new_id, NULL );
      }
#endif
      
      pthread_mutex_lock( &main_mutex );
      spawned_threads_fifo.pop_front( );
      num_spawned = spawned_threads_fifo.size( );
      pthread_mutex_unlock( &main_mutex );

    }
  }
};

//create the thread_release static(s)
pthread_cond_t                   teal::thread_release::new_thread_waiting = PTHREAD_COND_INITIALIZER;
std::deque< spawned_thread_t >   teal::thread_release::spawned_threads_fifo;
std::deque< thread_release * >   teal::thread_release::callback_cleanup;
std::map<pthread_t, bool>        teal::thread_release::threads_waiting;
std::map<pthread_t, bool>        teal::thread_release::threads_completed;
std::map<pthread_t, std::string> teal::thread_release::thread_names;
pthread_mutex_t                  teal::thread_release::main_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t                  teal::thread_release::thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;
std::string                      teal::thread_release::thread_being_created;


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//This 
//
std::deque<vreg_match*> callbacks;


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//This is the work part of the entry point from the verilog world.
//
int the_generic_callback (vreg_match* a_vreg_match)
{

  //     local_log << note << "teal thread_release begin from verilog callback." << endm;
  //ASSERT (a_vreg_match);

  vreg::invalidate_all_vregs (); //must be before the match() otherwise, the signal will never change!

  if (! a_vreg_match->match ()) {
    //    local_log << note << "teal thread_release no signal match returning from verilog callback." << *a_vreg_match << endm;
    return 0;
  }

  //  local_log << note << "teal acc_callback vreg matches." << *a_vreg_match << endm;


  //actually have some processing to do.
  
  callbacks.push_back (a_vreg_match);


  if (callbacks.size() != 1) { //stacked callbacks?
    // local_log << note << "teal callback called within callback. Work is queued. " << endm;
    return 0;
  }

  while (callbacks.size ()) {
    vreg_match* the_vreg_match = callbacks.front ();
    vreg::invalidate_all_vregs ();  //in case a stacked callback changed signals that another wanted.
    //ASSERT (the_vreg_match->the_thread_release);
    the_vreg_match->the_thread_release->do_callback ();    
    //local_log << note << "teal thread_release returning from do_callback." << endm;
    callbacks.pop_front ();  //MUST save the pop until the end because this tells when we are done processing.
    //If we pop'ed it in the beginning, a callback because of a changed val in teh c++ land would not see us processing.
  }


  //    local_log << note << "teal thread_release returning from verilog callback." << endm;
  return 0;
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//This is the actual entry point from the verilog world.
//
#if defined (vpi_2_0)
int  the_vpi_callback (s_cb_data* the_s_cb_data)
{
  //ASSERT (the_s_cb_data);
  //   vlog ("the vpi_callback")  << note << "teal vpi_callback called with reason." << the_s_cb_data->reason << endm;
  if (the_s_cb_data->reason != cbValueChange) {
    local_log << error << "teal vpi_callback called with unknown reason." << the_s_cb_data->reason << " Ignored" << endm;
    return 0;
  }
  
  return the_generic_callback ((vreg_match*)the_s_cb_data->user_data);
}

#else
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
int the_acc_callback (s_vc_record* the_s_vc_record)
{
  //ASSERT (the_s_vc_record);
  //local_log << note << "teal acc_callback called with reason." << the_s_vc_record->vc_reason << endm;
  if ( (the_s_vc_record->vc_reason != vregister_value_change) &&
       (the_s_vc_record->vc_reason != sregister_value_change) &&
       (the_s_vc_record->vc_reason != logic_value_change))    {
    local_log << error << "teal acc_callback called with unknown reason." << the_s_vc_record->vc_reason << " Ignored" << endm;
    return 0;
  }
  
  return the_generic_callback ((vreg_match*)the_s_vc_record->user_data);
}
#endif

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* teal::stop_thread (pthread_t id)
{
  //read up up on pthread cancel. would like to cause an exception in the thread
  //  local_log << note << "teal::stop_thread " << thread_name (id) << endm;
  int result = pthread_cancel (id);
  //  ASSERT (result == 0);
  //   local_log << note << "teal::stop_thread done cancel on " << thread_name (id) << endm;
  void* returned;
  result = pthread_join (id, &returned);
  //    local_log << note << "teal::stop_thread done join on " << thread_name (id) << endm;
  //ASSERT (result == 0);
  thread_release::thread_completed (id);
  return returned;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* teal::join_thread (pthread_t id)
{
  //read up up on pthread cancel. would like to cause an exception in the thread
  //  local_log << note << "teal::stop_thread " << thread_name (id) << endm;
  //  ASSERT (result == 0);
  //   local_log << note << "teal::stop_thread done cancel on " << thread_name (id) << endm;
  void* returned;
  int result = pthread_join (id, &returned);
  //    local_log << note << "teal::stop_thread done join on " << thread_name (id) << endm;
  assert (result == 0);
  thread_release::thread_completed (id);
  return returned;
}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::stop_all_threads ()
{
  for (std::map<pthread_t,bool>::reverse_iterator it(thread_release::threads_waiting.rbegin());
       (it != thread_release::threads_waiting.rend ()); ++it) {
    if ((thread_name (it->first) != "Teal Control Thread") &&
	(thread_name (it->first) != "user_main")) {
      stop_thread (it->first);
    }
  }
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::note_thread_completed ()
{
  thread_release::thread_completed (pthread_self ());
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void thread_cleanup (void* context)
{
  //      local_log << note << "Thread cleanup" << endm;
  const sensitivity& s = *(sensitivity*) (context);
  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin());
       it != s.list.end(); ++it) {
    (*it)->the_thread_release = 0;
#if defined (vpi_2_0)
    vpi_remove_cb ((*it)->the_call_back);
    (*it)->the_call_back = 0;
#else
#if defined (aldec) || defined (vcs) || defined (cver)
    acc_vcl_delete ((*it)->get_handle(), (consumer_function)the_acc_callback, (char*) (*it), vcl_verilog);
#else
    acc_vcl_delete ((*it)->get_handle(), the_acc_callback, (char*) (*it), vcl_verilog);
#endif
#endif
  }

  pthread_mutex_unlock (&thread_release::main_mutex);
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//


// added by bih 23Oct06 for determinist thread handling
void * teal::start_thread_wrapper ( void *t_params )
{

  pthread_mutex_lock( &thread_release::main_mutex );

  // suspend self and put on list of new threads to individually enable
  pthread_t       id    = pthread_self( );
  thread_release::thread_created_ (id, thread_release::thread_being_created);

  pthread_cond_t *pCond = new pthread_cond_t;
  pthread_cond_init( pCond, NULL );
  thread_release::thread_waiting_( id );

  spawned_thread_t spawn;
  spawn.id    = id;
  spawn.pCond = pCond;
  thread_release::spawned_threads_fifo.push_back( spawn );

  local_log << note << "Suspending thread " << ( long )id << " on condition " << ( long )pCond << endm;
  pthread_cond_signal( &thread_release::new_thread_waiting );
  do {
    pthread_cond_wait  ( pCond, &thread_release::main_mutex );
  } while ( thread_release::threadIsWaiting( id ) );

  delete pCond;

  pthread_mutex_unlock( &thread_release::main_mutex );

  // start user thread
  teal_wrap_params_t *t_params_ptr = ( teal_wrap_params_t * )t_params;

  ( *( t_params_ptr->thread ) )( t_params_ptr->thread_data );

  delete t_params_ptr;

}

pthread_t teal::start_thread (user_thread thread, void* user_data, const std::string & name)
{
  teal_wrap_params_t *w_params_ptr = new teal_wrap_params_t;

  w_params_ptr->thread      = thread;
  w_params_ptr->thread_data = user_data;

  pthread_t id;
  pthread_attr_t attributes;
  pthread_attr_init (&attributes);
  //  pthread_attr_setstacksize( &attributes, 0x20000 );

  pthread_mutex_lock (&thread_release::main_mutex);
  thread_release::thread_being_created = name;

  int result = pthread_create (&id, &attributes, teal::start_thread_wrapper, ( void * )w_params_ptr );  
  local_log << note << "Thread " << name << " created. ID is " << hex << 
    (long)id << " result " << result << endm;

  // block until we know the new thread has stopped
  do {
    pthread_cond_wait (&thread_release::new_thread_waiting, &thread_release::main_mutex );
  } while ( !thread_release::threadIsWaiting( id ) );

  thread_release::thread_being_created = "";

  pthread_mutex_unlock (&thread_release::main_mutex);
  
  return id;

  
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::at (const sensitivity& s)
{
  //  local_log << note << thread_name (pthread_self ()) << " teal::at() " << " begin " << endm;
  if (! s.list.size()) {
    local_log << error << "teal::at() called with no vregs! Ignored." << endm;
  }
  thread_release* the_thread_release = new thread_release ();    

  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin());
       it != s.list.end(); ++it) {
    (*it)->the_thread_release = the_thread_release;
#if defined (vpi_2_0) 
   s_cb_data call_back;
   static s_vpi_value no_value;
   static s_vpi_time no_time;
   no_value.format = vpiSuppressVal;
   no_time.type = vpiSuppressTime;
   //   no_value.format = vpiScalarVal;
   //   no_time.type = vpiSimTime;
   //   call_back.index = 0;

    call_back.reason = cbValueChange;
    call_back.cb_rtn = the_vpi_callback;
    call_back.obj = (*it)->get_handle ();
    call_back.user_data = (char*) (*it);
    call_back.value = &no_value;
    call_back.time = &no_time;
    (*it)->the_call_back = vpi_register_cb (&call_back);
    //       local_log << note << " after vpi_register_cb of " << (int)(*it)->the_call_back << " handle " << (int)call_back.obj << teal::endm;
#else
#if defined (aldec) || defined (vcs) || defined (cver)
    acc_vcl_add ((*it)->get_handle(), (consumer_function)the_acc_callback, (char*) (*it), vcl_verilog);
#else
    acc_vcl_add ((*it)->get_handle(), the_acc_callback, (char*) (*it), vcl_verilog);
#endif
#endif
    //    local_log << note << " after acc_vcl_add of " << (**it) << teal::endm;
  }

  pthread_mutex_lock (&thread_release::main_mutex);
  thread_release::thread_waiting_ (pthread_self ());

  pthread_cleanup_push (&thread_cleanup, (void*)&s);

  do {
    //        local_log << note << " waiting on condition. " << teal::endm;
    pthread_cond_wait (&the_thread_release->condition, &thread_release::main_mutex);
    //    local_log << note << "Thread " << thread_name (pthread_self()) << " done waiting on condition. " << 
    //              the_thread_release->really_released << endm;
  } while (! the_thread_release->really_released);

  pthread_cleanup_pop (0);

  pthread_mutex_unlock (&thread_release::main_mutex);

  //      local_log << note << "Thread " << thread_name (pthread_self()) << " really done waiting on condition. " << endm;

  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin());
       it != s.list.end(); ++it) {
    (*it)->the_thread_release = 0;
#if defined (vpi_2_0)
    vpi_remove_cb ((*it)->the_call_back);
    //    local_log << note << "Thread " << thread_name (pthread_self()) << " removed callback on. " << **it << endm;
    (*it)->the_call_back = 0;
#else
#if defined (aldec) || defined (vcs) || defined (cver)
    acc_vcl_delete ((*it)->get_handle(), (consumer_function)the_acc_callback, (char*) (*it), vcl_verilog);
#else
    acc_vcl_delete ((*it)->get_handle(), the_acc_callback, (char*) (*it), vcl_verilog);
#endif
#endif
  }

  //  delete the_thread_release; // do_callback still needs this! garbage collection? fixme

  // can't delete the_thread_release yet because verilog is still using it, so keep track of to clean up 
  // later
  while ( thread_release::callback_cleanup.size( ) ) {
    delete thread_release::callback_cleanup.front( );
    thread_release::callback_cleanup.pop_front( );
  }

  thread_release::callback_cleanup.push_back( the_thread_release );

}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
std::string teal::thread_name (pthread_t id) 
{
  //  pthread_mutex_lock (&thread_release::main_mutex);
  std::string returned =  thread_release::thread_name_ (id);
  //  pthread_mutex_unlock (&thread_release::main_mutex);
  return returned;
}


extern "C" void teal_main_call (int,int);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* run_main_thread (void*)
{
  // breakpoint for debugger
  acc_product_version( );

  user_main ();
  return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
int teal_main_internal (char*)
{
  pthread_attr_t attributes;
  pthread_attr_init (&attributes);
  pthread_t id;

  acc_product_version( );

  inhibit_time_read = true;
  local_log.message_display (vout::thread_name, false);
  thread_release::thread_created_( pthread_self( ), "Verilog" );
  local_log << note << "Teal_main: Starting main thread. " << endm;

  pthread_t main_id = start_thread (run_main_thread, 0, "user_main");
  local_log << note << "Teal_main: Started main thread. id:" << (long)main_id << endm;
  //ASSERT (result == 0);
  
  // added 24Oct06 by bih 
  // step through each new thread that's been created and unblock them one at a time
  thread_release::run_new_threads( );

  local_log << note << "Teal_main: Starting Simulation. " << endm;
  inhibit_time_read = false;
  return 0;
}

#if defined (vpi_2_0) && (!defined (vcs))
//should this be in the unnamed namespace ?

///////////////////////////////////////////////
///////////////////////////////////////////////
void teal::teal_main_register ()
{
  s_vpi_systf_data task_data = {0};
  task_data.type = vpiSysTask;
  task_data.tfname = "$teal_main";
#if defined (cver)
  task_data.calltf = (p_tffn) teal_main_internal;//0;
#else
  task_data.calltf = teal_main_internal;//0;
#endif
  task_data.compiletf = 0;//teal_main_internal;
  vpi_register_systf (&task_data);
}
#else
#if defined (cverX)
void teal_main_call ()
#else
void teal_main_call (int user_data, int reason)
#endif
{
  teal_main_internal (0);
}
#endif


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
uint64 teal::vtime ()
{
  uint64 returned (0);

#if !defined (teal_printf_io)
#  if defined (vpi_2_0)
  //  int high(0);
  s_vpi_time here_and_now;
  here_and_now.type = vpiSimTime;
  vpi_get_time (0, &here_and_now);
  returned  = ((uint64)here_and_now.high << 32) + here_and_now.low;
#  else
  int high(0);
  int low = inhibit_time_read ? 0 : tf_getlongtime (&high);
  returned  = ((uint64)high << 32) + low;
#  endif
#endif

  //  if (!inhibit_time_read) {
  //    std::cout << "vtime thread name" << thread_name (pthread_self ()) << std::endl;
  //  }
  //  local_log << note << " vtime " << returned  << " inhibit is " << inhibit_time_read << " " << endm;
  return returned;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::condition::condition (const std::string& name)   : 
  name_ (name),
  signalled_ (false),
  time_at_signal_ (vtime())
{

}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::condition::~condition ()
{

}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void semaphore_thread_cleanup (void* context)
{
  local_log << note << "Sempahore thread cleanup. releasing main mutex" << endm;
  pthread_mutex_unlock (&thread_release::main_mutex);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::condition::wait ()
{
  //  local_log << note << thread_name (pthread_self ()) << " teal::condition::wait() \"" << name_ << "\" begin " << endm;
  //WARNING: Currently (4/2004) cannot print thread_name() while the main mutex is held! The code will hang.

  if ((signalled_) && (vtime() >= time_at_signal_)) {
    signalled_ = false;
    //(taken out 5/24/04)    pthread_mutex_unlock (&thread_release::main_mutex);
    local_log << note << "teal::condition \"" << name_ << "\" wait after signalled." << endm;
    return;
  }

  // bih maintain map of conditions so we can wake threads in deterministic order
  pthread_mutex_lock (&thread_release::main_mutex);

  pthread_cond_t *pCond = new pthread_cond_t;
  pthread_cond_init( pCond, NULL );

  waiting_[ pthread_self () ] = pCond;
  thread_release::thread_waiting_ (pthread_self ());
  
  pthread_cleanup_push (&semaphore_thread_cleanup, 0);
  while (! signalled_) {
    //    local_log << note <<  " teal::condition::wait() \"" << name_ << "\" wait on condition " << endm;
    pthread_cond_wait ( pCond, &thread_release::main_mutex);
    //    local_log << note << " teal::condition::wait() " << name_ << " condition signalled " << endm;
  } 

  pthread_cleanup_pop (0);

  //local_log << note  << " teal::condition::wait() " << name_ << " wait signalled " << endm;

  delete pCond;
  signalled_ = false;
  waiting_.erase ( pthread_self( ) );
  thread_release::thread_running_ (pthread_self ());
  pthread_mutex_unlock (&thread_release::main_mutex);

  //  local_log << note << thread_name (pthread_self ()) << " teal::condition::wait() " << name_ << " returning " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::condition::signal ()
{
  //    local_log << note << thread_name (pthread_self ()) << " teal::condition::signal() \"" << name_ << "\" begin " << endm;
  // bih - deterministically wake threads up in order
  // copy map to local so we don;t conflict with threads trying to wait on same condition
  signalled_ = true;
  time_at_signal_ = vtime();

  std::map<pthread_t, pthread_cond_t*> wakeMap = waiting_;
  std::map<pthread_t, pthread_cond_t*>::iterator iter;

  waiting_.clear( );

  for ( iter = wakeMap.begin( ); iter != wakeMap.end( ); iter++ ) {
    pthread_mutex_lock (&thread_release::main_mutex);
    thread_release::thread_running_( iter->first );
    pthread_cond_signal( iter->second );
    pthread_mutex_unlock (&thread_release::main_mutex);

    do {
      sched_yield( );
    } while ( !thread_release::threadIsWaiting( iter->first ) && !thread_release::threadIsCompleted( iter->first ) );

  }

  //we've woken every thread waiting on this condition... 
  // check for any new threads we may have spawned
  thread_release::run_new_threads( );

  //  local_log << note << thread_name (pthread_self ()) << " teal::condition " << name_ << " signal()  done " << endm;
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::mutex::mutex (const std::string& name) :
  name_ (name),
#ifndef MUTEX_NEW
  condition_ (name),
#endif
  waiters_ (0),
  someone_running (false)

{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  //next line removed because older (<1998?) Linux pthreads do not have this
  //  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK);

  pthread_mutex_init (&mutex_, 0);
  pthread_mutex_init (&waiters_mutex_, 0);
  pthread_mutexattr_destroy (&attr);

  //   local_log << note << "teal::mutex " << name_ << " ctor  " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::mutex::~mutex ()
{
  //   local_log << note << "teal::mutex " << name_ << " dtor  " << endm;  
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
//Note: The theory is to use trylock() to either:
// (1) acquire the lock without fail. In this case we are free to use the hardware and all
//     we have to do is remember to unlock the mutex.
// (2) fail the lock. In this case, there is at least one other thread using the hardware, so
//     we fall on a condition. Then, when one of the waiting threads is signalled,
//     we acquire the mutex. It may fail a few times until the unlocking
//     thread get to execute its pthread_unlock, but it will occur within this verilocal_log callback.
//
void teal::mutex::lock ()
{
  //    local_log << note << thread_name (pthread_self ()) << " teal::mutex::lock \"" << name_ << "\"  begin " << endm;
  if (pthread_mutex_trylock (&mutex_)) {
    pthread_mutex_lock (&waiters_mutex_);

#ifdef MUTEX_NEW
    //    char       *waiterStr[64];
    //    sprintf( waiterStr, "%d", waiters_ );
    
    //    std::string condName      = name_ + waiterStr;
    condition  *pNewCondition = new condition( name_ );
    condition_dq.push_back( pNewCondition );
#endif
    waiters_++;
    //    local_log << note << " teal::mutex \"" << name_ << "\" lock begin wait. waiters_: " << waiters_ << endm;
    pthread_mutex_unlock (&waiters_mutex_);

#ifdef MUTEX_NEW
    pNewCondition->wait( );
    pthread_mutex_lock (&waiters_mutex_);
    condition_dq.pop_front( );
    delete pNewCondition;
    pthread_mutex_unlock (&waiters_mutex_);
#else
    condition_.wait ();
    //    local_log << note << " teal::mutex " << name_ << " lock begin back from wait. waiters_: " << waiters_ << endm;
#endif
    while (pthread_mutex_trylock (&mutex_)) {
            sched_yield (); //posix sleep thinge
    }
    someone_running = true;
    //    local_log << note << " teal::mutex " << name_ << " lock back from signal. waiters_: " << waiters_ << endm;
  }
  //  local_log << note <<  thread_name (pthread_self ()) << " teal::mutex::lock " << name_ << " acquired. " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::mutex::unlock ()
{
  //  local_log << note << thread_name (pthread_self ()) << " teal::mutex::unlock " << name_ << " begin " << endm;
  //  local_log << note << " teal::mutex " << name_ << " unlock begin " << endm;
    
  pthread_mutex_lock (&waiters_mutex_);

  if (waiters_) {
    waiters_--;
    pthread_mutex_unlock (&waiters_mutex_);
    someone_running = false; //ensure that someone other than me gets the mutex next 
#ifdef MUTEX_NEW
    pthread_mutex_unlock (&mutex_);
    condition *pCond = condition_dq.front( );
    pCond->signal( );
#else
    condition_.signal ();
    pthread_mutex_unlock (&mutex_);
#endif
    //local_log << note << thread_name (pthread_self ()) << " teal::mutex " << name_ << " after signal " << endm;
    while (! someone_running) {      
      sched_yield (); //posix sleep thinge
      //local_log << note << (int) (pthread_self ()) << " teal::mutex " << name_ << " waiting for someone else " << endm;
    };
  }
  else {
    pthread_mutex_unlock (&waiters_mutex_);
    pthread_mutex_unlock (&mutex_);
  }

  //  local_log << note << thread_name (pthread_self ()) << " teal::mutex::unlock " << name_ << " end lock released " << endm;
}

#ifdef FOO
// added by bih 28Jan05
// use this to signal back to C++ from a Verilog PLI/VPI/DPI call
// after broadcasting signal, wait for all threads to block before
// allowing Verilog to continue; necessary for simulation repeatability
void teal::condition::signalWait ()
{
  // maybe no longer need with rewritten signal code?
  signal( );
}
#endif


std::string teal::teal_version = "teal_0a.93c";
