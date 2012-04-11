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

module trans(test_done);
   input test_done;
   
   reg do_work;
   reg work_done;
   reg [10:0] op_code; //2048 tasks
   reg [31:0] totals  [0:1255]; //1255 tasks, 2**32 iterations
   initial $teal_memory_note (totals);

   integer    index;
   initial
      begin
	 $display ("%t ns: %m initing totals.\n", $time);
	 index = 0;
	 while (index <= 1255) begin
	    totals[index] = 32'h0;
	    index = index + 1;
	 end
	 $display ("%t ns: %m initing totals done.\n", $time);
      end


   initial
      begin
	 do_work = 0;
      end
	 
   always @(posedge (do_work))
      begin
	 work_done = 0;
	 do_work = 0;	       
	 
	 totals[op_code] = totals[op_code] + 1;
//	 $display ("%t ns Verilog %m Received op_code %d", $time, op_code);
	 #1; //pretend the transactor takes some time
	 work_done <= 1;  //non blocking so as not to miss the posedge.
      end

`ifdef hjklhjkhjkl
   always @(posedge (test_done))
      begin
	 index = 0;
	 while (index <= 20) begin
	    $display ("%t ns: %m totals[%0d]: %d.\n", $time, index, totals[index]);
	    index = index + 1;
	 end
   end // always @ (posedge (test_done))
`endif   

   endmodule // trans

module top;
   reg test_done;
   initial begin
      test_done = 0; 
      @ (posedge (test_done));
      #1 //give trans time to print
      $display ("%0t ns VERILOG: Received exit from Teal. Exiting simulation.", $time);
      $finish ();
   end

   trans trans0(test_done);
   trans trans1(test_done);
   trans trans2(test_done);
   trans trans3(test_done);
   

`ifdef hjklhjkhjkl
   //Usually, its teal that decides when to quit. But this test is so simple,
   //I could not come up with a plausable "when".
   reg simple_test_done;
   initial begin
      simple_test_done = 0;
      #400;
      $display ("%0t ns VERILOG: Requesting that Teal exit.", $time);
      simple_test_done = 1;
   end
`endif
   
   reg init_done;
   initial begin
      init_done = 0; 
      init_done = #1 1;
   end
   initial 
      begin
	 $verification_top;
      end 
   
`define hjklhjkhjkl
`ifdef hjklhjkhjkl
   reg keep_alive;
    initial
      begin
	 keep_alive = 0;
      end
   always @(keep_alive)
      begin
	 keep_alive <= #40 !keep_alive;
      end
`endif   
   
endmodule 
