

function truss::interfaces_dut build_interfaces ();
  interfaces_alu alu;
//   alu_input alu_input_1 (top.operand_a, top.operand_b, top.operand, top.op_valid, top.operation_done);
//   alu_output alu_output_1 (top.operation_done, top.result);
//  alu = new (alu_input_1, alu_output_1);
  
  alu = new (real_interfaces.alu_input_1, real_interfaces.alu_output_1, real_interfaces.top_reset_1);
//  alu.alu_input_1 = real_interfaces.alu_input_1;
//  alu.alu_output_1 = real_interfaces.alu_output_1;
  return alu;
endfunction
