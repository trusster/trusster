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
#ifndef __tthread__
#define __tthread__


namespace truss {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////// simple thread  //////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class thread {
  public:
    thread (const std::string n) : name_ (n), id (0) {}
    virtual ~thread () {/*assure stopped?*/}

    virtual void start () {id = teal::start_thread (local_run, this, name_);}
    virtual void stop () {if (id) {teal::stop_thread (id);} else {teal::join_thread (id);}} 

  protected:
    std::string name_; //allow to change during or after ctor
    virtual void start_ () = 0;
    pthread_t id;

  private:
    thread (const thread&); //no implementation, because disallow copy
    void operator= (const thread&); //no implementation, because disallow assignment

    static void* local_run (void* c) {
      thread* rl = static_cast <thread*> (c);
      rl->start_ (); 
      rl->id = 0;
      teal::note_thread_completed ();
      return 0;
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////// run loop  //////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class run_loop {
  public:
    run_loop ( const std::string name) :
      name_ (name), thread_running_ (false) {}
    virtual ~run_loop () {}

    virtual void start () {id = teal::start_thread (local_run, this, name_); thread_running_ = true;}
    virtual void stop () {if (thread_running_) {teal::stop_thread (id);} else {teal::join_thread (id);}}

  protected:
    virtual void loop_condition_ () = 0;
    virtual bool loop_body_ () = 0;

    const std::string name_;
    pthread_t id;
    bool thread_running_;

  private:
    run_loop (const run_loop&); //no implementation
    void operator= (const run_loop&); //no implementation

    static void* local_run (void* c) {
      run_loop* rl = (run_loop*) c;
      for (;;) {
	rl->loop_condition_ ();
	if (!rl->loop_body_ ()) break;
      }
      return 0;
    }
  };


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////  MULTI  THREAD  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  class internal_mutex_sentry {
  public:
    internal_mutex_sentry (pthread_mutex_t* m) : mutex_ (m)
      { pthread_mutex_lock (mutex_); }

    ~internal_mutex_sentry ()
      { pthread_mutex_unlock (mutex_); }

  private:
    pthread_mutex_t* mutex_;
  };

  //put into wt_threads.h?
  //sort of like mem_fun_t
  template <class data_type>
    struct mt_helper {
      typedef void (data_type::*m) ();
 
      data_type* object;
      m the_method;
      std::string name;
    };


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class multi_thread {
  public:
    multi_thread (const std::string n) : name_ (n) {pthread_mutex_init (&mutex_, 0);}
    virtual ~multi_thread () {/*assure stopped?*/}

    virtual void stop (const std::string& n) {
      if (threads_[name_ + n]) {
	teal::stop_thread (threads_[name_ + "::" + n]);
      } else {
	teal::join_thread (threads_[name_ + "::" + n]);
      }
    }

  protected:
    const std::string name_; 

    template <class data_type>
      void run (void (data_type::*method) (), const std::string& n) {
      internal_mutex_sentry foo (&mutex_);
      mt_helper<data_type>* mt = new mt_helper <data_type> (); 
      mt->object = (data_type*) this; 
      mt->the_method = method; 
      mt->name =  name_ + "::" + n;
      threads_[mt->name] = teal::start_thread (local_run<data_type>, mt, mt->name);
    }
  
  private:
    std::map < std::string, pthread_t> threads_;
    pthread_mutex_t mutex_; //to protect the map

    multi_thread (const multi_thread&); //no implementation, because disallow copy
    void operator= (const multi_thread&); //no implementation, because disallow assignment

    template <class data_type>
      static void* local_run (void* c) {
      mt_helper<data_type>* mt = static_cast < mt_helper < data_type> * >(c);
      data_type* object = mt->object;
      typename mt_helper<data_type>::m method = mt->the_method;
      (object->*method) (); //ugly, pure ugly
      mt->object->threads_[mt->name] = 0;
      teal::note_thread_completed ();
      //delete mt ???
      return 0;
    }
  };

};

#endif
