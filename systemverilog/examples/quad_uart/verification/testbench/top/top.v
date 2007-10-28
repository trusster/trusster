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
   wire        wb_ack;
   pulldown pd_ack (wb_ack);
   
   wire 	       wb_clock;
   reg 	       wb_clockr;
   reg 	       wb_rst;
   wire [31:0] wb_adr;   
   wire [31:0] wb_dat_to_uart;
   wire [31:0] wb_dat_from_uart;
   wire [3:0]  wb_sel;

   //
   // UART
   // 
   wire uart_tx_0, uart_tx_1, uart_tx_2, uart_tx_3;
   wire uart_rx_0, uart_rx_1, uart_rx_2, uart_rx_3;
   wire uart_rts_0, uart_rts_1, uart_rts_2, uart_rts_3;
   wire  uart_cts_0, uart_cts_1, uart_cts_2, uart_cts_3;
   wire uart_dtr_0, uart_dtr_1, uart_dtr_2, uart_dtr_3;
   wire  uart_dsr_0, uart_dsr_1, uart_dsr_2, uart_dsr_3;
   tri1 uart_ri_0, uart_ri_1, uart_ri_2, uart_ri_3;
   tri1 uart_dcd_0, uart_dcd_1, uart_dcd_2, uart_dcd_3;
   wire uart_int_0, uart_int_1, uart_int_2, uart_int_3;

   //
   // Initials
   //
   pullup pu_dtr_0 (uart_dtr_0);
   pullup pu_dsr_0 (uart_dsr_0);
   pullup pu_cts_0 (uart_cts_0);
   pulldown pu_rts_0 (uart_rts_0);

   pullup pu_dtr_1 (uart_dtr_1);
   pullup pu_dsr_1 (uart_dsr_1);
   pullup pu_cts_1 (uart_cts_1);
   pulldown pu_rts_1 (uart_rts_1);

   pullup pu_dtr_2 (uart_dtr_2);
   pullup pu_dsr_2 (uart_dsr_2);
   pullup pu_cts_2 (uart_cts_2);
   pulldown pu_rts_2 (uart_rts_2);

   pullup pu_dtr_3 (uart_dtr_3);
   pullup pu_dsr_3 (uart_dsr_3);
   pullup pu_cts_3 (uart_cts_3);
   pulldown pu_rts_3 (uart_rts_3);


      reg 	wb_rst_ir;
   assign   wb_rst_i = wb_rst_ir;   
   
   //
   // CLOCK
   //
   initial  wb_clockr = 0;
   always #16.955 wb_clockr = ~wb_clockr;
      assign   wb_clock = wb_clockr;
   
   quad_uart_top quad_uart_top (

				//Wishbone
				.wb_clk_i (wb_clockr),
				.wb_rst_i (wb_rst_i), 
				.wb_adr_i (wb_adr), 
				.wb_dat_i (wb_dat_to_uart), 
				.wb_dat_o (wb_dat_from_uart), 
				.wb_we_i  (wb_we), 
				.wb_stb_i (wb_stb), 
				.wb_cyc_i (wb_cyc), 
				.wb_ack_o (wb_ack),  
				.wb_sel_i (wb_sel),

				// QUAD UART
				.uart_tx  ({uart_tx_0, uart_tx_1, uart_tx_2, uart_tx_3}),
				.uart_rx  ({uart_rx_0, uart_rx_1, uart_rx_2, uart_rx_3}),
				.uart_rts ({uart_rts_0, uart_rts_1, uart_rts_2, uart_rts_3}),
				.uart_cts ({uart_cts_0, uart_cts_1, uart_cts_2, uart_cts_3}),
				.uart_dtr ({uart_dtr_0, uart_dtr_1, uart_dtr_2, uart_dtr_3}),
				.uart_dsr ({uart_dsr_0, uart_dsr_1, uart_dsr_2, uart_dsr_3}),
				.uart_ri  ({uart_ri_0, uart_ri_1, uart_ri_2, uart_ri_3}),
				.uart_dcd ({uart_dcd_0, uart_dcd_1, uart_dcd_2, uart_dcd_3}),
				.uart_int ({uart_int_0, uart_int_1, uart_int_2, uart_int_3})
				);

   wishbone_driver_verilog wishbone_driver_verilog (
				    .adr  (wb_adr),
				    .dout (wb_dat_to_uart),
				    .cyc  (wb_cyc),
				    .stb  (wb_stb),
				    .sel  (wb_sel),
				    .we   (wb_we),
				    .clk  (wb_clockr),
				    .rst  (wb_rst),
				    .din  (wb_dat_from_uart),
				    .ack  (wb_ack),
				    .err  (1'b0),
				    .rty  (1'b0)
				    );







   initial BAUD_RATE_CLOCK = 0;
   always #16.955 BAUD_RATE_CLOCK = ~BAUD_RATE_CLOCK;

   
   //Standard startup code
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
   watchdog_implementation watchdog_1 (.timeout (timeout));

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

   always @(uart_tx_1) begin
      $display ("%t %m uart_tx_1 changed to: %d", $time, uart_tx_1);
   end

   always @(uart_rx_1) begin
      $display ("%t %m uart_rx_1 changed to: %d", $time, uart_rx_1);
   end
   
   always @(uart_int_1) begin
      $display ("%t %m uart_int_1 changed to: %d", $time, uart_int_1);
   end

   always @(uart_tx_2) begin
      $display ("%t %m uart_tx_2 changed to: %d", $time, uart_tx_2);
   end

   always @(uart_rx_2) begin
      $display ("%t %m uart_rx_2 changed to: %d", $time, uart_rx_2);
   end
   
   always @(uart_int_2) begin
      $display ("%t %m uart_int_2 changed to: %d", $time, uart_int_2);
   end

   always @(uart_tx_3) begin
      $display ("%t %m uart_tx_3 changed to: %d", $time, uart_tx_3);
   end

   always @(uart_rx_3) begin
      $display ("%t %m uart_rx_3 changed to: %d", $time, uart_rx_3);
   end
   
   always @(uart_int_3) begin
      $display ("%t %m uart_int_3 changed to: %d", $time, uart_int_3);
   end
`endif      




//`define mfm_debug
`ifdef mfm_debug
   initial $dumpvars;
`endif   

endmodule 
