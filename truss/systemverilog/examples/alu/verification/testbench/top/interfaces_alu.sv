`include "interfaces_alu.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
module real_interfaces;
   alu_input alu_input_1 (top.operand_a, top.operand_b, top.operand, top.op_valid, top.operation_done);
   alu_output alu_output_1 (top.operation_done, top.result);
   top_reset top_reset_1 (top.clock, top.resetr);

   watchdog_interface watchdog_interface_0 (top.timeout, top.watchdog_1.counter);
endmodule // interfaces_alu

