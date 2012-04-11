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
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

`include "teal.svh"

`timescale 1 ns / 1 ns
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
program  verification_top ();
initial
  begin
`ifdef ncsim
     teal::vout log;
     log = new ("vout_test");
`else
     teal::vout log = new ("vout_test");
`endif

     @ (posedge (top.run_test[7]));
     begin
`ifdef ncsim	
	teal::file_vlog not_used;
	int foo;
        not_used = new (teal::dictionary_find ("out_file"), 
					teal::dictionary_find_integer ("interactive", 1));
`else
	int foo;
	teal::file_vlog not_used = new (teal::dictionary_find ("out_file"), 
					teal::dictionary_find_integer ("interactive", 1));
`endif
	
	//Test some debug logging
	foo = log.show_debug_level (teal::no_debug);
	log.debug ("Should not be displayed.");
	log.debug_n (5, "Level 5 Should not be displayed.");

	foo = log.show_debug_level (teal::debug);
	log.debug ("A debug message.");

	foo = log.show_debug_level (4);
	log.debug_n (5, "Level 5 Should not be displayed.");

	foo = log.show_debug_level (5);
	log.debug_n (5, "A \"Level 5\" debug message.");

	//normal debug should revert to the default, which is one
	foo = log.show_debug_level (0);
	log.debug ("Should not be displayed.");
	log.error ("show see this message");

	begin
`ifdef ncsim
	   teal::vlog v;
	   v = teal::vlog_get ();
`else
	   teal::vlog v = teal::vlog_get ();
`endif
	   if ( (v.how_many (teal::vout_error) != 1) 
		|| (v.how_many (teal::vout_debug) != 2)  ) begin
	      log.error ($psprintf ("Test Failed: Contained %0d errors and %0d debug messages",  
				    v.how_many (teal::vout_error), v.how_many (teal::vout_debug)));
	   end
	   else begin
	      log.info ($psprintf ("Test Passed: Contained %0d errors and %0d debug messages",  
				   v.how_many (teal::vout_error), v.how_many (teal::vout_debug)));
	   end
	end
     end
     
  end // initial begin
   endprogram
      
      

module interfaces_dut;
endmodule
