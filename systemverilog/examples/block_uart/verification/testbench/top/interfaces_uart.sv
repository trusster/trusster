
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

   

   uart_16550_interface uart_16550_interface_1 (top.int_o, top.wb_clock);

   uart_interface uart_interface_1 (top.DSR, top.DTR, top.TX, top.RX, top.RTS, top.CTS, top.BAUD_RATE_CLOCK);
   watchdog_interface watchdog_interface_0 (top.timeout, top.watchdog_1.counter);
   top_reset top_reset_1 (top.wb_rst_ir);
endmodule // interfaces_uart
