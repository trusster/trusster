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

`timescale 1 ns / 1 ns

`include "teal.svh"


interface memory_1 (
`ifdef VCS
   input 
 `else
`ifdef ncsim
input
`else
   output 
`endif
`endif
    reg        [122:0]  bank0  [0 : 312]
		);
endinterface

interface memory_2 (
`ifdef VCS
  input
`else		    
`ifdef ncsim
input
`else
   output 
`endif
`endif 
    reg [17:0] a_top_bank  [0 : 1717]
);
endinterface

`ifdef ncsim
package local_muck;
`endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_1 extends teal::memory_bank;
  virtual memory_1 memory_bank_1_;
   function new (virtual memory_1 mb);      super.new ("top.a_sub_module.bank0");   memory_bank_1_ = mb; endfunction 
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
`ifdef VCS
      value = top.a_sub_module.bank0[address];
`else
      value = memory_bank_1_.bank0[address];
`endif      
      log_.info ($psprintf ("Read[%0d] is %0d", address, value));
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
`ifdef VCS
      top.a_sub_module.bank0[address] <= value;
`else      
      memory_bank_1_.bank0[address] = value;
`endif
   log_.info ($psprintf ("Write[%0d] is %0d", address, value));
   endtask
endclass

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_2 extends teal::memory_bank;
  virtual memory_2 memory_bank_2_;
   function new (virtual memory_2 mb);      super.new ("top.a_top_bank");   memory_bank_2_ = mb; endfunction 
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
`ifdef VCS
      value = top.a_sub_module.bank0[address];
`else
      value = memory_bank_2_.a_top_bank[address];
`endif
      log_.info ($psprintf ("Read[%0d] is %0d", address, value));
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
`ifdef VCS
      top.a_sub_module.bank0[address] <= value;
`else
      memory_bank_2_.a_top_bank[address] = value;
`endif
      log_.info ($psprintf ("Write[%0d] is %0d", address, value));
   endtask
endclass


`ifdef ncsim
endpackage

import local_muck::*;
`endif

module interfaces_dut;
	memory_1 mb1_interface (top.a_sub_module.bank0);
	memory_2 mb2_interface (top.a_top_bank);
endmodule

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
program verification_top;

initial begin:initial_block
      teal::file_vlog not_used;
      teal::vout log;
      not_used = new (teal::dictionary_find ("out_file"), 
				      teal::dictionary_find_integer ("interactive", 1));
	log = new ("memory_test");


   @ (posedge (top.init_done));
   
   teal::vrandom_init_with_seed (teal::dictionary_find_integer ("seed", 0));

   //first, put them in the list, (do this in testbench top)
   begin
      local_muck::memory_bank_1 mb1;
      local_muck::memory_bank_2 mb2 ;
      mb1 = new (interfaces_dut.mb1_interface);
      mb2 = new (interfaces_dut.mb2_interface);
      teal::add_memory_bank (mb1);
      teal::add_memory_bank (mb2);
   end


   teal::add_map ("a_sub_module",   'h48000, 'h48000 + 312);  //partial path
   teal::add_map ("top.a_top_bank", 'h44000, 'h44000 + 1717); //full path test

   begin
      reg[122:0] sub_data;
      reg [17:0] top_data;
      string msg;
      int x;

      teal::read ('h44000 + 44, top_data);
      msg = $psprintf ("Initial value of top_level[44] is 0x%x (%0d)", top_data, top_data);
      if (top_data == 44) log.info (msg); else log.error (msg);

      teal::read ('h48000 + 175, sub_data);
      msg = $psprintf ("Initial value of sub_level[175] is 0x%x (%0d)", sub_data, sub_data);
      if (sub_data == 175) log.info (msg); else log.error (msg);

      teal::write ('h44000 + 44, 'hF2345); //test bit clipping
      teal::write ('h48000 + 175, 'h8988);   //test bit expansion
      #1;
      
      teal::read ('h44000 + 44, top_data);
      msg = $psprintf ("After setting top_level[44], value is 0x%x (%0d)", top_data, top_data);
      if (top_data == 'h32345) log.info (msg); else log.error (msg);

      teal::read ('h48000 + 175, sub_data);
      msg = $psprintf ("After setting sub_level[175], value is 0x%x (%0d)", sub_data, sub_data);
      if (sub_data == 'h8988) log.info (msg); else log.error (msg);
`ifdef kjjjjlj
      begin
      //now the direct way...
	 teal::memory_bank top_level;
	 teal::memory_bank sub_level;
	 
	top_level = teal::memory_lookup2 ("top.a_top_bank");  
	sub_level = teal::memory_lookup1 ('h48000);

	 top_level.from_memory (1717, top_data, 8);
	 msg = $psprintf ("Initial value of top_level[1717] is 0x%x (%0d)", top_data, top_data);
	 if (top_data == 1717) log.info (msg); else log.error (msg);

	 sub_level.from_memory (312, sub_data, 8);
	 msg = $psprintf ("Initial value of sub_level[312] is 0x%x (%0d)", sub_data, sub_data);
	 if (sub_data == 312) log.info (msg); else log.error (msg);

	 top_level.to_memory (1717, 'h3, 8);
	 sub_level.to_memory (312, 'h3232888, 8);
	 #1;

	 top_level.from_memory (1717, top_data, 8);
	 msg = $psprintf ("After setting top_level[1717], value is 0x%x (%0d)", top_data, top_data);
	 if (top_data == 'h3) log.info (msg); else log.error (msg);

	 sub_level.from_memory (312, sub_data, 8);
	 msg = $psprintf ("After setting sub_level[312] is 0x%x (%0d)", sub_data, sub_data);
	 if (sub_data == 'h3232888) log.info (msg); else log.error (msg);
      end
`endif      
   end


   

      begin
	  teal::vlog v;
	  v = teal::vlog_get ();
	 if (v.how_many (teal::vout_error)) begin
	    log.info ($psprintf ("Test Failed: Contained %0d error(s).",  v.how_many (teal::vout_error)));
	 end
	 else begin
	    log.info ("Test Passed. ");
	 end
      end
end // initial begin
   
   endprogram
