/*
Trusster Open Source License version 1.0a (TRUST)
copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
   
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  * Redistributions in any form must be accompanied by information on how to obtain 
    complete source code for this software and any accompanying software that uses this software.
    The source code must either be included in the distribution or be available in a timely fashion for no more than 
    the cost of distribution plus a nominal fee, and must be freely redistributable under reasonable and no more 
    restrictive conditions. For an executable file, complete source code means the source code for all modules it 
    contains. It does not include source code for modules or files that typically accompany the major components 
    of the operating system on which the executable file runs.
 

THIS SOFTWARE IS PROVIDED BY MIKE MINTZ AND ROBERT EKENDAHL ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL MIKE MINTZ AND ROBERT EKENDAHL OR ITS CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "teal.h"
#include <algorithm>

using namespace teal;
//TO do: cobbler's shoes - make a simple mutex_t sentry and use it!!!

//when its unsafe to use teal's logger
#define MARK std::cout << __FILE__ << " " << __LINE__ << std::endl;

#if defined (WIN32)
  bool operator== (const pthread_t& lhs, const pthread_t& rhs) {
    return lhs.p == rhs.p;
  }
    bool operator< (const pthread_t& lhs, const pthread_t& rhs) {
    return lhs.p < rhs.p;
  }
#endif
  
	uint32 thread_int (const pthread_t& id) {
#if defined (WIN32)
	return (uint32)id.p;
#else
	  return (uint32)id;
#endif
	}

namespace {
  vout local_vout ("Teal::synch",1); //a HACK because some simulators croak if acc_fetch_args is called too early.
  static bool inhibit_time_read (false);
  static bool synch_chatty_ (false);
  static bool quitting_ (false);
  static uint64 sim_time_ (0);
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
vout& teal::operator<< (vout& v, const vreg_match& s)
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
class teal::thread_release {
public:
  bool really_released; //Because conditions get released prematurely
  pthread_t thread_id; 
  pthread_cond_t condition;
  
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  static bool                             really_all_waiting;
  static pthread_cond_t                   all_waiting;
  static bool                             allow_all_waiting;
  static pthread_cond_t                   rescan_thread_list;
  static pthread_mutex_t                  main_mutex;
  static pthread_mutex_t                  thread_name_mutex;
  static std::map<pthread_t, bool>        threads_waiting;
  static std::map<pthread_t, std::string> thread_names;
  static std::string                      thread_being_created;

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  thread_release () :
    really_released (false),
    thread_id (pthread_self ())
  {
    pthread_cond_init (&condition, 0);
    pthread_mutex_init (&thread_name_mutex, 0);
  }


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Called by the acc callback 
  //
  void do_callback () {
    if (synch_chatty_)    local_vout << teal_info << "teal thread_release do_callback get mutex." << endm;
    pthread_mutex_lock (&main_mutex);

    if (!thread_found (thread_id)) {
      local_vout << teal_info << "teal thread_release do_callback on killed thread. " << thread_name (thread_id) << " Ignored." << endm;
      pthread_mutex_unlock (&main_mutex);
      return; //mfm 10/25/05
    }

    really_released = true;
    pthread_cond_signal (&condition);
    if (synch_chatty_)        local_vout << teal_info << "teal thread_release thread " << thread_name_ (thread_id) << " do_callback condition signaled." << endm;

    thread_running_ (thread_id);
    really_all_waiting = false;
    do {
      //      local_vout << teal_info << "teal thread_release waiting for all waiting." << endm;
      pthread_cond_wait (&all_waiting, &main_mutex);
    } while ((!really_all_waiting) && (quitting_));

      if (synch_chatty_)    local_vout << teal_info << "teal thread_release got really_all_waiting releasing  mutex." << endm;
    pthread_mutex_unlock (&main_mutex);
  }


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static bool thread_found (pthread_t id) {
    //have to use the iter so we don't create an entry
    bool returned (false);
    for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	 (it != threads_waiting.end ()); ++it) {
      if (it->first == id) {
	returned = true;
	break;
      }
    }
    return returned;
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static void print_threads_ (const std::string& prefix) {
    //    local_vout << teal_info << thread_name_ (pthread_self ()) << " " << prefix << endm;
    vout local_vout2 ("Teal::synch");
    local_vout2 << teal_info << prefix << endm;
    bool one_printed (false);
    for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	 (it != threads_waiting.end ()); ++it) {
      one_printed = true;
      local_vout2 << teal_info << "Thread : " << thread_name_ (it->first) << " (" << hex << thread_int (it->first) << ") is";
      local_vout2 << ((it->second) ? " waiting." : " running.") << endm;
    }
    if (! one_printed) {
      local_vout2 << teal_info << "No active threads." << endm;
    }
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  Utility
  //
  static std::string thread_name_ (pthread_t id) {
    //have to use the iterator so we don't create an entry
    pthread_mutex_lock (&thread_name_mutex);
    for (std::map<pthread_t,std::string>::iterator it(thread_names.begin());
	 (it != thread_names.end ()); ++it) {
      if (it->first == id) {
	pthread_mutex_unlock (&thread_name_mutex);
	return it->second;
      }
    }

    pthread_mutex_unlock (&thread_name_mutex);

    std::ostringstream temp;
    temp << "Unknown thread name for id: 0x" << std::hex << thread_int (id);
    if (thread_being_created != "") {return thread_being_created;}
    return temp.str();
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the acc callback
  //
  static void thread_running_ (pthread_t id) {
    if (thread_found (id)) {
      //ASSERT (!  threads_waiting[id]);
    }
    else {
      local_vout << teal_error << "thread_running_: Thread " << thread_int (id) << " not found. Current Threads:" << endm;
      print_threads_ ("thread running");
    }
    threads_waiting[id] = false;
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the start_thread() code
  //
  static void thread_created_ (pthread_t id, const std::string& name) {
    if (thread_found (id)) {
      //      local_vout << teal_info << "thread_created_: Thread " << (long)id << " already exists. Current Threads:" << endm;
      thread_names[id] = name;
      print_threads_ ("thread created");
      //it may already be at a wait point. Threads execute randomily.
    }
    else {
      threads_waiting[id] = false;
      thread_names[id] = name;
    }
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  called by the at () function
  //
  static void thread_waiting_ (pthread_t id) {
    if (thread_found (id)) {
      //ASSERT (threads_waiting[id]);
      threads_waiting[id] = true;
    }
    else {
      threads_waiting[id] = true;
      if (thread_being_created != "") {
	thread_names[id] = thread_being_created;
      }
      else {
	local_vout << teal_info << "thread_waiting_: Thread " << thread_int (id) << " not found. Current Threads:" << endm;
	print_threads_ ("thread waiting ");
      }
    }

    pthread_cond_signal (&rescan_thread_list);
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  
  //
  static void thread_completed (pthread_t id) {
    //    local_vout << teal_info << "thread_completed: get mutex " << endm;
    pthread_mutex_lock (&main_mutex);
    if (thread_found (id)) {
      //ASSERT (!  threads_waiting[id]);
      for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	   (it != threads_waiting.end ()); ++it) {
	if (it->first == id) {
	  //leave the name just in case
	  threads_waiting.erase (it);
	  break;
	}
      }
    }
    else {
      local_vout << teal_debug << "thread_completed: Thread " << thread_int (id) << " not found. Current Threads:" << endm;
      print_threads_ ("thread completed");
    }
    vout local_vout2 ("Teal::synch");
    local_vout2 << teal_info << "thread_completed: Thread " << thread_name_ (id) << endm;
    //      print_threads_ ();
    pthread_cond_signal (&rescan_thread_list);
    pthread_mutex_unlock (&main_mutex);
  }


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //  
  //
  static void control_thread () {
    pthread_mutex_lock (&main_mutex);
    while (1) {
      //      local_vout << teal_info << "Teal control thread: waiting for a thread to get to a wait point. number of threads: " 
      //		       << threads_waiting.size () <<endm;
      //      print_threads_ ("waiting for a rescan");
      pthread_cond_wait (&rescan_thread_list, &main_mutex);
      bool waiting (true);
      //            local_vout << teal_info << "Teal control thread: a thread got to a wait point, rescanning." 
      //			     << threads_waiting.size () <<endm;
      for (std::map<pthread_t,bool>::iterator it(threads_waiting.begin());
	   (it != threads_waiting.end ()); ++it) {
	if (! it->second) {
	  waiting = false;
	  //local_vout << teal_info << "Teal control thread: Thread :" << thread_name_ (it->first) << " is not at a wait point." << endm;
	  break;
	}
      }
      if (waiting && allow_all_waiting) {
	really_all_waiting = true;
	pthread_cond_broadcast (&all_waiting);
	//	local_vout << teal_info << "Teal control thread: all threads at wait points. signalling all_waiting.." << endm;
      }
    }
    pthread_mutex_unlock (&main_mutex);
  }
};

//create the thread_release static(s)
bool                             teal::thread_release::really_all_waiting;
pthread_cond_t                   teal::thread_release::all_waiting = PTHREAD_COND_INITIALIZER;
bool                             teal::thread_release::allow_all_waiting = true;
std::map<pthread_t, bool>        teal::thread_release::threads_waiting;
std::map<pthread_t, std::string> teal::thread_release::thread_names;
pthread_cond_t                   teal::thread_release::rescan_thread_list = PTHREAD_COND_INITIALIZER;
pthread_mutex_t                  teal::thread_release::main_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t                  teal::thread_release::thread_name_mutex = PTHREAD_MUTEX_INITIALIZER;
std::string                      teal::thread_release::thread_being_created;


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
std::deque<vreg_match*> callbacks;


/*previous*/ bool teal::synch_chatty (bool new_value) {bool returned =synch_chatty_; synch_chatty_ = new_value; return returned;}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//This is the work part of the entry point from the verilog world.
//
int the_generic_callback (vreg_match* a_vreg_match)
{
    if (quitting_) return 0;
  if (synch_chatty_)   local_vout << teal_info << "teal thread_release begin from verilog callback." << endm;
  //ASSERT (a_vreg_match);

  vreg::invalidate_all_vregs (); //must be before the match() otherwise, the signal will never change!

  if (! a_vreg_match->match ()) {
    if (synch_chatty_) local_vout << teal_info << "teal thread_release no signal match returning from verilog callback." << *a_vreg_match << endm;
    return 0;
  }

  if (synch_chatty_)  local_vout << teal_info << "teal acc_callback vreg matches." << *a_vreg_match << endm;


  //actually have some processing to do.
  
  callbacks.push_back (a_vreg_match);


  if (callbacks.size() != 1) { //stacked callbacks?
    // local_vout << teal_info << "teal callback called within callback. Work is queued. " << endm;
    return 0;
  }

  while (callbacks.size ()) {
    vreg_match* the_vreg_match = callbacks.front ();
    vreg::invalidate_all_vregs ();  //in case a stacked callback changed signals that another wanted.
    //ASSERT (the_vreg_match->the_thread_release);
    the_vreg_match->the_thread_release->do_callback ();    
    //local_vout << teal_info << "teal thread_release returning from do_callback." << endm;
    callbacks.pop_front ();  //MUST save the pop until the end because this tells when we are done processing.
    //If we pop'ed it in the beginning, a callback because of a changed val in teh c++ land would not see us processing.
  }

  //    local_vout << teal_info << "teal thread_release returning from verilog callback." << endm;
  return 0;
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//This is the actual entry point from the verilog world.
//
#if defined (vpi_2_0)
int  the_vpi_callback (s_cb_data* the_s_cb_data)
{
  assert (the_s_cb_data);
  //  vout t("the vpi_callback");t  << teal_info << "teal vpi_callback called with reason." << the_s_cb_data->reason << " address is: " <<
  //			       (int)the_s_cb_data << " offset is " << (int)((char*)&(the_s_cb_data->reason) - (char*)the_s_cb_data) << endm;
#if 0
  if (the_s_cb_data->reason != cbValueChange) {
    local_vout << teal_error << "teal vpi_callback called with unknown reason." << the_s_cb_data->reason << " Ignored" << endm;
    return 0;
  }
#endif
  sim_time_  = ((uint64)the_s_cb_data->time->high << 32) + the_s_cb_data->time->low;

  if (synch_chatty_) {
    local_vout << teal_info << "time high is " << the_s_cb_data->time->high << " time low is " 
	       << the_s_cb_data->time->low 
	       << " type is " << the_s_cb_data->time->type << endm; 
  }
  return the_generic_callback ((vreg_match*)the_s_cb_data->user_data);
}

#else
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
int the_acc_callback (s_vc_record* the_s_vc_record)
{
  //ASSERT (the_s_vc_record);
  if (synch_chatty_)    local_vout << teal_info << "teal acc_callback called with reason." << the_s_vc_record->vc_reason << endm;
  if ( (the_s_vc_record->vc_reason != vregister_value_change) &&
       (the_s_vc_record->vc_reason != sregister_value_change) &&
       (the_s_vc_record->vc_reason != logic_value_change))    {
    local_vout << teal_error << "teal acc_callback called with unknown reason." << the_s_vc_record->vc_reason << " Ignored" << endm;
    return 0;
  }
  sim_time_  = ((uint64)the_s_vc_record->vc_hightime << 32) + the_s_vc_record->vc_lowtime;

  return the_generic_callback ((vreg_match*)the_s_vc_record->user_data);
}
#endif

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* teal::stop_thread (pthread_t id)
{
  //read up up on pthread cancel. would like to cause an exception in the thread
  //  local_vout << teal_info << "teal::stop_thread " << thread_name (id) << endm;
  int result = pthread_cancel (id);
  //  ASSERT (result == 0);
  local_vout << teal_info << "teal::stop_thread done cancel on " << thread_name (id) << " result is " << result << endm;
  void* returned;
  result = pthread_join (id, &returned);
  if (synch_chatty_)   local_vout << teal_info << "teal::stop_thread done join on " << thread_name (id) << " result is " << result << endm;
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
  //  local_vout << teal_info << "teal::stop_thread " << thread_name (id) << endm;
  //  ASSERT (result == 0);
  //   local_vout << teal_info << "teal::stop_thread done cancel on " << thread_name (id) << endm;
  void* returned;
  int result = pthread_join (id, &returned);
  local_vout << teal_info << "teal::stop_thread done join on " << thread_name (id) << " with result " << result << endm;
  //  assert (result == 0);
  if (result) {
    local_vout << teal_info << "join error of " << result << " on thread " << thread_name (id) << endm;
  }
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
#if 0
    if ((thread_name (it->first) != "Teal Control Thread") &&
	(thread_name (it->first) != "verification_top")) {
#else
    if ((thread_name (it->first) != "Teal Control Thread") &&
	(thread_name (it->first) != thread_name (pthread_self()))) {
#endif
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
  if (synch_chatty_) local_vout << teal_info << "Thread cleanup for at()" << endm;
  const sensitivity& s = *(sensitivity*) (context);
  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin());
       it != s.list.end(); ++it) {
    (*it)->the_thread_release = 0;
#if defined (vpi_2_0)
    vpi_remove_cb ((*it)->the_call_back);
    (*it)->the_call_back = 0;
#else
#if  defined (vcs) || defined (cver)
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
pthread_t teal::start_thread (user_thread thread, void* user_data, const std::string & name)
{
  pthread_t id;
  pthread_mutex_lock (&thread_release::main_mutex);
  thread_release::thread_being_created = name;
  int result = pthread_create (&id, NULL, thread, user_data);  
  local_vout << teal_info << "Thread " << name << " created. ID is " << hex << 
   thread_int (id) << " result " << result << endm;

  thread_release::thread_created_ (id, name);
  thread_release::thread_being_created = "";
  pthread_mutex_unlock (&thread_release::main_mutex);

  return id;
}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::finish () {
  quitting_ = true;
  //  teal::thread_release::print_threads_ ("teal::finish: will stop the following threads:");
  //    stop_all_threads ();
  //  pthread_mutex_lock (&thread_release::main_mutex); //stop the threads!
  //follow the standard return from callback logic
  pthread_mutex_lock (&thread_release::main_mutex); //stop the threads!
  thread_release::thread_waiting_ (pthread_self ());
    thread_release::really_all_waiting = false;
    do {
      //      local_vout << teal_info << "teal thread_release waiting for all waiting." << endm;
      pthread_cond_wait (&thread_release::all_waiting, &thread_release::main_mutex);
    } while (!thread_release::really_all_waiting);
      pthread_mutex_unlock (&thread_release::main_mutex); 

#if defined (vpi_2_0)
    vpi_control (vpiFinish);
#else 
    tf_dofinish ();
#endif
    //    local_vout << teal_error << "teal::finish(). After HDL finish called!!!" << endm;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::at (const sensitivity& s)
{
  if (synch_chatty_)    local_vout << teal_info << thread_name (pthread_self ()) << " teal::at() " << " begin " << endm;
  if (! s.list.size()) {
    local_vout << teal_error << "teal::at() called with no vregs! Ignored." << endm;
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
   //   no_time.type = vpiSuppressTime;
   no_time.type = vpiSimTime;
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
    //    local_vout << teal_info << "cb reason is " << cbValueChange << " addr is " << (int)(&call_back) << endm;
    //       local_vout << teal_info << " after vpi_register_cb of " << (int)(*it)->the_call_back << " handle " << (int)call_back.obj << teal::endm;
#else
#if defined (vcs) || defined (cver)
    acc_vcl_add ((*it)->get_handle(), (consumer_function)the_acc_callback, (char*) (*it), vcl_verilog);
#else
    acc_vcl_add ((*it)->get_handle(), the_acc_callback, (char*) (*it), vcl_verilog);
#endif
#endif
    if (synch_chatty_)    local_vout << teal_info << " after acc_vcl_add of " << (**it) << teal::endm;
  }

  pthread_mutex_lock (&thread_release::main_mutex);
  thread_release::thread_waiting_ (pthread_self ());

  pthread_cleanup_push (&thread_cleanup, (void*)&s);

  do {
    if (synch_chatty_)         local_vout << teal_info << " waiting on condition. " << teal::endm;
    pthread_cond_wait (&the_thread_release->condition, &thread_release::main_mutex);
    if (synch_chatty_) {
      local_vout << teal_info << "Thread " << thread_name (pthread_self()) << " done waiting on condition. " << 
                  the_thread_release->really_released << endm;
    }
  } while (! the_thread_release->really_released);

  pthread_cleanup_pop (0);

  pthread_mutex_unlock (&thread_release::main_mutex);

  if (synch_chatty_) local_vout << teal_info << "Thread " << thread_name (pthread_self()) << " really done waiting on condition. " << endm;

  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin());
       it != s.list.end(); ++it) {
    (*it)->the_thread_release = 0;
#if defined (vpi_2_0)
    vpi_remove_cb ((*it)->the_call_back);
    //    local_vout << teal_info << "Thread " << thread_name (pthread_self()) << " removed callback on. " << **it << endm;
    (*it)->the_call_back = 0;
#else
#if defined (vcs) || defined (cver)
    acc_vcl_delete ((*it)->get_handle(), (consumer_function)the_acc_callback, (char*) (*it), vcl_verilog);
#else
    acc_vcl_delete ((*it)->get_handle(), the_acc_callback, (char*) (*it), vcl_verilog);
#endif
#endif
  }

  delete the_thread_release;
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


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* run_top_thread (void*)
{
  verification_top ();
  return 0;
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* run_control_thread (void*)
{
  thread_release::control_thread ();
  return 0;
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// added while loop on 5/24/04 to fix intermittent hang on startup
void* main_watcher (void* c)
{
  pthread_t id = *(pthread_t*) c;
  void* returned;
  while (inhibit_time_read) { //hack to "know" when the system is initially stalled
    pthread_mutex_lock (&thread_release::main_mutex);
    //need the signal to rescan in case I missed the all_waiting. This happens when pthreads
    //decides to run the main user thread to an at() clause before me (pthread_self())
    pthread_cond_signal (&thread_release::rescan_thread_list);
    pthread_mutex_unlock (&thread_release::main_mutex);
    sched_yield (); //posix sleep thinge
  }
  pthread_join (id, &returned);
  thread_release::thread_completed (id);
  return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
int teal_top_internal (char*)
{
  pthread_attr_t attributes;
  pthread_attr_init (&attributes);
  pthread_t id;

  inhibit_time_read = true;
  local_vout.message_display (vlog::thread_name, false);
  local_vout << teal_info << "teal_top: This is version \"" << teal_version << "\". " << endm;

  local_vout << teal_info << "teal_top: Starting main thread. " << endm;


  pthread_t main_id = start_thread (run_top_thread, 0, "verification_top");
  local_vout << teal_info << "teal_top: Started main thread. " << thread_name (main_id) << endm;
  
  pthread_t result = start_thread (run_control_thread, 0, "Teal Control Thread");  
  pthread_mutex_lock (&thread_release::main_mutex);
  thread_release::thread_waiting_ (result);
  pthread_mutex_unlock (&thread_release::main_mutex);

  //  local_vout << teal_info << "Teal_top: Started control thread. id:" << (long)id << endm;
  
  pthread_create (&id, &attributes, main_watcher, (size_t*)&main_id);  
  local_vout << teal_info << "teal_top: Started main watcher thread. id:" << thread_int (main_id) << endm;
  
  pthread_mutex_lock (&thread_release::main_mutex);

  //  if (thread_release::threads_waiting.size ()) { 
  //    thread_release::print_threads_ ("Initial thread list:");
    while (! thread_release::really_all_waiting) {
      //need the signal to rescan in case I missed the all_waiting. This happens when pthreads
      //decides to run the main user thread to an at() cluse before me (pthread_self())
      //      pthread_cond_signal (&thread_release::rescan_thread_list);
      //  local_vout << teal_info << "Thread " << thread_release::thread_name_ (pthread_self()) << " waiting on start condition. " << endm;
      pthread_cond_wait (&thread_release::all_waiting, &thread_release::main_mutex);
      //      MARK;
      //    thread_release::print_threads_ ("Initial thread list after wakeup:");
    }
    //  }
  pthread_mutex_unlock (&thread_release::main_mutex);


  local_vout << teal_info << "teal_top: Starting Simulation. " << endm;
  inhibit_time_read = false;
  return 0;
}

#if defined (vpi_2_0) && (!defined (vcs))
//should this be in the unnamed namespace ?

///////////////////////////////////////////////
///////////////////////////////////////////////
void teal_top_register ()
{
  s_vpi_systf_data task_data = {0};
  task_data.type = vpiSysTask;
  task_data.tfname = "$verification_top";
#if defined (cver)
  task_data.calltf = (p_tffn) teal_top_internal;//0;
#else
  task_data.calltf = teal_top_internal;//0;
#endif
  task_data.compiletf = 0;//teal_top_internal;
  vpi_register_systf (&task_data);
}


#if 0

//specific to ncsim muck
extern "C" void teal_register_ncsim ();
void teal_register_ncsim () {

}
#endif

#endif

#if defined (cverX)
extern "C" void teal_top_call ();
void teal_top_call ()
#else
#if defined (ncsim)
extern "C" void teal_top_call ();
  void teal_top_call ()
#else
extern "C" void teal_top_call (int,int);
void teal_top_call (int user_data, int reason)
#endif
#endif
{
  teal_top_internal (0);
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
uint64 teal::vtime ()
{
  uint64 returned (0);
  return (sim_time_);

  //old way... actually call teh systm. 

#if !defined (teal_printf_io)
#  if defined (vpi_2_0)
#if defined (ncsim)
  returned = sim_time_;
#else
  //  int high(0);
  s_vpi_time here_and_now;
  here_and_now.type = vpiSimTime;
  vpi_get_time (0, &here_and_now);
  returned  = ((uint64)here_and_now.high << 32) + here_and_now.low;
#  endif
#  else
#if defined (ncsim)
  returned = sim_time_;
#else
  int high(0);
  int low = inhibit_time_read ? 0 : tf_getlongtime (&high);
  returned  = ((uint64)high << 32) + low;
#endif

#  endif
#endif

  if (0&&synch_chatty_) {
    //    local_vout << teal_info << " vtime " << returned  << " inhibit is " << inhibit_time_read << " " << endm;
    local_vout << teal_info << " vtime inhibit is " << inhibit_time_read << " " << endm;
  }
  return returned;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::condition::condition (const std::string& name)   : 
  name_ (name),
  signalled_ (false),
  time_at_signal_ (vtime())
{
  pthread_cond_init (&condition_, 0);

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
  if (synch_chatty_) local_vout << teal_info << "Sempahore thread cleanup. releasing main mutex" << endm;
  pthread_mutex_unlock (&thread_release::main_mutex);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::condition::wait ()
{
  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::wait() \"" << name_ << "\" begin " << endm;
  //WARNING: Currently (4/2004) cannot print thread_name() while the main mutex is held! The code will hang.

  if ((signalled_) && (vtime() >= time_at_signal_)) {
    signalled_ = false;
    //(taken out 5/24/04)    pthread_mutex_unlock (&thread_release::main_mutex);
    if (vtime() != time_at_signal_) local_vout << teal_info << "teal::condition \"" << name_ << "\" wait after signalled.Originally signalled at:" << teal::dec << time_at_signal_ << endm;
    return;
  }

  pthread_mutex_lock (&thread_release::main_mutex);

  waiting_.push_back (pthread_self ());
  thread_release::thread_waiting_ (pthread_self ());
  
  pthread_cleanup_push (&semaphore_thread_cleanup, 0);
  while (! signalled_) {
    //    local_vout << teal_info <<  " teal::condition::wait() \"" << name_ << "\" wait on condition " << endm;
    pthread_cond_wait (&condition_, &thread_release::main_mutex);
    //    local_vout << teal_info << " teal::condition::wait() " << name_ << " condition signalled " << endm;
  } 

  pthread_cleanup_pop (0);

  //local_vout << teal_info  << " teal::condition::wait() " << name_ << " wait signalled " << endm;

  signalled_ = false;
  waiting_.erase (std::find (waiting_.begin (), waiting_.end(), pthread_self()));
  thread_release::thread_running_ (pthread_self ());
  thread_release::allow_all_waiting = true; //since we are safe from issueing an all_waiting now.
  pthread_mutex_unlock (&thread_release::main_mutex);

  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::wait() " << name_ << " returning " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//TO DO MERGE WITH WAIT!
void teal::condition::wait_now ()
{
  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::wait() \"" << name_ << "\" begin " << endm;
  //WARNING: Currently (4/2004) cannot print thread_name() while the main mutex is held! The code will hang.

  if ((signalled_) && (vtime() == time_at_signal_)) {
    signalled_ = false;
    //(taken out 5/24/04)    pthread_mutex_unlock (&thread_release::main_mutex);
    local_vout << teal_info << "teal::condition \"" << name_ << "\" wait after signalled.Originally signalled at:" << teal::dec << time_at_signal_ << endm;
    return;
  }

  pthread_mutex_lock (&thread_release::main_mutex);

  waiting_.push_back (pthread_self ());
  thread_release::thread_waiting_ (pthread_self ());
  
  pthread_cleanup_push (&semaphore_thread_cleanup, 0);
  while (! signalled_) {
    //    local_vout << teal_info <<  " teal::condition::wait() \"" << name_ << "\" wait on condition " << endm;
    pthread_cond_wait (&condition_, &thread_release::main_mutex);
    //    local_vout << teal_info << " teal::condition::wait() " << name_ << " condition signalled " << endm;
  } 

  pthread_cleanup_pop (0);

  //local_vout << teal_info  << " teal::condition::wait() " << name_ << " wait signalled " << endm;

  signalled_ = false;
  waiting_.erase (std::find (waiting_.begin (), waiting_.end(), pthread_self()));
  thread_release::thread_running_ (pthread_self ());
  thread_release::allow_all_waiting = true; //since we are safe from issueing an all_waiting now.
  pthread_mutex_unlock (&thread_release::main_mutex);

  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::wait() " << name_ << " returning " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::condition::signal ()
{
  //    local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::signal() \"" << name_ << "\" begin " << endm;

  //after the pthread_cond_signal(), some thread will wake up and start running... So, we would like
  //to say thread_running_ (some_id). But which id? The OS picks (since the threads all
  //really waiting on a pthread_cond_t). So we need a backdoor to the control thread to inhibit
  //it issuing all waiting condition. Sigh. More complexity.
  if (waiting_.size ()) {
    //    local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition::signal() " << name_ << " allow_all_waiting false " << endm;
    thread_release::allow_all_waiting = false;
  }
  pthread_mutex_lock (&thread_release::main_mutex);
  signalled_ = true;
  time_at_signal_ = vtime();
  //pthread_cond_signal (&condition_);
  pthread_cond_broadcast (&condition_);
  pthread_mutex_unlock (&thread_release::main_mutex);

  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::condition " << name_ << " signal()  done " << endm;
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::mutex::mutex (const std::string& name) :
  name_ (name),
  condition_ (name),
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

  //   local_vout << teal_info << "teal::mutex " << name_ << " ctor  " << endm;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::mutex::~mutex ()
{
  //   local_vout << teal_info << "teal::mutex " << name_ << " dtor  " << endm;  
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
//     thread get to execute its pthread_unlock, but it will occur within this verilocal_vout callback.
//
void teal::mutex::lock ()
{
  //    local_vout << teal_info << thread_name (pthread_self ()) << " teal::mutex::lock \"" << name_ << "\"  begin " << endm;
  if (pthread_mutex_trylock (&mutex_)) {
    pthread_mutex_lock (&waiters_mutex_);
    waiters_++;
    //    local_vout << teal_info << " teal::mutex \"" << name_ << "\" lock begin wait. waiters_: " << waiters_ << endm;
    pthread_mutex_unlock (&waiters_mutex_);

    condition_.wait ();
    //    local_vout << teal_info << " teal::mutex " << name_ << " lock begin back from wait. waiters_: " << waiters_ << endm;
    while (pthread_mutex_trylock (&mutex_)) {
            sched_yield (); //posix sleep thinge
    }
    someone_running = true;
    //    local_vout << teal_info << " teal::mutex " << name_ << " lock back from signal. waiters_: " << waiters_ << endm;
  }
  //  local_vout << teal_info <<  thread_name (pthread_self ()) << " teal::mutex::lock " << name_ << " acquired. " << endm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::mutex::unlock ()
{
  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::mutex::unlock " << name_ << " begin " << endm;
  //  local_vout << teal_info << " teal::mutex " << name_ << " unlock begin " << endm;
    
  pthread_mutex_lock (&waiters_mutex_);

  if (waiters_) {
    waiters_--;
    pthread_mutex_unlock (&waiters_mutex_);
    someone_running = false; //ensure that someone other than me gets the mutex next 
    condition_.signal ();
    pthread_mutex_unlock (&mutex_);
    //local_vout << teal_info << thread_name (pthread_self ()) << " teal::mutex " << name_ << " after signal " << endm;
    while (! someone_running) {      
      sched_yield (); //posix sleep thinge
      //local_vout << teal_info << (int) (pthread_self ()) << " teal::mutex " << name_ << " waiting for someone else " << endm;
    };
  }
  else {
    pthread_mutex_unlock (&waiters_mutex_);
    pthread_mutex_unlock (&mutex_);
  }

  //  local_vout << teal_info << thread_name (pthread_self ()) << " teal::mutex::unlock " << name_ << " end lock released " << endm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::print_threads (const std::string& prefix) 
{
  pthread_mutex_lock (&teal::thread_release::main_mutex);
  teal::thread_release::print_threads_ (prefix);
  pthread_mutex_unlock (&teal::thread_release::main_mutex);
}

std::string teal::teal_version = "teal_1.50a";
