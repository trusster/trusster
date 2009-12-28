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

#include "truss_watchdog.h"

using namespace teal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
truss::watchdog::watchdog (const std::string& name, const std::string path, shutdown* a_shutdown) : 
  verification_component (name), thread (name), 
  shutdown_ (a_shutdown), hdl_timeout_ (path + ".timeout", teal::vreg::observe_and_control), 
  hdl_timeout_count_ (path + ".counter", teal::vreg::observe_and_control),
  timeout_occurred_ (false), hdl_timeout_occurred_ (false)
{}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void truss::watchdog::report (const std::string prefix) const {
   if (timeout_occurred_) {
     log_ << teal_error
	  << (hdl_timeout_occurred_ ? "HDL timeout occurred." : "Test-based shutdown occurred") << teal::endm;
   }
   else {
     log_ << teal_debug << " Watchdog not triggerred. Normal test completion." << teal::endm;
   }
 }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void truss::watchdog::start_ () {
  //set the the timeout value
  uint32 value = teal::dictionary::find (name + "_timeout", 10000000);
  teal::vout x (name);
  //  x << teal_info << "  setting " << teal::dec << hdl_timeout_count_ << " to "  << value << teal::endm;
  x << teal_info << "  Using Timeout of " << teal::dec << value << teal::endm;
  hdl_timeout_count_ = value;
  at (posedge (hdl_timeout_));
  hdl_timeout_occurred_ = true;
  shutdown_now ("HDL Timeout!!!");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void truss::watchdog::shutdown_now (const std::string prefix) 
{
  timeout_occurred_ = true;
  shutdown_->shutdown_now (prefix);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void truss::watchdog::wait_for_completion () {  
  at (posedge (hdl_timeout_));
}
