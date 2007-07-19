
function truss::interfaces_dut build_interfaces ();
  interfaces_uart uart;
  uart = new ({real_interfaces.uart_interface_0, real_interfaces.uart_interface_1, 
	       real_interfaces.uart_interface_2, real_interfaces.uart_interface_3},
	      real_interfaces.wishbone_driver_interface_1, 
	      {real_interfaces.uart_16550_interface_0, real_interfaces.uart_16550_interface_1,
	       real_interfaces.uart_16550_interface_2, real_interfaces.uart_16550_interface_3}, 
	      real_interfaces.top_reset_1);

uart.uart_interface_[2] = real_interfaces.uart_interface_2;
uart.uart_interface_[3] = real_interfaces.uart_interface_3;
uart.uart_16550_interface_[2]= real_interfaces.uart_16550_interface_2;
uart.uart_16550_interface_[3]= real_interfaces.uart_16550_interface_3;

  return uart;
endfunction
