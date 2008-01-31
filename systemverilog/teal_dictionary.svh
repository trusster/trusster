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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////// Dictionary /////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class dictionary_impl;
   extern function string find_on_command_line (string name, string default_name);
   extern task read (string path);
   extern task clear ();
   extern function bit put (string name, string value, input bit replace_existing = 1);
   extern function string find (string name); 
   extern function integer find_integer (string name, integer default_value);
   local string lines_[string];
      local vout log_;
      extern function new ();
    extern local task process_file_ (string path);
       extern local function string teal_scan_plusargs (string name);
 endclass // dictionary_impl


   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function automatic dictionary_impl dictionary_get ();
      static dictionary_impl lv;  //singleton
      if (lv == null) lv = new ();
      return (lv);
   endfunction


//only seaches command line args. Useful to get a file name to start dictionary with.
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string dictionary_find_on_command_line (string name, string default_name);
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif
  return (impl.find_on_command_line (name, default_name));
endfunction

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task dictionary_read (string path);
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif

   impl.read (path);
endtask

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//removes all entries
task dictionary_clear ();
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif

   impl.clear ();
endtask
      

parameter bit replace_entry = 1;
parameter bit default_only = 0;

//returns 1 if placed or overwritten
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit dictionary_put (string name, string value, input bit replace_existing = 1);
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif
   return (impl.put (name, value, replace_existing));
endfunction // bit
      

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string dictionary_find (string name);  //returns "" if not found in constraints or command line
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif
   return (impl.find (name));
endfunction   
      

//sets to default if not found
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function automatic integer dictionary_find_integer (string name, integer default_value);
`ifdef ncsim
  dictionary_impl impl;
  impl = dictionary_get ();
`else
  dictionary_impl impl = dictionary_get ();
`endif
   return (impl.find_integer (name, default_value));
endfunction   


`include "teal_dictionary.sv"
