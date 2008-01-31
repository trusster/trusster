/*
Trusster Open Source License version 1.0a (TRUST)
copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
   
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  * Redistributions in any form must be accompanied by information on how to obtain 
    complete source code for this software and any accompanying software that uses this software.
    The source code must either be included in the distribution or be available in a timely fashion for no more than 
    the cost of distribution plus a nominal fee, and must be freely redistributable under reasonable and no more 
    restrictive conditions. For an executable file, complete source code means the source code for all modules it 
    contains. It does not include source code for modules or files that typically accompany the major components 
    of the operating system on which the executable file runs.
 

THIS SOFTWARE IS PROVIDED BY MIKE MINTZ AND ROBERT EKENDAHL ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL MIKE MINTZ AND ROBERT EKENDAHL OR ITS CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
nOR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


`include "teal.svh"

`ifdef MTI
import "DPI-C" pure function real sqrt (real x);
`else
`ifdef ncsim
package arrgh;
import "DPI-C" pure function real sqrt3 (real x);
function real sqrt2 (real x); return sqrt3(x); endfunction
endpackage;

`timescale 1ns/1ns;

`else
import "DPI" pure function real sqrt2 (inout realtime x);
`endif
`endif

package local_fcns;   

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function realtime abs_f (realtime lhs);
//return ((lhs < 0) ? -lhs : lhs);
 if (lhs < 0) 
	return (-lhs);
	else return (lhs);
endfunction
endpackage

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
program   verification_top ();

`ifdef ncsim
      teal::file_vlog not_used;
      teal::vout log;
   int histogram_8[*];
   int histogram_range[*];
   int histogram_32[*];
initial begin:initial_block
      not_used = new (teal::dictionary_find ("out_file"), 
				      teal::dictionary_find_integer ("interactive", 1));
	log = new ("dictionary_test");
`else
initial begin:initial_block
      teal::file_vlog not_used = new (teal::dictionary_find ("out_file"), 
				      teal::dictionary_find_integer ("interactive", 1));
      teal::vout log = new ("dictionary_test");
   int histogram_8[*];
   int histogram_range[*];
   int histogram_32[*];
`endif


   @ (posedge (top.run_test[7]));

   //in production, choose one of the two
   teal::vrandom_init_with_file ("dictionary.txt"); 
   teal::vrandom_init_with_seed (teal::dictionary_find_integer ("seed", 0));

   for (integer i = 0; i < (12_000* 5); ++i) begin
      bit[31:0] x;
      bit[31:0] y;
      
      `RAND_8 (x)
	++histogram_8[x];
      `RAND_RANGE (y, 45, 12045)
      
	++histogram_range[y];
      `RAND_32 (x)
	++histogram_32[x];
   end

   begin
`ifdef ncsim
      integer foo;
      foo = $fopen ("data.txt", "w+");
      assert (foo) else $fatal (03301999);
`else
      integer foo = $fopen ("data.txt", "w+");
      assert (foo) else $fatal ("Unable to open data file for writing");
`endif

      for (integer i = 0; (i < 256); ++i) begin
	 string msg; int dummy;
	 $fwrite (foo, "%0d \n", histogram_8[i]);
//	 $display ($psprintf ("histo[%0d} = %0d", i, histogram_8[i]));
      end
      $fclose (foo);
   end

   begin
`ifdef ncsim
      integer foo;
      foo = $fopen ("data2.txt", "w+");
      assert (foo) else $fatal (10051984);
`else
      integer foo = $fopen ("data2.txt", "w+");
      assert (foo) else $fatal ("Unable to open data file for writing");
`endif
      for (integer i = 45; (i < 12_045); ++i) begin
	 string msg; int dummy;
	 $fwrite (foo, "%0d \n", histogram_range[i]);
//	 $display ($psprintf ("histo[%0d} = %0d", i, histogram_range[i]));
      end
      $fclose (foo);
   end
   begin
`ifdef ncsim
      integer foo;
      foo = $fopen ("data3.txt", "w+");
      assert (foo) else $fatal (03271962);
`else
      integer foo = $fopen ("data3.txt", "w+");
      assert (foo) else $fatal ("Unable to open data file for writing");
`endif
      for (integer i = 0; (i < 16_000); ++i) begin
	 string msg; int dummy;
	 $fwrite (foo, "%0d \n", histogram_32[i]);
//	 dummy = $sformat (msg, "histo[%0d} = %0d", i, histogram_32[i]);
//	 $display ($psprintf ("histo[%0d} = %0d", i, histogram_32[i]));
      end
      $fclose (foo);
   end
   //check that distribution is normal
   //compute standard distribution and verify that rand is < 1 sigma
   begin: main_checking
`ifdef ncsim
      realtime mean;
      realtime  mean_of_squares;
      mean= 0;
      mean_of_squares = 0;
`else
      realtime mean= 0;
      realtime  mean_of_squares = 0;
`endif
      for (int i= 0; i < 256; ++i) begin
	 mean += histogram_8[i];
	 mean_of_squares += histogram_8[i] * histogram_8[i];
      end

      //normalize
      mean /= 256;
      mean_of_squares /= 256;
      begin : generate_std_deviation
	 realtime standard_deviation;
	 realtime temp;
	 temp = local_fcns::abs_f ( mean_of_squares - (mean*mean));
`ifdef MTI
	 standard_deviation = sqrt (temp);
`else
`ifdef ncsim
	 standard_deviation = arrgh::sqrt2 (temp);
`else
	 standard_deviation = sqrt2 (temp);
`endif
`endif
	 if (standard_deviation < 1) standard_deviation = 1;  //correct for "spot on target"
	 
	 begin
	    string msg;
//	    int dummy =  $sformat (msg, "mean %0f and  mean of squares %0f standard deviation %0f temp %0f",  
//				   mean, mean_of_squares, standard_deviation, temp);
      $display ("mean of squares is %0f", mean_of_squares);
      $display ("mean is %0f sqared is %0f ", mean, mean*mean);
      $display ("temp is %0f", temp);
      $display ("standard deviation is %0f", standard_deviation);	    
	    log.info ($psprintf ("mean %0f and  mean of squares %0f standard deviation %0f temp %0f",  
				   mean, mean_of_squares, standard_deviation, temp));
	 end


	 begin  : std_deviation_check
	    int num_within_one;
	    bit [31:0] num_within_two;
	    num_within_one = 0;
	    num_within_two = 0;
	    for (int i= 0; i < 256; ++i) begin
	       if (local_fcns::abs_f (histogram_8[i] - mean) <= standard_deviation) ++num_within_one;
	       if (local_fcns::abs_f (histogram_8[i] - mean) <= (2 *standard_deviation)) ++num_within_two;
	    end

	    begin
	       string msg;
		msg = $psprintf ("%0d were within one standard deviation. Expected at least: %0d",
				      num_within_one, (0.68 * 256));
	       if (num_within_one >= (0.68 * 256))  log.info (msg);  else log.error (msg) ;
	    end
	    
	    begin
	       string msg;
	       msg = $psprintf ("%0d  were within two standard deviations. Expected at least: %0d",
				      num_within_two, (0.95 * 256));
	       if (num_within_two >= (0.95 * 256)) log.info (msg); else log.error (msg);
	    end // begin
	 end // block: std_deviation_check
      end // block: generate_std_deviation
   end // block: main_checking
   

   //check that distribution is normal
   //compute standard distribution and verify that rand is < 1 sigma
   begin : main_checking_0
`ifdef ncsim
      realtime mean;
      realtime  mean_of_squares;
	mean = 0;
	mean_of_squares = 0;
`else
      realtime mean= 0;
      realtime  mean_of_squares = 0;
`endif
      for (int i= 45; i < 12_000; ++i) begin
	 mean += histogram_range[i];
	 mean_of_squares += histogram_range[i] * histogram_range[i];
      end
      //normalize
      mean /= 12_000;
      mean_of_squares /= 12_000;
      begin : gen_std_deviation
	 realtime temp;
	 realtime standard_deviation;
	temp = local_fcns::abs_f ( mean_of_squares - (mean*mean));
`ifdef MTI
	standard_deviation = sqrt (temp);
`else
`ifdef ncsim
	 standard_deviation = arrgh::sqrt2 (temp);
`else
	 standard_deviation = sqrt2 (temp);
`endif
`endif
      $display ("square root of %f0f is %0f", temp, standard_deviation);	    

	 begin
	    string msg;
	msg = $psprintf ("mean %0f and  mean of squares %0f standard deviation %0f temp %0f",  
				   mean, mean_of_squares, standard_deviation, temp);
      $display ("mean of squares is %0f", mean_of_squares);
      $display ("mean is %0f sqared is %0f ", mean, mean*mean);
	    
      $display ("temp is %0f", temp);
      $display ("standard deviation is %0f", standard_deviation);	    
	    log.info (msg);
	 end


	 begin :  check_std_deviation
	    int num_within_one;
	    bit [31:0] num_within_two;
	    num_within_one = 0;
	    num_within_two = 0;

	    for (int i= 45; i < 12_000; ++i) begin
	       if (local_fcns::abs_f (histogram_range[i] - mean) <= standard_deviation) ++num_within_one;
	       if (local_fcns::abs_f (histogram_range[i] - mean) <= (2 *standard_deviation)) ++num_within_two;
	    end

	    begin
	       string msg;
		msg = $psprintf ("%0d were within one standard deviation. Expected at least: %0d",
				      num_within_one, (0.68 * 12_000));
	       if (num_within_one >= (0.68 * 12_000))  log.info (msg);  else log.error (msg) ;
	    end
	    
	    begin
	       string msg;
		msg = $psprintf ("%0d were within two standard deviations. Expected at least: %0d",
				      num_within_two, (0.95 * 12_000));
	       if (num_within_two >= (0.95 * 12_000)) log.info (msg); else log.error (msg);
	    end // begin
	 end // block: check_std_deviation
      end // block: gen
   end // block: main_checking
      
      begin
`ifdef ncsim
	   teal::vlog v;
	   v = teal::vlog_get ();
`else
	   teal::vlog v = teal::vlog_get ();
`endif
	 if (v.how_many (teal::vout_error)) begin
	    log.info ($psprintf ("Test Failed: Contained %0d error(s).",  v.how_many (teal::vout_error)));
	 end
	 else begin
	    log.info ("Test Passed. ");
	 end
      end
end // initial begin
   
   endprogram

module interfaces_dut;
endmodule
