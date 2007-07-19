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


`include "test_component.svh"

`include "driver.svh"
`include "generator.svh"
`include "checker.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 function alu_test_component::new (string n, alu_generator g, alu_driver b, alu_checker c);
    super.new (n);
    alu_generator_ = g;
    alu_driver_ = b;
    alu_checker_ = c;
    log_.show_debug_level (teal::debug); 
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task alu_test_component::time_zero_setup ();
  log_.debug (" time_zero_setup");
  alu_driver_.time_zero_setup ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task alu_test_component::out_of_reset (truss::reset r);
  log_.debug (" out_of_reset");
  alu_driver_.out_of_reset (r);
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 function void alu_test_component::randomize2 ();
   bit [7:0] min_words = teal::dictionary_find_integer ({name_, "_min_num_operations"}, 10);
   bit [7:0] max_words = teal::dictionary_find_integer ({name_, "_max_num_operations"}, 15);
  number_of_operations_ = get_number_of_operations (min_words, max_words);
  log_.debug (" randomize2");
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task alu_test_component::write_to_hardware ();
  log_.debug (" time_zero_setup");
  alu_driver_.write_to_hardware ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task alu_test_component::wait_for_completion_ ();
  log_.debug (" internal wait for completion ");
  alu_checker_.wait_for_completion ();
   endtask



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task alu_test_component::start_components_ ();
  log_.debug ("Starting components.");
  alu_driver_.start (); 
  alu_checker_.start ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task alu_test_component::generate2 ();
      string msg;
      msg = $psprintf ("alu_test_component::generate number of opertaions is %0d", number_of_operations_);
      log_.debug (msg);
      alu_generator_.do_operations (number_of_operations_);
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit [7:0] get_number_of_operations (bit [7:0] min_v, bit [7:0] max_v);
    bit [7:0] returned;
    `RAND_RANGE (returned, min_v, max_v) 
    return returned;
  endfunction
