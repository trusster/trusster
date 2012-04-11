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
`ifndef __teal_memory__
 `define __teal_memory__

parameter int MAX_DATA = 1024;

virtual class memory_bank;
   extern function new  (string path);

      //the two methods are pure virtual, you must implement them
`ifdef ncsim
 virtual task from_memory (bit [63:0] address, output bit [MAX_DATA - 1:0] value, input int size); endtask
 virtual task to_memory (bit [63:0] address, bit [MAX_DATA - 1:0]  value, int size); endtask
`else
 `PURE virtual task from_memory (bit [63:0] address, output bit [MAX_DATA - 1:0] value, input int size); 
 `PURE virtual task to_memory (bit [63:0] address, bit [MAX_DATA - 1:0]  value, int size); 
`endif
   extern function bit contains1 (string path);
   extern function integer contains2 (bit [63:0] address);
   extern function string stats ();
   extern function void locate (bit [63:0] first_address, bit [63:0] last_address);
      
   local bit [63:0] first_address_;
   local bit [63:0] last_address_;
   protected vout log_;
endclass


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//internal singleton
class memory_impl;
   extern function void add_map (string path, bit [63:0] first_address, bit [63:0] last_address);
   extern task read (bit [63:0] global_address, output bit [MAX_DATA - 1:0] value, input int size = MAX_DATA);
   extern task write (bit [63:0] global_address, input bit [MAX_DATA - 1:0] value, int size = MAX_DATA);
   extern function void add_memory_bank (/*owner*/ memory_bank bank);
   extern function /*cached*/ memory_bank lookup1 (bit [63:0] address_in_range);
   extern function /*cached*/ memory_bank lookup2 (string partial_path);

   local memory_bank memory_banks_[$];
   local vout log_;
   function new (); log_ = new ("teal::memory_implmentation"); endfunction
endclass 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function memory_impl memory_get ();
   static memory_impl lv;// = new ();  //singleton
   if (lv == null) lv = new ();
   return (lv);
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The "normal" way to access memory...

//setup a mapping between some global address space and some memory
function void  add_map (string path, bit [63:0] first_address, bit [63:0] last_address);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif
   impl.add_map (path, first_address, last_address);
endfunction


//now access it...
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task read (bit [63:0] global_address, output bit [MAX_DATA - 1:0] value, input int size = MAX_DATA);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif

   impl.read (global_address, value, size);
endtask // teal_add_map

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task write (bit [63:0] global_address, input bit [MAX_DATA - 1:0] value, int size = MAX_DATA);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif

   impl.write (global_address, value, size);
endtask // teal_add_map

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void add_memory_bank (/*owner*/ memory_bank bank);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif

   impl.add_memory_bank (bank);
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function /*cached*/ memory_bank memory_lookup1 (bit [63:0] address_in_range);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif

   return (impl.lookup1 (address_in_range));
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function /*cached*/ memory_bank memory_lookup2 (string partial_path);
`ifdef ncsim
   memory_impl impl;
   impl = memory_get ();
`else
   memory_impl impl = memory_get ();
`endif

   return (impl.lookup2 (partial_path));
endfunction

 `include "teal_memory.sv"
`endif //  `ifndef __teal_memory__
