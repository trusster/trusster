`timescale 1ns/1ns
//debug
//`define rtl_debug

module alu_top (
		operand_a,
		operand_b,
		operator,
		op_valid,
		operation_done,
		result
		);
   input [31:0] operand_a;
   input [31:0] operand_b;
   input [7:0] 	operator;
   input 	op_valid;
   output 	operation_done;
   output [31:0] result;
   reg [31:0] 	 result;
   
   
   
   reg 		 operation_done_r;
   initial operation_done_r = 0;
   assign 	 operation_done = operation_done_r;

   always @(posedge (op_valid)) begin
`ifdef rtl_debug
      $display ("%t %m Operator %d, operand a 0x%x operand b 0x%x", $time, operator, operand_a, operand_b);
`endif

      case (operator)  // number of bits in a word
	8'h00 : result  <= operand_a + operand_b;
	8'h01 : result  <= operand_a - operand_b;
	8'h02 : result  <= operand_a & operand_b;
	8'h03 : result  <= operand_a | operand_b;
	default: result <= 32'hx;
      endcase
      #10;
      operation_done_r <= 1;
      //      $display ("%t %m mark operation done.", $time);
      #1;
      operation_done_r <= 0;
      //      $display ("%t %m mark operation not done.", $time);      
   end


`ifdef rtl_debug
   always @(op_valid) begin
      $display ("%t %m op valid changed to %d", $time, op_valid);
   end

   always @(operation_done) begin
      $display ("%t %m op done changed to %d", $time, operation_done);
      #10;
      operation_done_r = 0;
      $display ("%t %m mark operation not done.", $time);      
   end
`endif
   
endmodule // alu_top

