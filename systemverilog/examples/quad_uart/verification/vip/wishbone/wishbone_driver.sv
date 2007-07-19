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

`include "wishbone_driver.svh"

import teal::*;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function wishbone_driver::new (string n, virtual wishbone_driver_interface p);
   name_ = n;
   wishbone_driver_interface_ = p;
   mutex_ = new (1);
   log_ = new (n);
//   log_.show_debug_level (teal::debug);
endfunction 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 task wishbone_driver::pause (teal::uint32 count);
    for (teal::uint32 i = count; i; --i) begin
      @ (posedge (wishbone_driver_interface_.clock_));
    end
 endtask 
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task wishbone_driver::to_memory (bit [63:0] address, input bit [MAX_DATA - 1:0]  value, teal::uint32 size);
   string msg;
   int foo;

   mutex_.get (1);
   #1234;

   msg = $psprintf ("to_memory: 0x%0x 0x%0x (size %d)", address, value, size);
   log_.debug (msg);
   
   wishbone_driver_interface_.op_code_ <= 0;
   wishbone_driver_interface_.address_ <= address;
   //unclear if the following code should be in the verilog or not, woudl have to send size down
   //and there would be some make length set by the verilog reg
   if (size <= 8) begin  //put the data on the right line
      case (address % 4) 
	0: begin wishbone_driver_interface_.select_ <= 1; wishbone_driver_interface_.data_in_ <= value; end
	1: begin wishbone_driver_interface_.select_ <= 2; wishbone_driver_interface_.data_in_ <= value << 8; end
	2: begin wishbone_driver_interface_.select_ <= 4; wishbone_driver_interface_.data_in_ <= value << 16; end
	3: begin wishbone_driver_interface_.select_ <= 8; wishbone_driver_interface_.data_in_ <= value << 24; end
      endcase // case(a % 4)
   end
   else begin
      `truss_assert (size == 64); //have to code the 16 case or maybe even multi write case
      wishbone_driver_interface_.select_ <= 'hF;
      wishbone_driver_interface_.data_in_ <= value;
   end
   wishbone_driver_interface_.do_work_ <= 1;           //signal to verilog
   //top.wishbone_driver_verilog.do_work <= 1;
   
   log_.debug ({msg, " now wait "});   
   @ (posedge (wishbone_driver_interface_.work_done_)); //wait for ack
   wishbone_driver_interface_.do_work_ <= 0;           //signal to verilog
   mutex_.put (1);
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task wishbone_driver::from_memory (bit [63:0] address, inout bit [MAX_DATA - 1:0] returned, input teal::uint32 size);
   mutex_.get (1);
   #1234;
   
   wishbone_driver_interface_.op_code_ <= 1;
   wishbone_driver_interface_.address_ <= address;
   if (size <= 8) begin  //put the data on the right line
      case (address % 4) 
	0: wishbone_driver_interface_.select_ <= 1; 
	1: wishbone_driver_interface_.select_ <= 2; 
	2: wishbone_driver_interface_.select_ <= 4; 
	3: wishbone_driver_interface_.select_ <= 8; 
      endcase
   end
   else begin
      wishbone_driver_interface_.select_ <= 'hF;
   end
   wishbone_driver_interface_.do_work_ <= 1;           //signal to verilog
   @ (posedge (wishbone_driver_interface_.work_done_)); //wait for ack
   wishbone_driver_interface_.do_work_ <= 0;           //signal to verilog   
   returned = wishbone_driver_interface_.data_out_;

   
   if (size == 8) begin  //put the data on the right line
      case (address % 4) 
	1: returned = returned >> 8; 
	2: returned = returned >> 16; 
	3: returned = returned >> 24; 
      endcase
   end

   begin
      string msg;
      int foo;
      msg = $psprintf ("from_memory (completed): 0x%0x 0x%0x (size %d)", address, returned, size);
      log_.debug (msg);
   end      
   mutex_.put (1);
endtask
