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

#include "testbench.h"

#include "truss.h"


#include "uart_16550_configuration.h"
#include "uart_interface.h"

#include "wishbone_driver.h"
#include "wishbone_memory_bank.h"


#define uart_registers_first 0
#define uart_registers_last ((number_of_uarts - 1)<<4) + 7

using namespace truss;

const teal::uint64 UART_CLOCK_FREQUENCY =  29489826; //29.4Mhz

testbench::testbench (const std::string top_path) : truss::testbench_base ("testbench"), top_ (top_path)
{
  log_ << teal_debug << "testbench new() begin " << teal::endm;

  for (teal::uint32 i(0); i < number_of_uarts; ++i) {
    uart_interface[i] = new uart::interface (top_path, i);
  }

  //Now for the main chip register interface
  port <wishbone::configuration::signals>::pins  wishbone_port;
  //  wishbone_port [wishbone::configuration::reset] = top_ + ".wb_rst_ir";
  wishbone_port [wishbone::configuration::reset] = top_ + ".wb_rst";
    wishbone_port [wishbone::configuration::clock] = top_ + ".wb_clock";
    //wishbone_port [wishbone::configuration::clock] = top_ + ".wb_clockr";
  wishbone_port [wishbone::configuration::address] = top_ + ".wishbone_driver.address";
  wishbone_port [wishbone::configuration::data] = top_ + ".wishbone_driver.data";
  wishbone_port [wishbone::configuration::select] = top_ + ".wishbone_driver.select";
  wishbone_port [wishbone::configuration::op_code] = top_ + ".wishbone_driver.op_code";
  wishbone_port [wishbone::configuration::do_work] = top_ + ".wishbone_driver.do_work";
  wishbone_port [wishbone::configuration::work_done] = top_ + ".wishbone_driver.work_done";

  wishbone_driver_ = new wishbone::wishbone_driver ("WB", wishbone_port);
  teal::memory::add_memory_bank (new wishbone::wishbone_memory_bank ("Wishbone", "main_bus", wishbone_driver_));
  teal::memory::add_map ("main_bus", uart_registers_first, uart_registers_last);

  log_ << teal_debug << "testbench new() done " << teal::endm;
}

testbench::~testbench () {
  delete uart_interface;
  delete wishbone_driver_;
}

void testbench::time_zero_setup () {
  //  wishbone_driver_->reset = 0;
  log_ << teal_debug << "testbench reset() done " << teal::endm;
};

void testbench::out_of_reset (reset r) {
  wishbone_driver_->pause (10);
  wishbone_driver_->reset = 1;
  wishbone_driver_->pause (10);
  wishbone_driver_->reset = 0;
  log_ << teal_debug << "testbench out_of_rest() done " << teal::endm;
}

void testbench::randomize () {
  for (teal::uint32 i(0); i < number_of_uarts; ++i) {
        uart_interface[i]->uart_configuration->randomize ();
  }
  wishbone_driver_->start ();
}

void testbench::write_to_hardware () {
}

void testbench::start () {
}

void testbench::stop () {}

//no wait_for_completion, let the test/exercisors do that
void testbench::wait_for_completion () {}

void testbench::report (std::string prefix) const {
  for (teal::uint32 i(0); i < number_of_uarts; ++i) {
    uart_interface[i]->uart_configuration->report (prefix);
  }
}

