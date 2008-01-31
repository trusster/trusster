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

module top;

   reg [1:0] top_signal;
   reg [31:0] add_one_var;
   integer i;
   initial
      begin
	 top_signal = 'h0;
	 add_one_var = 'h0;	 
	 for (i = 0; i < 10 ;  i = i + 1)  begin 
	    top_signal = #7 ~top_signal; 
	 end
      end 
   
   always @(top_signal)
      begin
	 $write ("[%0t ns] VERILOG: top signal changed is now %x\n", $time, top_signal);	 
      end

   always @(add_one_var)
      begin
	 $write ("[%0t ns] VERILOG: add_one_var changed is now %x\n", $time, add_one_var);	 
      end

   reg clk;
   initial 
      begin
	 clk = 0;
      end


   reg [3:0] toggle_var;
   reg  toggle_var_changed;
   integer j;
   initial
      begin
	 toggle_var = 0;
	toggle_var_changed = 0;
      end 


   always @(toggle_var)
      begin
	 $write ("[%0t ns] VERILOG:: toggle_var changed is now 0x%x\n", $time, toggle_var);	 
	toggle_var_changed = ~toggle_var_changed;
      end

   initial 
      begin
	#1
	 $verification_top;
      end 

   integer k;
   initial
      begin
	 for (k = 0; k < 11 ; k = k + 1)  begin 
	    clk = #5 ~clk; 
	 end 
      end 

   reg test_done;
   initial begin
      test_done = 0; 
     @ (posedge (test_done));
      $write ("[%0t ns] VERILOG: Received exit from Teal. Exiting simulation.\n", $time);
      $finish ();
   end


   //Usually, its teal that decides when to quit. But this test is so simple,
   //I could not come up with a plausable "when".
   reg simple_test_done;
   initial begin
      simple_test_done = 0;
//      #22;
      #400;
      $write ("[%0t ns] VERILOG: Forcing exit from Teal.\n", $time);
      simple_test_done = 1;
   end

   reg init_done;
   initial begin
      init_done = 0; 
      init_done = #1 1;
   end


endmodule 
