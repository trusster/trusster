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

`timescale 1 ns / 1 ns


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
module sub_module;
   reg       [122:0]  bank0  [0 : 312]; 

   always @(bank0[75])
      begin
	 $display ("%t from always block --> bank0[75] is %x\n", $time, bank0[75]);	 
      end
   

   integer index;
   initial
      begin
	 $display ("%t mem_test.v: %m initing sub module RAM.\n", $time);
	 index = 0;
	 while (index <= 312) begin
	    bank0[index] = index; //123'h0;
	    index = index + 1;
	 end
	 $display ("%t mem_test.v: %m initing sub module RAM done. \n", $time);     
      end

//   initial $teal_memory_note (bank0);

   initial begin
	#101
	if (bank0[312] != 'h3232888) $display ("%t ERROR: from _verilog --> memory[312] is %0d 0x%x\n", $time, bank0[312], bank0[312]);
   end 

   always @(bank0[44])
      begin
	 $display ("%t from always memory[44] --> bank0[44] is %0d 0x%x \n", $time, bank0[44], bank0[44]);	 
      end

   integer i;
   initial begin
      for (i = 0; i < 2; i = i + 1)  begin
	 #100
	$display ("%t from _verilog --> bank0[312] is %0d 0x%x\n", $time, bank0[312], bank0[312]);
      end
   end

endmodule


module top;
   reg       [17:0]  a_top_bank  [0 : 1717]; 

   always @(a_top_bank[175])
      begin
	 $display ("%t from always memory[175] --> a_top_bank[175] is %0d 0x%x \n", $time, a_top_bank[175], a_top_bank[175]);	 
      end
   
//   initial $teal_memory_note (a_top_bank);

   integer index;
   initial
      begin
	 $display ("%t mem_test.v: %m initing RAM.\n", $time);
	 index = 0;
	 while (index <= 1717) begin
	    a_top_bank[index] = index; //18'h0;
	    index = index + 1;
	 end
	 $display ("%t mem_test.v: %m initing RAM done. \n", $time);     
      end

   reg [1:0] a_top_memory;

   initial begin
	#101
	if (a_top_bank[1717] != 'h3) $display ("%t ERROR: from _verilog --> memory[1717] is %x\n", $time, a_top_bank[1717]);
   end 

   sub_module a_sub_module();

   integer i;
   initial begin
      for (i = 0; i < 2; i = i + 1)  begin
	 #100
	$display ("%t from _verilog --> memory[44] is %x\n", $time, a_top_bank[44]);
      end
   end

   always @(a_top_bank[44])
      begin
	$display ("%t from always block --> a_top_bank[44] is %x\n", $time, a_top_bank[44]);	 
      end
   

   reg init_done;
reg memory_change;
   initial begin
      init_done = 0; 
	memory_change = 0;
      init_done = #3 1;
      $display ("%t Verilog: signalling init_done", $time);
	#4;
	init_done = 0;
	memory_change = ~memory_change;
   end

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
  end


//   initial $teal_top;

endmodule 

