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

   reg DTR;
   reg DSR;
   wire TX;
   wire RX;
   reg 	BAUD_RATE_CLOCK;
//   wire BAUD_RATE_CLOCK;
   reg 	CTS;
   reg 	RTS;


   //Hack because icarus optimizes unconnected nets away
   initial DTR = 0;
   initial DSR = 0;
   //   initial TX = 0;
   //   initial RX = 0;
   initial CTS = 1;
   initial RTS = 0;

   pullup (TX);
   pullup (RX);


   reg 	wb_clockr;
   reg 	wb_rst_ir;
// GERRY HELP  wire [`UART_ADDR_WIDTH-1:0] wb_adr_i;
   wire [31:0] wb_adr_i;   
   wire [31:0] 		       wb_dat_i;
   wire [31:0] 		       wb_dat_o;
   wire [3:0] 		       wb_sel_i;

   reg 	    RX_r; //necessary?
   

   integer  e;


   wire wb_clock;
   wire wb_rst_i;
   wire dsr_i;
   wire cts_i;
   wire ri_i;
   wire dcd_i;
   assign   wb_clock = wb_clockr;
   assign   wb_rst_i = wb_rst_ir;
//   assign   RX = RX_r;
   assign   cts_i = 1; //cts_ir;
   assign   dsr_i = 1; //dsr_ir;
   assign   ri_i = 1; //ri_ir;
   assign   dcd_i = 1; //dcd_ir;
   
   uart_top  uart_0 (
		     wb_clock, 
   
		     // Wishbone signals
		     wb_rst_i, wb_adr_i[`UART_ADDR_WIDTH-1:0], wb_dat_i, wb_dat_o, wb_we_i, wb_stb_i, wb_cyc_i, wb_ack_o,  wb_sel_i,
		     int_o, // interrupt request

		     // UART signals
		     // serial input/output
		     TX, RX,

		     // modem signals
		     rts_o, cts_i, dtr_o, dsr_i, ri_i, dcd_i
		     `ifdef UART_HAS_BAUDRATE_OUTPUT
		     , baud1_o
		     `endif

		     );

   wishbone_driver wishbone_driver (// Outputs
               .adr                 (wb_adr_i),
               .dout                (wb_dat_i),
               .cyc                 (wb_cyc_i),
               .stb                 (wb_stb_i),
               .sel                 (wb_sel_i),
               .we                (wb_we_i),
               // Inputs
               .clk                 (wb_clock),
               .rst                 (wb_rst_i),
               .din                 (wb_dat_o),
               .ack                 (wb_ack_o),
               .err                 (1'b0),
               .rty                 (1'b0));



   initial  wb_clockr = 0;
   
   always
     begin
//	#5 wb_clockr = ~wb_clock;
	#16.955 wb_clockr = ~wb_clock;
     end





   //Standard startup muck
   initial BAUD_RATE_CLOCK = 0;
   always #16.955 BAUD_RATE_CLOCK = ~ BAUD_RATE_CLOCK;
//   assign BAUD_RATE_CLOCK = wb_clockr;
   

   initial 
     begin
//	#1;
	RTS = 1;
//	$display ("%t Starting Truss", $time);
//	$verification_top;
     end


   reg[7:0] init_done;
   initial begin
      init_done = 0; 
      init_done = #43 8'hFF;
   end


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

   always @(TX) begin
      $display ("%t %m TX changed to: %d", $time, TX);
   end

   always @(RX) begin
      $display ("%t %m RX changed to: %d", $time, RX);
   end
   
   always @(int_o) begin
      $display ("%t %m int_o changed to: %d", $time, int_o);
   end
endmodule 


