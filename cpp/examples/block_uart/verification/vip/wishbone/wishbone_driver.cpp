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
  truss::thread (n), 
  reset (p[configuration::reset], 1, teal::vreg::observe_and_control),
  name (n), 
  clock_ (p[configuration::clock], 1, teal::vreg::observe_only),
  address_ (p[configuration::address], 32, teal::vreg::observe_and_control),
  data_ (p[configuration::data],       32, teal::vreg::observe_and_control),
  select_ (p[configuration::select],    4, teal::vreg::observe_and_control),
  op_code_ (p[configuration::op_code],  2, teal::vreg::observe_and_control),
  do_work_ (p[configuration::do_work],  1, teal::vreg::observe_and_control),
  work_done_ (p[configuration::work_done], 1, teal::vreg::observe_only),
  mutex_ (n),
  log_ (n),  internal_do_work_(n), internal_done_ (n)
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void wishbone::wishbone_driver::pause (teal::uint32 count) {
  for (teal::uint32 i (count); i; --i) {
    log_ << teal_info << "Pause at count " << i << teal::endm;
    teal::at (posedge (clock_));
  }
}
  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  wishbone::wishbone_driver::start_ ()
{
  //    log_ << teal_info << "starting " << teal::endm;
  do {
    internal_do_work_.wait();
    //log_ << teal_info << "saw do work " << teal::endm;
    if (internal_read_) internal_data_ = read8_ (internal_address_); else write8_ (internal_address_, internal_data_);
    internal_done_.signal ();
  } while (1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::reg wishbone::wishbone_driver::read8 (teal::uint32 a) 
{
  teal::mutex_sentry guard (mutex_);
  internal_read_ = true;
  internal_address_ = a;
  internal_data_.resize (8);
  internal_do_work_.signal ();
  internal_done_.wait ();
  teal::reg returned(0,32);  
  returned = internal_data_;
  return returned;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void wishbone::wishbone_driver::write8 (teal::uint32 a, const teal::reg& d) {
  teal::mutex_sentry guard (mutex_);
  log_ << teal_info << "begin write a: " << a << teal::endm;
  internal_read_ = false;
  internal_address_ = a;
  internal_data_.resize (d.bit_length ());
  internal_data_ = d.to_int();
  internal_do_work_.signal ();
  //    log_ << teal_info << "wait for done " << teal::endm;
  internal_done_.wait ();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void wishbone::wishbone_driver::write8_ (teal::uint32 a, const teal::reg& d) {

  teal::at(posedge(clock_));
  log_ << teal_info << "wishbone_driver::write() about to execute got mutex Addr " << hex << a << " data " << d << " do work " << do_work_ << endm;
  op_code_ = 0;
  address_ = a;
  //unclear if the following code should be in the verilog or not, woudl have to send size down
  //and there would be some make length set by the verilog reg

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
  do_work_ = 0;           //signal to verilog
  do_work_ = 1;           //signal to verilog
  log_ << teal_info << "opcode set wait for work_done:" << work_done_ << endm;
  teal::at (posedge (work_done_)); //wait for ack
  log_ << teal_info << "opcode set done wait for work_done:" << work_done_ << endm;
    

  //systemc cleanup all drivers
  reg all_zs = reg(0,32);
  for (int i(0); i < 32; ++i) all_zs(i,i) = teal::reg::Z;
  data_ = all_zs;
  address_ = all_zs;
  op_code_ = all_zs;  
  do_work_ = teal::reg::Z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::reg wishbone::wishbone_driver::read8_ (teal::uint32 a) 
{
  teal::reg returned(0,32);
  teal::at(posedge(clock_));
  log_ << teal_info << "wishbone_driver::read() about to execute got mutex Addr " << hex << a << endm;
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
  do_work_ = 0;           //signal to verilog
  do_work_ = 1;           //signal to verilog
  //    log_ << teal_info << "opcode set wait for work_done:" << work_done << endm;
  teal::at (posedge (work_done_)); //wait for ack
  returned = data_;
  log_ << teal_info << "Read returned :" << returned << endm;
  if (1 || returned.bit_length () <= 8) {  //put the data on the right line
    switch (a % 4) {
    case 0: break;
    case 1: {returned = returned >> 8; break;}
    case 2: {returned = returned >> 16; break;}
    case 3: {returned = returned >> 24; break;}
    }
  }
  log_ << teal_info << "Read returned (after shift) :" << returned << endm;
  do_work_ = teal::reg::Z;  
  reg all_zs = reg(0,32);
  for (int i(0); i < 32; ++i) all_zs(i,i) = teal::reg::Z;
  address_ = all_zs;
  op_code_ = all_zs;  
  return (returned);
}
