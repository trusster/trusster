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

`timescale 1 ns / 1 ps

module top;

   reg [31:0] operand_a_reg;
   reg [31:0] operand_b_reg;
   reg [7:0]  operand_reg;
   reg 	       op_valid_reg;

   wire [31:0] operand_a;
   wire [31:0] operand_b;
   wire [7:0]  operand;
   wire        op_valid;

   wire       operation_done;
   wire [31:0] result;

//   assign operand_a = operand_a_reg;
//   assign operand_b = operand_b_reg;
 // assign operand = operand_reg;
//   initial operand_a_reg = 32'h0;
   
   
   alu_top alu_top (
		    .operand_a (operand_a),
		    .operand_b (operand_b),
		    .operator (operand),
		    .op_valid (op_valid),
		    .operation_done (operation_done),
		    .result (result)
		    );


   initial op_valid_reg = 0;
   wire 	clock;
   wire 	reset;
   reg 		clockr;
   reg 		resetr;

   assign      clock = clockr;
   assign      reset = resetr;

   initial  clockr = 0;
   always
     begin
	#5 clockr = ~clock;
     end
   
   watchdog_implementation watchdog_1 (.timeout (timeout));


endmodule 


