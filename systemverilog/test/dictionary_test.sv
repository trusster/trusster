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
`ifdef ncsim
`timescale 1 ns / 1 ns
`endif

///////////////////////////////////////////////
///////////////////////////////////////////////
program verification_top ();

initial begin
   teal::dictionary_read ("dictionary.txt");
   begin
`ifdef ncsim
      teal::file_vlog not_used;
      teal::vout log;
      not_used = new (teal::dictionary_find ("out_file"), 
				      teal::dictionary_find_integer ("interactive", 1));
	log = new ("dictionary_test");
`else
      teal::file_vlog not_used = new (teal::dictionary_find ("out_file"), 
				      teal::dictionary_find_integer ("interactive", 1));
      teal::vout log = new ("dictionary_test");
`endif

      @ (posedge (top.run_test[2]));

      begin

	 string a_string;
	a_string = teal::dictionary_find ("a_string");
	 if (a_string == "Hello_World!") 
	   begin
	      log.info ({" string_value: expected \"Hello_World!\", received \"",  a_string,  "\""});
	   end
	 else begin
	    log.error ({" string_value: expected \"Hello_World!\", received \"" , a_string, "\""});
	 end
      end


      begin
	 string b_string;
	b_string = teal::dictionary_find ("out_file");
	 if (b_string == "dictionary_test_results.txt") begin
	    log.info ({" string_value: expected \"dictionary_test_results.txt\"", 
                       "  received \"", b_string, "\"" });
	 end
	 else
	   log.error ({" string_value: expected \"dictionary_test_results.txt\"", 
                       "  received \"", b_string, "\"" });
      end

      begin
	 string c_string;
	 real a_double;
	 int foo;
	 
	c_string = teal::dictionary_find ("a_double");
	foo = $sscanf (c_string, "%g", a_double);
	 if (a_double == 1.22345)
	   log.info ($psprintf ("a_double: expected 1.22345, received %g", a_double));
	 else log.error ($psprintf ("a_double: expected 1.22345, received %g", a_double));
      end
      

      begin
	 integer an_int;
	an_int = teal::dictionary_find_integer ("version", 99);
	 if (an_int == 1)
	   log.info ($psprintf ( "version: expected 1, received %0d", an_int));
	 else log.error ($psprintf ( "version: expected 1, received %0d", an_int));
      end


      begin
	 bit found;
	string a_string2;
	
	found = teal::dictionary_put ("not_present", "a_value");
	a_string2 = teal::dictionary_find ("not_present");

	 if (found)
	   log.error ($psprintf ( "not_present: expected found 0, actual found %0d", found));
	 else log.info ($psprintf ( "not_present: expected found 0, actual found %0d", found));

	 if (a_string2 == "a_value")
	   log.info ($psprintf ("not_present: expected \"a_value\", received \"%0s\"", a_string2));
	 else log.error ($psprintf ("not_present: expected \"a_value\", received \"%0s\"", a_string2));
      end // begin

      begin
	 bit found2;
	 string a_string3;


	found2 = teal::dictionary_put ("to_be_overridden", "another_value", 1);
	a_string3 = teal::dictionary_find ("to_be_overridden");

	 if (found2)
	   log.info ($psprintf ("not_present: expected found 1, actual found %0d", found2));
	 else log.error ($psprintf ("not_present: expected found 1, actual found %0d", found2));

	 if (a_string3 == "another_value")
	   log.info ($psprintf ("to_be_overridden: expected \"another_value\", received \"%0s\"", a_string3));
	 else log.error ($psprintf ("to_be_overridden: expected \"another_value\", received \"%0s\"", a_string3));
      end
      
      begin
	 string a_string4;
	 integer a;
	 integer b;
	 int foo;

	a_string4  = teal::dictionary_find ("two_ints");
	foo  = $sscanf (a_string4, "%d %d", a, b);
	 if ((a === 45) && (b === 8890))
	   log.info ($psprintf ("a: expected 45, received %0d and b: expected 8890, received %0d", a, b));
	 else log.error ($psprintf ("a: expected 45, received %0d and b: expected 8890, received %0d", a, b));
      end

      begin
	 string c_string ;
	 integer hex_value;
	 int foo;
	 
   	c_string = teal::dictionary_find ("hex_value");
	foo = $sscanf (c_string, "%x", hex_value);

	 if (hex_value == 'habcdef0)
	   log.info ($psprintf ("hex_value: expected abcdef0, received %x", hex_value));
	 else log.error ($psprintf ("hex_value: expected abcdef0, received %x", hex_value));
      end
      
      
      teal::dictionary_clear ();

      begin
`ifdef ncsim
	   teal::vlog v;
	   v = teal::vlog_get ();
`else
	   teal::vlog v = teal::vlog_get ();
`endif
	 if (v.how_many (teal::vout_error)) begin
	    log.info ($psprintf ("Test Failed: Contained %0d errors",  v.how_many (teal::vout_error)));
	 end
	 else begin
	    log.info ("Test Passed. ");
	 end
      end
   end
end // initial begin
   endprogram

module interfaces_dut;
endmodule
