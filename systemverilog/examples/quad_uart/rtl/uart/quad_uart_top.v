


`include "timescale.v"
`include "uart_defines.v"

module quad_uart_top(
		     wb_clk_i, 
		     // Wishbone signals
		     wb_rst_i, 
		     wb_adr_i, 
		     wb_dat_i, 
		     wb_dat_o, 
		     wb_we_i, 
		     wb_stb_i, 
		     wb_cyc_i, 
		     wb_ack_o, 
		     wb_sel_i,

		     // UART signal
		     uart_tx, 
		     uart_rx, 
		     uart_rts, 
		     uart_cts, 
		     uart_dtr, 
		     uart_dsr, 
		     uart_ri, 
		     uart_dcd, 
		     uart_int
		     );


   parameter                    uart_data_width = `UART_DATA_WIDTH;

   // Wishbone
   input 		        wb_clk_i;
   input 		        wb_rst_i;
   input [31:0] 	        wb_adr_i;
   input [uart_data_width-1:0]  wb_dat_i;
   output [uart_data_width-1:0] wb_dat_o;
   input 			wb_we_i;
   input 			wb_stb_i;
   input 			wb_cyc_i;
   output 			wb_ack_o;
   input [3:0] 			wb_sel_i;

   // UART
   output [0:3]			uart_tx;
   input  [0:3]			uart_rx;
   output [0:3]			uart_rts;
   input  [0:3]			uart_cts;
   output [0:3]			uart_dtr;
   input  [0:3]			uart_dsr;
   input  [0:3]			uart_ri;
   input  [0:3]			uart_dcd;
   output [0:3]			uart_int;

   
   wire [31:0] 			wb_dat_0;
   wire 			wb_ack_0;
   
   wire [31:0] 			wb_dat_1;
   wire 			wb_ack_1;
   
   wire [31:0] 			wb_dat_2;
   wire 			wb_ack_2;
   
   wire [31:0] 			wb_dat_3;
   wire 			wb_ack_3;
   
   
   wire [4:0] 			wb_address;


   //
   // DECODE FOR UART 0
   //
   
   assign wb_address = {3'h0, wb_adr_i[1:0]};
   assign wb_we_0 = (wb_adr_i[31:3] === 0) ?  wb_we_i : 0;
   assign wb_ack_o = (wb_adr_i[31:3] === 0) ?  wb_ack_0 : 32'hZ;   
   assign wb_dat_o = (wb_adr_i[31:3] === 0) ? wb_dat_0 : 32'hZ;
   assign wb_stb_0 = (wb_adr_i[31:3] === 0) ?  wb_stb_i : 0;
   //
   // UART 0
   //
   uart_top  uart_0 (
		     // Wishbone signals
		     .wb_clk_i (wb_clk_i), 
		     .wb_rst_i (wb_rst_i), 
		     .wb_adr_i (wb_address), 
		     .wb_dat_i (wb_dat_i), 
		     .wb_dat_o (wb_dat_0), 
		     .wb_we_i  (wb_we_0), 
		     .wb_stb_i (wb_stb_0), 
		     .wb_cyc_i (wb_cyc_i), 
		     .wb_ack_o (wb_ack_0),  
		     .wb_sel_i (wb_sel_i),
		     
		     .int_o(uart_int[0]), 
		     
		     // Uart signals
		     .stx_pad_o(uart_tx[0]), 
		     .srx_pad_i(uart_rx[0]), 
		     .rts_pad_o(uart_rts[0]), 
		     .cts_pad_i(uart_cts[0]), 
		     .dtr_pad_o(uart_dtr[0]), 
		     .dsr_pad_i(uart_dsr[0]), 
		     .ri_pad_i(uart_ri[0]), 
		     .dcd_pad_i(uart_dcd[0])
		     
		     `ifdef UART_HAS_BAUDRATE_OUTPUT
		     , .baud_o(baud1_o)
		     `endif
		     
		     );






   //
   // DECODE FOR UART 1
   //
   
   assign wb_we_1 = (wb_adr_i[31:3] === 1) ?  wb_we_i : 0;
   assign wb_ack_o = (wb_adr_i[31:3] === 1) ?  wb_ack_1 : 32'hZ;   
   assign wb_dat_o = (wb_adr_i[31:3] === 1) ? wb_dat_1 : 32'hZ;
   assign wb_stb_1 = (wb_adr_i[31:3] === 1) ?  wb_stb_i : 0;

   //
   // UART 1
   //
   uart_top  uart_1 (
		     // Wishbone signals
		     .wb_clk_i (wb_clk_i), 
		     .wb_rst_i (wb_rst_i), 
		     .wb_adr_i (wb_address), 
		     .wb_dat_i (wb_dat_i), 
		     .wb_dat_o (wb_dat_1), 
		     .wb_we_i  (wb_we_1), 
		     .wb_stb_i (wb_stb_1), 
		     .wb_cyc_i (wb_cyc_i), 
		     .wb_ack_o (wb_ack_1),  
		     .wb_sel_i (wb_sel_i),
		     
		     .int_o(uart_int[1]), 
		     
		     // Uart signals
		     .stx_pad_o(uart_tx[1]), 
		     .srx_pad_i(uart_rx[1]), 
		     .rts_pad_o(uart_rts[1]), 
		     .cts_pad_i(uart_cts[1]), 
		     .dtr_pad_o(uart_dtr[1]), 
		     .dsr_pad_i(uart_dsr[1]), 
		     .ri_pad_i(uart_ri[1]), 
		     .dcd_pad_i(uart_dcd[1])
		     
		     `ifdef UART_HAS_BAUDRATE_OUTPUT
		     , .baud_o(baud1_o)
		     `endif
		     
		     );






   //
   // DECODE FOR UART 2
   //
   
   assign wb_address = {3'h0, wb_adr_i[1:0]};
   assign wb_we_2 = (wb_adr_i[31:3] === 2) ?  wb_we_i : 0;
   assign wb_ack_o = (wb_adr_i[31:3] === 2) ?  wb_ack_2 : 32'hZ;   
   assign wb_dat_o = (wb_adr_i[31:3] === 2) ? wb_dat_2 : 32'hZ;
   assign wb_stb_2 = (wb_adr_i[31:3] === 2) ?  wb_stb_i : 0;
   //
   // UART 2
   //
   uart_top  uart_2 (
		     // Wishbone signals
		     .wb_clk_i (wb_clk_i), 
		     .wb_rst_i (wb_rst_i), 
		     .wb_adr_i (wb_address), 
		     .wb_dat_i (wb_dat_i), 
		     .wb_dat_o (wb_dat_2), 
		     .wb_we_i  (wb_we_2), 
		     .wb_stb_i (wb_stb_2), 
		     .wb_cyc_i (wb_cyc_i), 
		     .wb_ack_o (wb_ack_2),  
		     .wb_sel_i (wb_sel_i),
		     
		     .int_o(uart_int[2]), 
		     
		     // Uart signals
		     .stx_pad_o(uart_tx[2]), 
		     .srx_pad_i(uart_rx[2]), 
		     .rts_pad_o(uart_rts[2]), 
		     .cts_pad_i(uart_cts[2]), 
		     .dtr_pad_o(uart_dtr[2]), 
		     .dsr_pad_i(uart_dsr[2]), 
		     .ri_pad_i(uart_ri[2]), 
		     .dcd_pad_i(uart_dcd[2])
		     
		     `ifdef UART_HAS_BAUDRATE_OUTPUT
		     , .baud_o(baud1_o)
		     `endif
		     
		     );






   //
   // DECODE FOR UART 3
   //
   
   assign wb_address = {3'h0, wb_adr_i[1:0]};
   assign wb_we_3 = (wb_adr_i[31:3] === 3) ?  wb_we_i : 0;
   assign wb_ack_o = (wb_adr_i[31:3] === 3) ?  wb_ack_3 : 32'hZ;   
   assign wb_dat_o = (wb_adr_i[31:3] === 3) ? wb_dat_3 : 32'hZ;
   assign wb_stb_3 = (wb_adr_i[31:3] === 3) ?  wb_stb_i : 0;
   //
   // UART 3
   //
   uart_top  uart_3 (
		     // Wishbone signals
		     .wb_clk_i (wb_clk_i), 
		     .wb_rst_i (wb_rst_i), 
		     .wb_adr_i (wb_address), 
		     .wb_dat_i (wb_dat_i), 
		     .wb_dat_o (wb_dat_3), 
		     .wb_we_i  (wb_we_3), 
		     .wb_stb_i (wb_stb_3), 
		     .wb_cyc_i (wb_cyc_i), 
		     .wb_ack_o (wb_ack_3),  
		     .wb_sel_i (wb_sel_i),
		     
		     .int_o(uart_int[3]), 
		     
		     // Uart signals
		     .stx_pad_o(uart_tx[3]), 
		     .srx_pad_i(uart_rx[3]), 
		     .rts_pad_o(uart_rts[3]), 
		     .cts_pad_i(uart_cts[3]), 
		     .dtr_pad_o(uart_dtr[3]), 
		     .dsr_pad_i(uart_dsr[3]), 
		     .ri_pad_i(uart_ri[3]), 
		     .dcd_pad_i(uart_dcd[3])
		     
		     `ifdef UART_HAS_BAUDRATE_OUTPUT
		     , .baud_o(baud1_o)
		     `endif
		     
		     );
   
   
endmodule // quad_uart_top
