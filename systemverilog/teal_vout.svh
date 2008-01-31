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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.svh



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Teal predefined message IDs (reserved from 0x800 - 0xf00)
    //Note: Applications can use 0x00 through 0x7ff
    typedef enum {vout_first_id = 32'h800, vout_time, vout_thread_name, vout_functional_area,
		  vout_fatal, vout_error, vout_info, vout_debug, vout_message_data, vout_endl, 
		  vout_file, vout_line, vout_endm, vout_last_id} teal_predefined_message_ids;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class message; integer id; string value; endclass
class message_list;
   extern function void put_message (integer id, string value);
   extern function string convert_to_string (output bit fatal_seen);
    message the_list_[$]; //would like it to be friend to vlog
endclass

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class vlog;
   extern function new ();
   extern function vlog get ();
      
   extern function void output_message (message_list msg);
   extern function void local_print (string val);
    extern function integer how_many (integer an_id); //given a meta-info tag, how many got printed?


    protected virtual function automatic message_list output_message_ (message_list m);
	      return m; 
            endfunction


`ifdef ncsim
    protected virtual function string local_print_ (string val); endfunction
`else
    `PURE protected virtual function string local_print_ (string val); 
`endif


    static vlog the_; 
    protected static int id_count_[*];

    protected vlog after_me_; //use by the push logic to create a chain (intrusive list)
    protected bit fatal_message_seen_;
  endclass

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class file_vlog  extends vlog;
    extern function new (string file_name, bit also_to_screen = 1);

    extern protected virtual function string local_print_ (string val);
 
      local integer out_file_; 
      local bit also_to_screen_;
 endclass


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    parameter  integer debug = 1; //can be used with debug_level()
    parameter  integer no_debug = 0; //can be used with debug_level()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 class vout;
	extern function new (string functional_area, integer initial_show_level = no_debug);
        extern function string name ();

	extern function void info (string msg);
	extern function void  error (string msg);
	extern function void  fatal (string msg);
	extern function void  debug (string msg);
	extern function void  debug_n (integer level, string msg);

	extern function integer show_debug_level (integer new_one);


	//nominally the enum info_type, but left as int for expansion
	//The first way to not print some bit of meta data.
      extern  virtual function /*previous*/ bit message_display (integer id, bit new_value);
	 extern protected function void put_line_ (integer id, string value, string msg, integer level);
	    


	local string functional_area_;
	local integer show_debug_level_;
	local bit message_display_[*]; 
endclass // vout

`include "teal_vout_pre.sv"
   
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function vlog vlog_get ();
      static local_vlog lv;
      if (lv == null) lv = new ();
      return (lv.get ());
   endfunction

`include "teal_vout_post.sv"

