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
`ifndef __uart_1655__
 `define __uart_1655__

 `include "uart_16550_configuration.svh"

 `include "uart_bfm_agent.svh" //to get block;

 `include "truss.svh"

//WARNING: MUST ALWAYS BE data_terminal_equipment
//(1) This uart DUT looks to always be a data_terminal_equipment. (not clear from documentation)
//(2) Would have to add to this code and vip to swizzle lines in software and have assigns in testbench

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class uart_16550_sfm extends truss::verification_component;
   local uart_configuration configuration_;
   local virtual uart_16550_interface uart_16550_interface_;
      
   //valid after start
   local teal::uint64 one_bit_; 
   local teal::uint64 clock_frequency_;
   local int index_;

   extern function new (string name, int index, virtual uart_16550_interface ui,
			uart_configuration_16550 c, teal::uint64 clock_frequency);


   task time_zero_setup (); endtask
   task out_of_reset (truss::reset r); endtask
   virtual function void  randomize2 (); endfunction
   extern task start ();
   extern task write_to_hardware ();

   task wait_for_completion (); endtask
   function void report (string prefix); endfunction

   //These set the DUT wires accordingly, where true maps to active
   extern task dtr (bit new_value);
   extern task dsr (bit new_value);

   extern task cts (bit new_value);
   extern task rts (bit new_value);

   extern task send (uart_block current_tx);

   `PURE protected virtual task receive_completed_ (uart_block the_block);

   extern local task do_receive_completed_ (uart_word the_word);

   extern task pause_ (teal::uint32 count);
   extern task do_rx_thread_ ();

   extern local function teal::uint32 bus_address_ (teal::uint32 offset);
endclass
`endif
