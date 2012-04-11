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

`include "truss.svh"

`include "testbench.svh"

`ifndef TEST
`error "TEST undefined! cannot build a test!"
`endif


`ifndef SEED
 `define SEED 1
`endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
program verification_top ();

   //USER MUST IMPLEMENT!!!!
   `include "build_interfaces.svh"
     
initial begin
   truss::shutdown a_shutdown;
   string test_name = `TEST_NAME;  //define coming in from truss script
   string file_name = teal::dictionary_find_on_command_line ("dictionary", {test_name, ".cfg"});
   string top;
   testbench testbench_0;
   truss::watchdog  watchdog_0;
   `TEST test_0;
   teal::vout log;
   
   log = new ("verification_top:"); 
   log.show_debug_level (99);

   teal::dictionary_read (file_name);   //In code, use deafult to allow the read in values to stand
   a_shutdown = new ("Shutdown"); //ptrs set up a few lines down, after all built

   begin
      teal::file_vlog not_used = new (teal::dictionary_find ("out_file"), teal::dictionary_find_integer ("interactive", 1));
      truss::error_limit_vlog not_used_2 = new (teal::dictionary_find_integer ("error_limit", 10), a_shutdown);
   end
   

   log.info ({"This is truss version \"", truss::version(), "\""});

   top = teal::dictionary_find_on_command_line ("truss_hdl_top", "top");
   if (top == "") top = "top";

   testbench_0 = new (top, build_interfaces ());
`ifdef virtual_interfaces_in_packages
   watchdog_0 = new  ("watchdog", real_interfaces.watchdog_interface_0, a_shutdown);   //YOU PROVIDE real_interfaces!!!
`else
   watchdog_0 = new  ("watchdog", a_shutdown);
`endif
   test_0 = new (testbench_0, watchdog_0, teal::dictionary_find_on_command_line ("test_name", test_name)); 

   //now patch pointers. Shutdown was needed in the error limit earlier
   a_shutdown.test_ = test_0;
   a_shutdown.testbench_ = testbench_0;
   a_shutdown.watchdog_ = watchdog_0;

   log.debug ({"Using dictionary file: ", file_name});

   teal::vrandom_init_with_seed (teal::dictionary_find_integer ("seed", `SEED));
   begin
      string msg;
      msg = $psprintf ("Using seed: %0d",  teal::dictionary_find_integer ("seed", `SEED));
      log.info (msg);
   end
   
   log.debug ("Phase: randomize");

   test_0.randomize2 ();          //first to allow to setup testbench
   testbench_0.randomize2 ();
   watchdog_0.start ();

   log.debug ("Phase:time_zero_setup");

   testbench_0.time_zero_setup ();
   watchdog_0.time_zero_setup ();
   test_0.time_zero_setup ();  //test last to be able to undo incorrect testbench setup

   log.debug ("Phase: out_of_reset");

   testbench_0.out_of_reset (truss::cold);
   watchdog_0.out_of_reset (truss::cold);
   test_0.out_of_reset (truss::cold);

   log.debug ("Phase: write_to_hardware");

   testbench_0.write_to_hardware ();
   watchdog_0.write_to_hardware ();
   test_0.write_to_hardware ();

   log.debug ("Phase: start");   

   testbench_0.start ();
   test_0.start ();

   log.debug ("Phase: wait_for_completion");   

   testbench_0.wait_for_completion ();
   test_0.wait_for_completion ();

   log.debug ("Phase: final_report");   
   a_shutdown.shutdown_now ("Final Report: ");
end // initial begin
   endprogram
     
