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
#ifndef __watchdog__
#define __watchdog__

#include "teal.h"

#include "truss_verification_component.h"
#include "truss_thread.h"

#include "truss_shutdown.h"

namespace truss {
  class watchdog : public verification_component, public thread {
  public:
    watchdog (const std::string& name, const std::string path, shutdown* s);
    ~watchdog () {};

    virtual void time_zero_setup () {};
    virtual void out_of_reset (reset) {};
    virtual void randomize () {};
    virtual void write_to_hardware () {};
    virtual void start () {thread::start ();};
    virtual void stop () {thread::stop ();};
    virtual void wait_for_completion ();

    virtual void report (const std::string prefix) const;

    //ROBERT?    void assure_wallclock (teal::uint32 milli_seconds);  //or time_t sturcture?

    //for remote or other watch dog mechanisms to create a timeout
    void shutdown_now (const std::string prefix);

  protected:
    void start_ ();
    shutdown* shutdown_;
    teal::vreg hdl_timeout_;
    teal::vreg hdl_timeout_count_;
    bool timeout_occurred_;
    bool hdl_timeout_occurred_;

  private:
    watchdog (const watchdog&);
    void operator= (const watchdog&);
  };
};
#endif
