/*
 * Copyright (c) 2004 Mike Mintz (mike@applevalleysoftware.com)
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

`timescale 1 ps / 1 ps
module sub_module;
   reg [55:0] a_sub_vreg;
   initial a_sub_vreg = 56'h234567891200;

   always @(a_sub_vreg)
      begin
	$display ("[%0t ns] verilog: from always block --> a_sub_vreg is %x", $time, a_sub_vreg);	 
      end
   
endmodule

module sub2;
	reg a_good_path;
	initial begin
	a_good_path = #10 ~a_good_path;
	end
endmodule

module sub1;
	sub2 sub2();
endmodule

module top;
   reg [1:0] a_top_vreg;

   wire [5:0] a_wire;
   pullup (a_wire[0]);
   pulldown (a_wire[1]);

   initial begin
      a_top_vreg = 2'b0;
//      a_top_vreg = #5 ~a_top_vreg;
//      a_top_vreg = #15 ~a_top_vreg;
//      a_top_vreg = #25 ~a_top_vreg;
   end 

   sub_module a_sub_module();
   sub1 sub1();

   integer i;
   initial begin
      for (i = 0; i < 10; i = i + 1)  begin
	 #3
	       $write("[%0t ns] Verilog: --> vreg is %x\n", $time, a_top_vreg);
      end
   end

   always @(a_top_vreg)
      begin
	$display ("%t from always block --> a_top_vreg is %x\n", $time, a_top_vreg);	 
      end
   
   always @(a_wire)
      begin
	$display ("%t from always block --> a_wire is %b\n", $time, a_wire);	 
      end


   initial $verification_top;

   reg init_done;
   initial begin
      init_done = 0; 
      init_done = #10 1;
   end


 integer  j;
 reg clk;
  initial begin
	clk <= 0;
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
	 #10;
 	  clk <= ~clk; 
  end

endmodule 

