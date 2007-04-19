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

`include "truss.svh"

`include "uart_bfm_agent.svh"
`include "uart_16550_agent.svh"
`include "uart_generator_agent.svh"
`include "uart_checker_agent.svh"
`include "uart_16550_configuration.svh"

`include "wishbone_driver.svh"
`include "wishbone_memory_bank.svh"

`define uart_registers_first  0
`define uart_registers_last ((number_of_uarts - 1)<<4) + 7

import truss::*;



function testbench::new (string name);
   super.new (name);
   log_.debug ("testbench new() begin ");

   begin
      virtual uart_interface ui[number_of_uarts] = {interfaces_uart.uart_interface_0, interfaces_uart.uart_interface_1,
						    interfaces_uart.uart_interface_2, interfaces_uart.uart_interface_3};
      
      virtual uart_16550_interface ui_16550[number_of_uarts] = {interfaces_uart.uart_16550_interface_0, interfaces_uart.uart_16550_interface_1,
						    interfaces_uart.uart_16550_interface_2, interfaces_uart.uart_16550_interface_3};
      
      for (teal::uint32 i = 0; i < number_of_uarts; ++i) begin
	 uart_group[i] = new (name, i, ui[i], ui_16550[i]);
      end
   end


      //Now for the main chip register interface
      wishbone_driver_ = new ("WB", interfaces_uart.wishbone_driver_interface_1);
      begin
	 wishbone_memory_bank mb = new ("main_bus", wishbone_driver_);
	 teal::add_memory_bank (mb);
	 teal::add_map ("main_bus", `uart_registers_first, `uart_registers_last);
      end


      log_.debug ( "testbench new() done ");
   endfunction 


task testbench::time_zero_setup ();
   top.wb_rst_ir <= 1;
   #1;
endtask

task testbench::out_of_reset (truss::reset r);
   top.wb_rst_ir <= 1;
   wishbone_driver_.pause (10);
   top.wb_rst_ir <= 0;
endtask

task testbench::randomize2 ();  
  for (teal::uint32 i = 0; i < number_of_uarts; ++i) begin
        uart_group[i].uart_configuration.randomize2 ();
  end
endtask

task testbench::write_to_hardware (); endtask
task testbench::start (); log_.info ("testbench starting"); endtask


//no wait_for_completion, let the test/exercisors do that
task testbench::wait_for_completion (); endtask
task testbench::report (string prefix);  
  for (teal::uint32 i = 0; i < number_of_uarts; ++i) begin
        uart_group[i].uart_configuration.report (prefix);
  end
endtask


