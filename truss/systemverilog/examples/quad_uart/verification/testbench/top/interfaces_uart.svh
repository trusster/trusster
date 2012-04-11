/*
Trusster Open Source License version 1.0a (TRUST)
copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
   
   Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
   Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
   Redistributions in any form must be accompanied by information on how to obtain 
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

`ifndef __interfaces_uart__
 `define __interfaces_uart__

interface uart_interface (
    output reg dtr,
    output reg     dsr,
    input     rx,
    output reg     tx,
    output reg     cts,
    output reg     rts,			  
    input     baud_rate_clock
);
endinterface // uart_interface


interface wishbone_driver_interface (
    input     clock_,     
    output reg [31:0] address_,
    output reg [31:0] data_in_,
    input [31:0] data_out_,
    output reg [3:0] 	select_,
    output reg [1:0] 	op_code_,
    output reg 	do_work_,
    input	work_done_
				     );
endinterface 

interface uart_16550_interface (
    wire 	interrupt_request_,
    wire 	baud_rate_clock_
			      );
   
  endinterface


interface top_reset (
    output reg 	       wb_rst_ir
		     );
  endinterface // top_reset

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Note that the watchdog interface does not go in here
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class interfaces_uart extends truss::interfaces_dut;
   virtual uart_interface uart_interface_[4];
   virtual wishbone_driver_interface wishbone_driver_interface_1;
   virtual uart_16550_interface uart_16550_interface_[4];
   virtual top_reset top_reset_;

   function new (virtual uart_interface input_[4], virtual wishbone_driver_interface output_1,
		 virtual uart_16550_interface u16550_[4],	 virtual top_reset tr);
      uart_interface_ = input_;
      wishbone_driver_interface_1 = output_1;
      uart_16550_interface_ = u16550_;
      top_reset_ = tr;
      endfunction
endclass

`endif //  `ifndef __interfaces_alu__
