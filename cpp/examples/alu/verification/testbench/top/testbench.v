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

`timescale 1 ns / 1 ns

module top;

   reg [31:0] operand_a;
   reg [31:0] operand_b;
   reg [7:0]  operand;
   wire       operation_done;
   wire [31:0] result;
   
   reg 	       op_valid;
   
   alu_top alu_top (
		    .operand_a (operand_a),
		    .operand_b (operand_b),
		    .operator (operand),
		    .op_valid (op_valid),
		    .operation_done (operation_done),
		    .result (result)
		    );


   initial op_valid = 0;
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
   

//   initial 
//     begin
//	$display ("%t 123 Starting Truss", $time);
//#100;
//	$verification_top;
//     end

   watchdog watchdog (.timeout (timeout));


endmodule 


