/*
 * Copyright (c) 2004 Mike Mintz (mike@applevalleyverification.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

`include "timescale.v"
`include "uart_defines.v"

module top;

   reg 	       BAUD_RATE_CLOCK;

   //
   // Wishbone
   //
   tri1        wb_ack;
   reg 	       wb_clockr;
   wire	       wb_clock;
   reg 	       wb_rst;
   wire [31:0] wb_adr;   
   wire [31:0] wb_dat_to_uart;
   wire [31:0] wb_dat_from_uart;
   wire [3:0]  wb_sel;

   //
   // UART
   // 
   wire uart_tx_0;
   wire uart_rx_0;
   wire uart_rts_0;
   reg  uart_cts_0;
   wire uart_dtr_0;
   reg  uart_dsr_0;
   tri1 uart_ri_0;
   tri1 uart_dcd_0;
   wire uart_int_0;

   //
   // Initials
   //
   initial begin
      uart_cts_0 = 1;
      uart_dsr_0 = 0;
   end
   
   
   assign   wb_clock = wb_clockr;   
   
   uart_top  uart_0 (
		     //Wishbone
		     .wb_clk_i (wb_clock),
		     .wb_rst_i (wb_rst), 
		     .wb_adr_i ({2'b0, wb_adr[2:0]}), 
		     .wb_dat_i (wb_dat_to_uart), 
		     .wb_dat_o (wb_dat_from_uart), 
		     .wb_we_i  (wb_we), 
		     .wb_stb_i (wb_stb), 
		     .wb_cyc_i (wb_cyc), 
		     .wb_ack_o (wb_ack),  
		     .wb_sel_i (wb_sel),


		     .int_o(uart_int_0), 
		     
		     // Uart signals
		     .stx_pad_o(uart_tx_0), 
		     .srx_pad_i(uart_rx_0), 
		     .rts_pad_o(uart_rts_0), 
		     .cts_pad_i(uart_cts_0), 
		     .dtr_pad_o(uart_dtr_0), 
		     .dsr_pad_i(uart_dsr_0), 
		     .ri_pad_i(uart_ri_0), 
		     .dcd_pad_i(uart_dcd_0)
		     );

   wishbone_driver wishbone_driver (
				    .adr  (wb_adr),
				    .dout (wb_dat_to_uart),
				    .cyc  (wb_cyc),
				    .stb  (wb_stb),
				    .sel  (wb_sel),
				    .we   (wb_we),
				    .clk  (wb_clock),
				    .rst  (wb_rst),
				    .din  (wb_dat_from_uart),
				    .ack  (wb_ack),
				    .err  (1'b0),
				    .rty  (1'b0)
				    );






   initial  wb_clockr = 0;
   
   always
     begin
	#16.955 wb_clockr = ~wb_clock;
     end


   initial BAUD_RATE_CLOCK = 0;
   always #16.955 BAUD_RATE_CLOCK = ~BAUD_RATE_CLOCK;

   

   reg test_done;
   reg test_done_ack;
   initial begin
      test_done = 0; 
      test_done_ack = 0;
      @ (posedge (test_done));
      #1 //give trans time to print
	$display ("%0t ns VERILOG: Received exit from Teal. Exiting simulation.", $time);
      $finish;
      test_done_ack = 1;
   end

   wire timeout;
   watchdog watchdog (.timeout (timeout));

//`define mfm_debug
`ifdef mfm_debug
   always @(uart_tx_0) begin
      $display ("%t %m uart_tx_0 changed to: %d", $time, uart_tx_0);
   end

   always @(uart_rx_0) begin
      $display ("%t %m uart_rx_0 changed to: %d", $time, uart_rx_0);
   end
   
   always @(uart_int_0) begin
      $display ("%t %m uart_int_0 changed to: %d", $time, uart_int_0);
   end
`endif      




//`define mfm_debug
`ifdef mfm_debug
   initial $dumpvars;
`endif   

endmodule 


