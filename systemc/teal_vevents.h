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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h

#if !defined (__teal_vevents_h__)
#define __teal_vevents_h__

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
//Note: maybe stickiness (done_) should be a bool in ctor?
  class condition  {
  public:
    explicit condition (const std::string name) : event_ (), name_ (name), done_ (false) {} 
      void signal () {done_ = true;event_.notify ();};
    void wait () {
#if defined(VM_SIZE)
      vout pm(name_);
      VM_SIZE(pm);
#endif
      //      if (done_) {done_ = false; return;}
      sc_core::wait (event_);
#if defined(VM_SIZE)
      VM_SIZE(pm);
#endif
    }
    
    std::string name ()const {return name_;}
    
  private:
    sc_core::sc_event event_;
    friend void wait (condition& ev);
    std::string name_;
    bool done_;
  };

  inline void wait (condition& ev) {return ev.wait ();}
  typedef condition event;

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class mutex  {
  public:
    mutex (const std::string& name) : name_ (name), unlock_ (name), process_ (0) {};

    void lock () {
      do {
	if (process_) {
	  unlock_.wait(); 
	}
	if (!process_) { //process will be set if someone else was waiting, woke up wih me, but got to run before me
	  process_ = sc_core::sc_get_curr_process_handle ();
	  assert (process_);
	}

      } while (process_ != sc_core::sc_get_curr_process_handle ());
    };

    void unlock () {
      if (!process_) {
	teal::vout log (name_); log << teal_fatal << "Cannot unlock because not ever locked!" << teal::endm;
	return;
      }
      if (sc_core::sc_get_curr_process_handle () != process_) {
	teal::vout log (name_); log << teal_fatal << "Cannot unlock because \"" 
				    << process_->name() << "\" locked it and I am \"" 
				    << sc_core::sc_get_curr_process_handle ()->name() << "\"" << teal::endm;
	return;
      }
      process_ = 0;
      unlock_.signal ();
    };

    const std::string name_;
  private:
    teal::condition           unlock_;
    sc_core::sc_process_b*    process_;
  };

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class mutex_sentry {
  public:
    mutex_sentry (mutex& m) : mutex_ (m) {mutex_.lock ();}
    virtual ~mutex_sentry () {mutex_.unlock ();};
  private:
    mutex& mutex_;
  };

#endif
