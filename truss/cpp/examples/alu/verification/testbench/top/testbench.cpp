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

#include "driver_agent.h"
#include "monitor_agent.h"
#include "generator_agent.h"

#include "checker.h"

#define truss_mark       log_ << teal_debug  << teal::endm

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
testbench::testbench (const std::string top_path) : truss::testbench_base ("testbench"), top_ (top_path),
						    reset_ (top_ + ".reset", 1, teal::vreg::observe_and_control),
						    clock_ (top_ + ".clock", 1, teal::vreg::observe_only)

    {
      log_.show_debug_level (teal::debug);

      log_ << teal_debug << "testbench new() begin Top is: \"" << top_ << "\"" << teal::endm;

      truss::channel<alu::operation>* to_chip = new truss::channel<alu::operation>   ("alu to_chip");


      truss::port<alu::driver_configuration::signals>::pins driver_wires;
      driver_wires [alu::driver_configuration::operand_a] = top_ + ".operand_a";
      driver_wires [alu::driver_configuration::operand_b] = top_ + ".operand_b";
      driver_wires [alu::driver_configuration::op_code] = top_ + ".operand";
      driver_wires [alu::driver_configuration::op_valid] = top_ + ".op_valid";
      driver_wires [alu::driver_configuration::op_done] = top_ + ".operation_done";


      driver                       = new alu::driver_agent       ("alu_driver_0", driver_wires, to_chip);

      truss_mark;
      generator                 = new alu::generator_agent ("driver_generator_0", to_chip);

      truss_mark;

      truss::channel<alu::operation>* to_chip_copy = new truss::channel<alu::operation> ("alu to_chip checker");
      to_chip->add_listner (to_chip_copy);

      truss::channel<teal::uint32>* from_chip = new truss::channel<teal::uint32>   ("alu from chip");
      truss::port<alu::monitor_configuration::signals>::pins monitor_wires;
      log_ << teal_debug  << teal::endm;

      monitor_wires [alu::monitor_configuration::operation_done] = top_ + ".operation_done";
      monitor_wires [alu::monitor_configuration::result] = top_ + ".result";

      log_ << teal_debug  << teal::endm;
      monitor                     = new alu::monitor_agent ("alu_monitor_0",   monitor_wires, from_chip);
      checker                     = new alu::checker ("alu_checker_0",   to_chip_copy, from_chip);

      log_ << teal_debug << "testbench new() end " << teal::endm;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   testbench::~testbench () {
    delete driver;
    delete generator;
    delete monitor;
    delete checker;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void testbench::time_zero_setup () {
    log_ << teal_debug << "time zero setup" << teal::endm;
    reset_ = 0;
  };

const teal::uint32 reset_count = 10;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void testbench::out_of_reset (reset r) {
     log_ << teal_debug << "out of reset clock is: " << clock_ << teal::endm;
    reset_ = 1;
    for (teal::uint32 i(0); i < reset_count; ++i) {
      log_ << teal_info << " reset clock count " << i << teal::endm;
      teal::at (teal::posedge (clock_));
    }
    reset_ = 0;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void testbench::start () { 
    log_ << teal_debug << "start" << teal::endm;
    monitor->start ();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void testbench::stop () {
    log_ << teal_debug << "stop" << teal::endm;
    monitor->stop ();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void testbench::randomize ()         {log_ << teal_debug << "randomize" << teal::endm;  }
   void testbench::write_to_hardware () {log_ << teal_debug << "write to hardware" << teal::endm;  }
   void testbench::wait_for_completion () {  log_ << teal_debug << "wait for completion" << teal::endm;  }
   void testbench::report (const std::string prefix) const {log_ << teal_debug << prefix << " report" << teal::endm;  }

