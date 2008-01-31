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

#include "wishbone_driver.h"

using namespace teal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
wishbone::wishbone_driver::wishbone_driver (const std::string& n, truss::port <configuration::signals>::pins  p) :
    reset (p[configuration::reset]),
    name (n), 
    clock_ (p[configuration::clock]),
    address_ (p[configuration::address]),
    data_ (p[configuration::data]),
    select_ (p[configuration::select]),
    op_code_ (p[configuration::op_code]),
    do_work_ (p[configuration::do_work]),
    work_done_ (p[configuration::work_done]),
    mutex_ (n),
    log_ (n)
    {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void wishbone::wishbone_driver::pause (teal::uint32 count) {
    for (teal::uint32 i (count); i; --i) {
      teal::at (posedge (clock_));
    }
  }
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void wishbone::wishbone_driver::write8 (teal::uint32 a, const teal::reg& d) {
    log_ << teal_info << "about to get mutex Addr " << hex << a << " data " << d << endm;
    teal::mutex_sentry guard (mutex_);
    log_ << teal_info << "about to execute got mutex Addr " << hex << a << " data " << d << endm;
    op_code_ = 0;
    address_ = a;
    //unclear if the following code should be in the verilog or not, woudl have to send size down
    //and tehre would be some make length set by the verilog reg
    if (d.bit_length () <= 8) {  //put the data on the right line
      switch (a % 4) {
      case 0: {select_ = 1; data_ = d; break;}
      case 1: {select_ = 2; data_ = d << 8; break;}
      case 2: {select_ = 4; data_ = d << 16; break;}
      case 3: {select_ = 8; data_ = d << 24; break;}
      }
    }
    else {
      truss_assert (d.bit_length() == 64); //have to code the 16 case or maybe even multi write case
      select_ = 0xF;
      data_ = d;
    }
    do_work_ = 1;           //signal to verilog
    //    log_ << teal_info << "opcode set wait for work_done:" << work_done << endm;
    teal::at (posedge (work_done_)); //wait for ack
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  teal::reg wishbone::wishbone_driver::read8 (teal::uint32 a) 
    {
      teal::reg returned(0,32);
    teal::mutex_sentry guard (mutex_);
    //        log_ << teal_info << "about to execute got mutex " << do_work << endm;
    op_code_ = 1;
    address_ = a;
    if (1 || returned.bit_length () <= 8) {  //put the data on the right line
      switch (a % 4) {
      case 0: {select_ = 1; break;}
      case 1: {select_ = 2; break;}
      case 2: {select_ = 4; break;}
      case 3: {select_ = 8; break;}
      }
    }
    else {
      select_ = 0xF;
    }
    do_work_ = 1;           //signal to verilog
    //    log_ << teal_info << "opcode set wait for work_done:" << work_done << endm;
    teal::at (posedge (work_done_)); //wait for ack
    returned = data_;
    //    log_ << teal_info << "Read returned :" << returned << endm;
    if (1 || returned.bit_length () <= 8) {  //put the data on the right line
      switch (a % 4) {
      case 0: break;
      case 1: {returned = returned >> 8; break;}
      case 2: {returned = returned >> 16; break;}
      case 3: {returned = returned >> 24; break;}
      }
    }
    log_ << teal_info << "Read returned (after shift) :" << returned << endm;
    return (returned);
  }
