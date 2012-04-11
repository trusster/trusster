
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
module real_interfaces;
   wishbone_driver_interface wishbone_driver_interface_1 ( .clock_ (top.wishbone_driver_verilog.clk),
							  .address_ (top.wishbone_driver_verilog.address),
							  .data_in_ (top.wishbone_driver_verilog.data_in),
							  .data_out_ (top.wishbone_driver_verilog.data_outr),
							  .select_ (top.wishbone_driver_verilog.select),
							  .op_code_ (top.wishbone_driver_verilog.op_code),
							  .do_work_ (top.wishbone_driver_verilog.do_work),
							  .work_done_ (top.wishbone_driver_verilog.work_doner));

   watchdog_interface watchdog_interface_0 (top.timeout, top.watchdog_1.counter);
   top_reset top_reset_1 (top.wb_rst_ir);
   

   uart_16550_interface uart_16550_interface_0 (top.uart_int_0, top.wb_clock);
   uart_16550_interface uart_16550_interface_1 (top.uart_int_1, top.wb_clock);
   uart_16550_interface uart_16550_interface_2 (top.uart_int_2, top.wb_clock);
   uart_16550_interface uart_16550_interface_3 (top.uart_int_3, top.wb_clock);

   uart_interface uart_interface_0 (top.uart_dsr_0, top.uart_dtr_0, top.uart_tx_0, top.uart_rx_0, top.uart_rts_0, top.uart_cts_0, top.BAUD_RATE_CLOCK);
   uart_interface uart_interface_1 (top.uart_dsr_1, top.uart_dtr_1, top.uart_tx_1, top.uart_rx_1, top.uart_rts_1, top.uart_cts_1, top.BAUD_RATE_CLOCK);
   uart_interface uart_interface_2 (top.uart_dsr_2, top.uart_dtr_2, top.uart_tx_2, top.uart_rx_2, top.uart_rts_2, top.uart_cts_2, top.BAUD_RATE_CLOCK);
   uart_interface uart_interface_3 (top.uart_dsr_3, top.uart_dtr_3, top.uart_tx_3, top.uart_rx_3, top.uart_rts_3, top.uart_cts_3, top.BAUD_RATE_CLOCK);

endmodule // interfaces_uart
