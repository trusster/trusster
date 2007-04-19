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
task message_list::put_message;
   the_list_.push_bask (m);
endtask // message_list


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string message_list::convert_to_string (output bit fatal_seen);
   string returned;
   for (integer i = 0; i < the_list_.size(); ++i) begin
      returned = {returned, the_list_[i].value};
      if (the_list_[i].id == vout_fatal) fatal_seen = 1;
   end
   return (returned);
endfunction


parameter int string_npos = -1;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function integer str_find (string s1, string key);
   for (integer i = 0; i < (s1.len () - key.len()); ++i) begin
      integer j = 0;
      for (; j < s2.len (); ++j) begin
	if (s1[i + j] != key[j]) break;
      end
      if (j == key.len ()) return i;
   end
   return string_npos;
endfunction // str_find

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class local_vlog extends :vlog;
   function string local_print_ (const ref string val);
      $display (val);
      if ((fatal_message_seen_) && (str_find (val, "FATAL") != 0)) begin
	 $finish (); //not good enough
      end
      fatal_message_seen_ = false;
      return val;
   endfunction // string

   protected virtual function message_list output_message_ (const ref message_list m);
      string val = m.convert_to_string (fatal_message_seen_);
      vlog v = vlog_get ();
      v.local_print (val);
     return m;
   endfunction
endclass



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function vout::new (string functional_area, integer initial_show_level);
  functional_area__ = functional_area;
//  show_debug_level_ = teal_dictionary_find_integer (functional_area, "_show_debug_level"}, initial_show_level;
   message_display_[vout::time] = 1;
endfunction // vout


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string file_vlog::local_print_ (const ref string val);
   $display ("writing to file /"%s/"", val);
   
   $fwrite (out_file_, val);
   return (also_to_screen_) ? val : "";
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function file_vlog::new (string file_name, bit also_to_screen);
   super.new ();
   out_file_ = $fopen (file_name, "w");
   if (out_file_ == 0) $display ({"Unable to open file: ", file_name});
   $display ({"opened file: ", file_name});
   also_to_screen_ = also_to_screen;
endfunction


#error ksahchvfsfhshkjl