

function truss::interfaces_dut build_interfaces ();
  interfaces_uart uart;
`ifdef ATHDL_SIM
virtual uart_interface tmp_uart_intf[4];
virtual uart_16550_interface tmp_uart_16550_intf[4];
                                                                                
tmp_uart_intf[0] = real_interfaces.uart_interface_0;
tmp_uart_intf[1] = real_interfaces.uart_interface_1;
tmp_uart_intf[2] = real_interfaces.uart_interface_2;
tmp_uart_intf[3] = real_interfaces.uart_interface_3;
                                                                                
tmp_uart_16550_intf[0] = real_interfaces.uart_16550_interface_0;
tmp_uart_16550_intf[1] = real_interfaces.uart_16550_interface_1;
tmp_uart_16550_intf[2] = real_interfaces.uart_16550_interface_2;
tmp_uart_16550_intf[3] = real_interfaces.uart_16550_interface_3;
                                                                                
uart = new(tmp_uart_intf, real_interfaces.wishbone_driver_interface_1, tmp_uart_16550_intf, real_interfaces.top_reset_1);
                                                                                
`else
  uart = new ({real_interfaces.uart_interface_0, real_interfaces.uart_interface_1, 
	       real_interfaces.uart_interface_2, real_interfaces.uart_interface_3},
	      real_interfaces.wishbone_driver_interface_1, 
	      {real_interfaces.uart_16550_interface_0, real_interfaces.uart_16550_interface_1,
	       real_interfaces.uart_16550_interface_2, real_interfaces.uart_16550_interface_3}, 
	      real_interfaces.top_reset_1);
`endif
  return uart;
endfunction
