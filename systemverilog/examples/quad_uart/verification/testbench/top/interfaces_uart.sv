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


interface uart_interface (
    wire dtr,
    wire     dsr,
    wire     rx,
    wire     tx,
    wire     cts,
    wire     rts,			  
    input     baud_rate_clock
);
endinterface // uart_interface


interface wishbone_driver_interface (
    wire     clock_,     
    wire [31:0] address_,
    wire [31:0] data_in_,
    input [31:0] data_out_,
    wire [3:0] 	select_,
    wire [1:0] 	op_code_,
    inout 	do_work_,
    input	work_done_
				     );
endinterface 

interface uart_16550_interface (
    wire 	interrupt_request_,
    wire 	baud_rate_clock_
			      );
   
  endinterface



module interfaces_uart;
   wishbone_driver_interface wishbone_driver_interface_1 ( .clock_ (top.wishbone_driver_verilog.clk),
							  .address_ (top.wishbone_driver_verilog.address),
							  .data_in_ (top.wishbone_driver_verilog.data_in),
							  .data_out_ (top.wishbone_driver_verilog.data_outr),
							  .select_ (top.wishbone_driver_verilog.select),
							  .op_code_ (top.wishbone_driver_verilog.op_code),
							  .do_work_ (top.wishbone_driver_verilog.do_work),
							  .work_done_ (top.wishbone_driver_verilog.work_doner));

   

   uart_16550_interface uart_16550_interface_0 (top.uart_int_0, top.wb_clock);
   uart_interface uart_interface_0 (top.uart_dsr_0, top.uart_dtr_0, top.uart_tx_0, top.uart_rx_0, top.uart_rts_0, top.uart_cts_0, top.BAUD_RATE_CLOCK);

   uart_16550_interface uart_16550_interface_1 (top.uart_int_1, top.wb_clock);
   uart_interface uart_interface_1 (top.uart_dsr_1, top.uart_dtr_1, top.uart_tx_1, top.uart_rx_1, top.uart_rts_1, top.uart_cts_1, top.BAUD_RATE_CLOCK);

   uart_16550_interface uart_16550_interface_2 (top.uart_int_2, top.wb_clock);
   uart_interface uart_interface_2 (top.uart_dsr_2, top.uart_dtr_2, top.uart_tx_2, top.uart_rx_2, top.uart_rts_2, top.uart_cts_2, top.BAUD_RATE_CLOCK);

   uart_16550_interface uart_16550_interface_3 (top.uart_int_3, top.wb_clock);
   uart_interface uart_interface_3 (top.uart_dsr_3, top.uart_dtr_3, top.uart_tx_3, top.uart_rx_3, top.uart_rts_3, top.uart_cts_3, top.BAUD_RATE_CLOCK);
endmodule // interfaces_uart
