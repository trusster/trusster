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

`include "block_uart.svh"
`include "uart_basic_test_component.svh"

`include "uart_bfm.svh"
`include "uart_16550_sfm.svh"
`include "uart_generator.svh"
`include "uart_checker.svh"
`include "uart_16550_configuration.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function block_uart::new (testbench tb, truss::watchdog w, string n);  
   super.new (n, w);
   log_.info ("uart_test new() begin ");
   
   testbench_ = tb;
   uart_test_component_ingress_ = new ("uart_test_component_ingress", tb.uart_ingress_generator,  tb.uart_program_sfm,  tb.uart_ingress_checker);
   uart_test_component_egress_ = new ("uart_test_component_egress",  tb.uart_egress_generator, tb.uart_protocol_bfm, tb.uart_egress_checker);

  //add configuration default constraints
  teal::dictionary_put ({tb.uart_configuration.name, "_min_baud"}, "4800",    teal::default_only);
  teal::dictionary_put ({tb.uart_configuration.name, "_min_data_size"}, "5",  teal::default_only);
  teal::dictionary_put ({tb.uart_configuration.name, "_max_data_size"}, "8", teal::default_only);

  //add generator default constraints
  teal::dictionary_put ({tb.uart_egress_generator.name, "_min_word_delay"}, "1", teal::default_only);
  teal::dictionary_put ({tb.uart_egress_generator.name, "_max_word_delay"}, "1", teal::default_only);
  teal::dictionary_put ({tb.uart_ingress_generator.name, "_min_word_delay"}, "1",  teal::default_only);
  teal::dictionary_put ({tb.uart_ingress_generator.name, "_max_word_delay"}, "1",  teal::default_only);
   log_.info ("uart_test new() end ");
endfunction // new

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task block_uart::time_zero_setup (); uart_test_component_egress_.time_zero_setup ();endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task block_uart::out_of_reset (truss::reset r); uart_test_component_egress_.out_of_reset (r);endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void block_uart::randomize2 (); uart_test_component_egress_.randomize2 (); uart_test_component_ingress_.randomize2 ();endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task block_uart::write_to_hardware (); 
  uart_test_component_egress_.write_to_hardware ();
  uart_test_component_ingress_.write_to_hardware ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task block_uart::start ();
   fork
      uart_test_component_ingress_.start ();
      uart_test_component_egress_.start ();
   join_none
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task block_uart::wait_for_completion (); 
  uart_test_component_ingress_.wait_for_completion (); 
  uart_test_component_egress_.wait_for_completion ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void block_uart::report (string prefix);
  uart_test_component_ingress_.report (prefix); 
  uart_test_component_egress_.report (prefix);
endfunction
  
