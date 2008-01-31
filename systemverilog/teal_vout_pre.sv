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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal_vout.svh

//How the &*&F^* to hide this from the package scope?


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void message_list::put_message (integer id, string value);
`ifdef ncsim
   message m;
  m = new ();
`else
   message m = new ();
`endif
   m.id = id;
   m.value = value;
   the_list_.push_back (m);
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string message_list::convert_to_string (output bit fatal_seen);
   string returned;
//   $display ("convert to string");
   for (integer i = 0; i < the_list_.size(); ++i) begin
//      $display ($psprintf ("Covert adding %s . Total is %s", the_list_[i].value, returned));
      returned = {returned, the_list_[i].value};
//	 $display ($psprintf ("Covert after adding %s . Line is %s", the_list_[i].value, returned));
    if (the_list_[i].id == vout_fatal) fatal_seen = 1;
   end
   return (returned);
endfunction

parameter integer string_npos = -1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function integer str_find (string s1, string key);
//   $display ("str_find: look for key \"%s\" in \"%s\"", key, s1);
   if (s1 == key) return 0; 
   
   for (integer i = 0; i < (s1.len () - key.len()); ++i) begin
      integer j;
      j = 0; 
      for (int questa = 0; j <key.len (); ++j) begin
	// $display ("Compare %c (%0d) with %c (%0d)", s1.getc (i + j), i + j, key.getc (j), j);
	 
	 if (s1.getc (i + j) != key.getc (j))begin
	    break;
	 end
	 
      end
      if ((i != (s1.len () - key.len())) && (j == key.len ())) begin
	 //$display ("teal_vout_pre.sv, str_find returning found \"%s\" in \"%s\" at index %0d",  key, s1, i);
	 return i;
      end
   end
   return string_npos;
endfunction // str_find

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function vlog::new ();
   fatal_message_seen_ = 0;
   after_me_ = the_;
   the_ = this;
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class local_vlog extends vlog;
   protected virtual function string local_print_ (string val);
//      $display ("will print \"%s\"", val, 0, 0,0);  //don't ask
      $display (val);
      if ((fatal_message_seen_) && (str_find (val, "FATAL") != 0)) begin
	 $finish (); //not good enough ???
      end
      fatal_message_seen_ = 0;
      return val;
   endfunction // string

   protected virtual function automatic message_list output_message_ (message_list m);
`ifdef ncsim
      string val;
      vlog v;

     val = m.convert_to_string (fatal_message_seen_);
      v = vlog_get ();
`else
      string val = m.convert_to_string (fatal_message_seen_);
      vlog v = vlog_get ();
`endif
//   $display ("after convert: will print \"%s\"", val, 0, 0,0);  //don't ask
      for (integer i = 0; i < m.the_list_.size(); ++i) begin
	 id_count_[m.the_list_[i].id]++;
      end
      
      v.local_print (val);
     return m;
   endfunction
endclass

