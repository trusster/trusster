

function truss::interfaces_dut build_interfaces ();
  interfaces_uart uart;
  uart = new (real_interfaces.uart_interface_1, real_interfaces.wishbone_driver_interface_1, 
	      real_interfaces.uart_16550_interface_1, real_interfaces.top_reset_1);
  return uart;
endfunction
