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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function memory_bank::new  (string path);
   log_ = new (path);
endfunction // new

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit memory_bank::contains1 (string path);
   return ((str_find (log_.name (), path) == string_npos) ? 0 : 1);
endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function integer memory_bank::contains2 (bit [63:0] address);
   return ((address >= first_address_) && (address <= last_address_) ? (address - first_address_) : -1);
endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void memory_bank::locate (bit [63:0] first_address, bit [63:0] last_address);
   first_address_ = first_address;
   last_address_ = last_address;
//   log_.info ($psprintf ("Locate [0x%0x to 0x%0x]", first_address, last_address));
endfunction


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string memory_bank::stats ();
   return $psprintf ("name: \"%s\" [0x%x, 0x%x]", log_.name (), first_address_, last_address_);
endfunction 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void memory_impl::add_map (string path, bit [63:0] first_address, bit [63:0] last_address);
   int found_index;
  found_index = -1;
   for (int i = 0; i < memory_banks_.size (); ++i) begin
      if (memory_banks_[i].contains1 (path)) begin
	 if (found_index == -1) begin
	    found_index = i;
	 end
	 else begin
	    log_.error ( $psprintf ("teal::add_map() Found two banks that match path \"%s\"  %s and %s",
			      path, memory_banks_[i].stats (), memory_banks_[found_index].stats ()));
	 end
      end
   end // for (int i = 0; i < memory_banks_.size (); ++i)
   
   if (found_index == -1) begin
      log_.error ($psprintf ("teal::add_map() Found no banks (of %0d)  that match path \"%s\"", memory_banks_.size (), path));      
   end
   else memory_banks_[found_index].locate (first_address, last_address);
endfunction

   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task memory_impl::read (bit [63:0] global_address, output bit [MAX_DATA - 1:0] value, input int size);
`ifdef ncsim
   bit found;
   int found_index;
   found = 0;
   found_index = -1;
`else
   bit found = 0;
   int found_index = -1;
`endif
   for (int i = 0; i < memory_banks_.size (); ++i) begin
`ifdef ncsim
      integer offset;
      offset = memory_banks_[i].contains2 (global_address);
`else
      integer offset = memory_banks_[i].contains2 (global_address);
`endif
      if (offset != -1) begin
	 if (found) begin
	    log_.error ($psprintf ("Found two banks that contain address 0x%x. %s and %s",
			      global_address, memory_banks_[i].stats (), memory_banks_[found_index].stats ()));
	 end
	 else begin
	    found = 1;
	    found_index = i;
	    memory_banks_[i].from_memory (offset, value, size);
	 end
      end
   end
   if (!found) begin
      log_.error ($psprintf ("Unable to read from address 0x%x", global_address));
   end
endtask // read

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task memory_impl::write (bit [63:0] global_address,  input bit [MAX_DATA - 1:0] value, int size);
`ifdef ncsim
   bit found;
   int found_index;
   found = 0;
   found_index = -1;
`else
   bit found = 0;
   int found_index = -1;
`endif
   for (int i = 0; i < memory_banks_.size (); ++i) begin
`ifdef ncsim
      integer offset;
      offset = memory_banks_[i].contains2 (global_address);
`else
      integer offset = memory_banks_[i].contains2 (global_address);
`endif
      if (offset != -1) begin
	 if (found) begin
	    log_.error ($psprintf ("Found two banks that contain address 0x%x. %s and %s",
			      global_address, memory_banks_[i].stats (), memory_banks_[found_index].stats ()));
	 end
	 else begin
	    found = 1;
	    found_index = i;
	    memory_banks_[i].to_memory (offset, value, size);
	 end
      end
   end
   if (!found) begin
      log_.error ($psprintf ("Unable to write to address 0x%x", global_address));
   end
endtask 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void memory_impl::add_memory_bank (/*owner*/ memory_bank bank);
   memory_banks_.push_back (bank);   
endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function /*cached*/ memory_bank memory_impl::lookup1 (bit [63:0] address_in_range);
`ifdef ncsim
   bit found;
   int found_index;
   memory_bank returned;

   found = 0;
   found_index = -1;
   returned = null;
`else
   bit found = 0;
   int found_index = -1;
   memory_bank returned = null;
`endif

   
   for (int i = 0; i < memory_banks_.size (); ++i) begin
`ifdef ncsim
      integer offset;
     offset = memory_banks_[i].contains2 (address_in_range);
`else
      integer offset = memory_banks_[i].contains2 (address_in_range);
`endif
      if (offset != -1) begin
	 if (found) begin
	    log_.error ($psprintf ("Found two banks that contain address 0x%x. %s and %s",
			      address_in_range, memory_banks_[i].stats (), memory_banks_[found_index].stats ()));
	 end
	 else begin
	    found = 1;
	    found_index = i;
	    returned = memory_banks_[i];
	 end
      end
   end
   if (!found) begin
      log_.error ($psprintf ("Unable to find memory bank which contains address 0x%x", address_in_range));
   end
   return returned;
endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function /*cached*/ memory_bank memory_impl::lookup2 (string partial_path);
`ifdef ncsim
   bit found;
   int found_index;
   memory_bank returned;

   found = 0;
   found_index = -1;
   returned = null;
`else
   bit found = 0;
   int found_index = -1;
   memory_bank returned = null;
`endif
   
   for (int i = 0; i < memory_banks_.size (); ++i) begin
      if (memory_banks_[i].contains1 (partial_path)) begin
	 if (found) begin
	    string msg;
	    log_.error ($psprintf ("Found two banks that contain partial path \"%s\". %s and %s",
			      partial_path, memory_banks_[i].stats (), memory_banks_[found_index].stats ()));
	 end
	 else begin
	    found = 1;
	    found_index = i;
	    returned = memory_banks_[i];
	 end
      end
   end
   if (!found) begin
      log_.error ($psprintf ("Unable to find memory bank which contains string \"%s\"", partial_path));
   end
   return returned;
endfunction
