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

`include "teal.svh"

`include "quad_uart_irritators.svh"
`include "uart_basic_test_component.svh"
`include "uart_basic_irritator.svh"

`include "uart_bfm.svh"
`include "uart_16550_sfm.svh"
`include "uart_generator.svh"
`include "uart_checker.svh"
`include "uart_16550_configuration.svh"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void quad_uart_irritators::standard_configuration (string name) ;
   //add configuration default constraints
   teal::dictionary_put ({name, "_min_baud"}, "4800",    teal::default_only);
   //    teal::dictionary_put ({name, "_max_baud"}, "19200",    teal::default_only);
   teal::dictionary_put ({name, "_max_baud"}, "9600",    teal::default_only);
   teal::dictionary_put ({name, "_min_data_size"}, "5",  teal::default_only);
   teal::dictionary_put ({name, "_max_data_size"}, "8", teal::default_only);
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void quad_uart_irritators::standard_generator (string name) ;
   //add generator default constraints
   teal::dictionary_put ({name, "_min_word_delay"}, "1", teal::default_only);
   teal::dictionary_put ({name, "_max_word_delay"}, "1", teal::default_only);
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function quad_uart_irritators::new (testbench tb, truss::watchdog w, string n);
   super.new (n,w);
   testbench_ = tb;
   `truss_assert (number_of_uarts >= 2); 
endfunction



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void quad_uart_irritators::randomize2 ();
   string msg;
   int foo;
   int egress_count = 0;
   int ingress_count = 0;

   log_.info ("randomize2() begin ");

   min_uart_index = teal::dictionary_find_integer ({name_, "_min_uart_index"}, 0);
   max_uart_index = teal::dictionary_find_integer ({name_, "_max_uart_index"}, (number_of_uarts - 1));
   `truss_assert (randomize ());
   
   msg = $psprintf ("Selected Test component index of %0d", uart_index);
   log_.info (msg);


   //now for the test components...
   begin
      string id;
      id = $psprintf ("%0d", uart_index);

      uart_test_component_ingress_ = new  ({"uart_test_component_ingress ", id}, 
					   testbench_.a_uart_group[uart_index].uart_ingress_generator,  
					   testbench_.a_uart_group[uart_index].uart_program_sfm, 
					   testbench_.a_uart_group[uart_index].uart_ingress_checker);
      standard_generator (testbench_.a_uart_group[uart_index].uart_ingress_generator.name );
      uart_test_component_ingress_.randomize2 ();


      uart_test_component_egress_ = new  ({"uart_test_component_egress ", id}, 
					  testbench_.a_uart_group[uart_index].uart_egress_generator, 
					  testbench_.a_uart_group[uart_index].uart_protocol_bfm, 
					  testbench_.a_uart_group[uart_index].uart_egress_checker);
      standard_generator (testbench_.a_uart_group[uart_index].uart_egress_generator.name );
      uart_test_component_egress_.randomize2 ();

      standard_configuration (testbench_.a_uart_group[uart_index].uart_configuration.name);
   end

   //now for the test irritators...
   for (teal::uint32 i = 0; i < number_of_uarts; ++i) begin
      string id;
      id = $psprintf ("%0d", i);

      if (i != uart_index) begin
	 uart_basic_irritator bi = new  ({"uart_test_component_ingress ", id}, 
					 testbench_.a_uart_group[i].uart_ingress_generator,  
					 testbench_.a_uart_group[i].uart_program_sfm, 
					 testbench_.a_uart_group[i].uart_ingress_checker);
	 standard_generator (testbench_.a_uart_group[i].uart_ingress_generator.name );
	 bi.randomize2 ();
	 uart_basic_irritator_ingress_[ingress_count++] = bi;
	 


	 bi = new  ({"uart_test_component_egress ", id}, 
		    testbench_.a_uart_group[i].uart_egress_generator, 
		    testbench_.a_uart_group[i].uart_protocol_bfm, 
		    testbench_.a_uart_group[i].uart_egress_checker);
	 standard_generator (testbench_.a_uart_group[i].uart_egress_generator.name );
	 standard_configuration (testbench_.a_uart_group[i].uart_configuration.name);
	 bi.randomize2 ();
	 uart_basic_irritator_egress_[egress_count++] = bi;
      end
   end
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
`define for_each(data, method)\
for (integer i = 0; i < data.size (); i++) begin \
   data[i].method ();\
     end


`define for_each_1(data, method, param)\
for (integer i = 0; i < data.size (); i++) begin \
   data[i].method (param);\
     end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task quad_uart_irritators::time_zero_setup () ;
   uart_test_component_egress_.time_zero_setup ();
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_egress_ [i].time_zero_setup ();
   end
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task quad_uart_irritators::out_of_reset (truss::reset r) ;
   uart_test_component_egress_.out_of_reset (r);
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_egress_[i].out_of_reset (r);
   end
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task quad_uart_irritators::write_to_hardware () ;
   uart_test_component_egress_.write_to_hardware ();
   uart_test_component_ingress_.write_to_hardware ();
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_egress_[i].write_to_hardware ();
      uart_basic_irritator_ingress_[i].write_to_hardware ();
   end
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task quad_uart_irritators::start () ;
   uart_test_component_ingress_.start ();
   uart_test_component_egress_.start ();
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_ingress_[i].start ();
      uart_basic_irritator_egress_[i].start ();
   end
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task quad_uart_irritators::wait_for_completion () ;
   uart_test_component_ingress_.wait_for_completion (); 
   uart_test_component_egress_.wait_for_completion ();
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_ingress_[i].wait_for_completion (); 
      uart_basic_irritator_egress_[i].wait_for_completion ();
   end
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void quad_uart_irritators::report (string prefix) ;
   uart_test_component_ingress_.report (prefix); 
   uart_test_component_egress_.report (prefix);
   for (teal::uint32 i= 0; i < (number_of_uarts - 1); ++i) begin
      uart_basic_irritator_ingress_[i].report (prefix); 
      uart_basic_irritator_egress_[i].report (prefix);
   end
endfunction

