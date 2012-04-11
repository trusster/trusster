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
function vout::new (string functional_area, integer initial_show_level);
  functional_area_ = functional_area;
  show_debug_level_ = dictionary_find_integer ({functional_area, "_show_debug_level"}, initial_show_level);
//   show_debug_level_ = 1;
   
   for (integer i = vout_first_id; (i < vout_last_id); ++i) begin
    message_display_ [i] = 1;
  end
endfunction // vout

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function void vlog::output_message (message_list msg);
`ifdef ncsim
      message_list msg2;
      msg2 = output_message_ (msg); 
`else
      message_list msg2 = output_message_ (msg); 
`endif
      if (after_me_ != null) after_me_.output_message (msg2);
    endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function void  vlog::local_print (string val);
`ifdef ncsim
      string val2;
      val2 = local_print_ (val);
`else
      string val2 = local_print_ (val);
`endif
//      $display ("after local local_print_ after_me %0d will print \"%s\"", (after_me_ != null), val2, 0, 0,0);  //don't ask
      if ((after_me_ != null) && (val2 != "")) after_me_.local_print (val2);
    endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string vout::name ();
  return (functional_area_);
endfunction // vout

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function /*previous*/ bit vout::message_display (integer id, bit new_value);
`ifdef ncsim
   bit returned;
   returned = message_display_[id];
`else
   bit returned = message_display_[id];
`endif
   message_display_[id] = new_value;
   return returned;
endfunction
	 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::info (string msg);
   put_line_ (vout_info, "[INFO]", msg, 0);
endfunction // vout

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::error (string msg);
   put_line_ (vout_error, "[ERROR]", msg, 0);
endfunction 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::fatal (string msg);
   put_line_ (vout_fatal, "[FATAL]", msg, 0);
endfunction 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::debug (string msg);
   put_line_ (vout_debug, "[DEBUG]", msg, 1);
endfunction // vout

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::debug_n (integer level, string msg);
   put_line_ (vout_debug, "[DEBUG]", msg, level);
endfunction // vout

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vout::put_line_ (integer id, string value, string msg, integer level);
   string t;
`ifdef ncsim
   message_list a_message_list;
   a_message_list = new ();
`else
   message_list a_message_list = new ();
`endif

//   $display ("put line %d %s %s", id, value, msg);
   
    $sformat (t, "[%0t]", $time);
    
   if (message_display_[vout_time]) a_message_list.put_message (vout_time, t);
   if (message_display_[vout_info]) a_message_list.put_message (id, value);
   if (message_display_[vout_functional_area]) 
     a_message_list.put_message (vout_functional_area, {"[", functional_area_ ,"]"});
//  put_message (vout_thread_name, "[" + thread_name (pthread_self()) + "]");
   if (message_display_[vout_message_data]) 
     a_message_list.put_message (vout_message_data, msg);
   begin
`ifdef ncsim
      vlog one;
      one = vlog_get();
`else
      vlog one = vlog_get();
`endif
// $display ($psprintf ("current %d show %d", level , show_debug_level_));      
      if (level <= show_debug_level_) one.output_message (a_message_list);
   end
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function integer vout::show_debug_level (integer new_one);
`ifdef ncsim
    integer returned;
    returned = show_debug_level_; show_debug_level_ = new_one; return (returned); endfunction
`else
    integer returned = show_debug_level_; show_debug_level_ = new_one; return (returned); endfunction
`endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string file_vlog::local_print_ (string val);
   $fwrite (out_file_, "%s\n", val);
//      $display ("file vout: after local local_print_ after_me %0d to_screen %0d will print \"%s\"", (after_me_ != null), also_to_screen_, val, 0, 0,0);  //don't ask

  if (also_to_screen_) return val; else return "";
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function vlog vlog::get ();
   return (the_);
endfunction // vlog

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function file_vlog::new (string file_name, bit also_to_screen);
   super.new ();
   out_file_ = $fopen (file_name, "w");
   if (out_file_ == 0) $display ({"Unable to open file: ", file_name});
   $display ({"opened file: ", file_name});
   also_to_screen_ = also_to_screen;
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function integer vlog::how_many (integer an_id); //given a meta-info tag, how many got printed?
   return  id_count_[an_id];
endfunction // vlog

     
