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
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This class is used to signal between threads. Just like
  //pthread's condition, except that the teal system knows that the
  //thread is waiting.
  //
  class condition {
  public:
    condition (const std::string& name);
    virtual ~condition ();

    virtual void wait ();
    //as above, but do not consider signals that have occured prior to the current simulation time
    virtual void wait_now ();
    virtual void signal ();
    const std::string name_;

  protected:
    bool signalled_;
    uint64 time_at_signal_;
    pthread_cond_t condition_;
    std::deque<pthread_t> waiting_;

  private:
    condition& operator= (const condition&) ;
    condition (const condition&);
  };


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class mutex {
  public:
    mutex (const std::string& name);
    virtual ~mutex ();

    virtual void lock ();
    virtual void unlock ();

    const std::string name_;

  protected:
    condition condition_;
    uint32 waiters_;
    pthread_mutex_t mutex_;
    pthread_mutex_t waiters_mutex_;
    bool someone_running;
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

