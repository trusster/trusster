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

`include "testbench.svh"

`include "driver_agent.svh"
`include "monitor_agent.svh"
`include "generator_agent.svh"

`include "checker.svh"

`include "interfaces_alu.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function  testbench::new ( string top_path, truss::interfaces_dut dut_base);
   interfaces_alu alu_dut;
   
   super.new (top_path, dut_base);
   log_.show_debug_level (teal::debug);

   log_.debug ( "testbench new() begin " );
   begin
      int foo = $cast (alu_dut, dut_base);
      assert (foo);
   end

   begin
      alu_channel to_chip = new ("alu to_chip");
      alu_channel to_chip_copy = new ("alu to_chip checker");
      alu_generator_agent gen_arrgh = new  ("driver_generator_0", to_chip);
//      alu_driver_agent driver_arrgh = new ("alu_driver_0", interfaces_alu.alu_input_1, to_chip);
      alu_driver_agent driver_arrgh = new ("alu_driver_0", alu_dut.alu_input_1, to_chip);

      truss::int_channel from_chip = new ("alu from chip");
//      alu_monitor_agent monitor_arrgh = new ("alu_monitor_0", interfaces_alu.alu_output_1, from_chip);
      alu_monitor_agent monitor_arrgh = new ("alu_monitor_0", alu_dut.alu_output_1, from_chip);
      checker = new ("alu_checker_0", to_chip_copy, from_chip);
      
      generator = gen_arrgh;
      driver = driver_arrgh;
      monitor = monitor_arrgh;
      
      to_chip.add_listner (to_chip_copy);

      top_reset_ = alu_dut.top_reset_;
      
      log_.debug ( "testbench new() end " );
   end
 endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task testbench::time_zero_setup ();
    log_.debug ( "time zero setup" );
    top_reset_.resetr = 0;
endtask

parameter int reset_count = 10;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task testbench::out_of_reset (truss::reset r);
    log_.debug ( "Begin: out of reset" );
    top_reset_.resetr = 1;
   for (int i = 0; i < reset_count; ++i)  @ (posedge (top_reset_.clock));
    top_reset_.resetr = 0;
    log_.debug ( "Done: out of reset" );
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task testbench::start () ;
   log_.debug ( "start" );
   monitor.start ();
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function void testbench::randomize2 ();         log_.debug ( "randomize" );  endfunction
   task testbench::write_to_hardware (); log_.debug ( "write to hardware" );  endtask
   task testbench::wait_for_completion ();   log_.debug ( "wait for completion" );  endtask
   function void testbench::report ( string prefix);  log_.debug ({prefix, " report"});  endfunction

