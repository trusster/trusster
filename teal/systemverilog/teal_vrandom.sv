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
/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
from: http://www.ics.uci.edu/~eppstein/projects/pairs/Source/testbed/rand48

I've mixed classic erand48 with Bjarne Stoustrup's rand class (see sect 22.7, p 685, The C++ programming language)
CONSIDER: MD5 implementation
NOTE: some systems come with an erand48, but not all ;-)
 */


`define RAND48_SEED_0   63'h330e
`define RAND48_SEED_1   63'habcd
`define RAND48_SEED_2   63'h1234
`define RAND48_MULT_0   63'he66d
`define RAND48_MULT_1   63'hdeec
`define RAND48_MULT_2   63'h0005
`define RAND48_ADD      63'h000b

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function vrandom::new  ();
//constants in the cpp impl 
   _rand48_seed[0] = `RAND48_SEED_0;
   _rand48_seed[1] = `RAND48_SEED_1;
   _rand48_seed[2] = `RAND48_SEED_2;
   
   _rand48_mult[0] = `RAND48_MULT_0;
   _rand48_mult[1] = `RAND48_MULT_1;
   _rand48_mult[2] = `RAND48_MULT_2;
   _rand48_add = `RAND48_ADD;
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void vrandom::set  (string file, bit[31:0] line);
  seed__[0] = master_seed_[0];
  seed__[1] = master_seed_[1];
  seed__[2] = master_seed_[2];
  
  //first, fold in the string part
  for (int i = 0; i < file.len (); ++i) begin
    //CONSIDER: some ascii function like ! is_printable ()
//    if (! isalnum (file[i])) continue;  //skip the non-meat
    seed__[i % 3] = seed__[i % 3] ^ file.getc (i);
  end
  //now the integer part. Seperate close integers by mult with a big prime
  line *= 1103515245;
  seed__[0] = seed__[0] ^ (line & 'hff);
  seed__[1] = seed__[1] ^ ((line >> 8) & 'hff); 
  seed__[2] = seed__[2] ^ (line >> 16);

   `ifdef learn_how_to_get_thread_name
  //now the thread name so that same instances in multiple threads are unique
  string t = teal::thread_name (pthread_self ());
  for (int j= 0; j < t.len (); ++j) begin
    if (! isalnum (t[j])) continue;  //skip the non-meat
    seed__[j % 3] ^= t[j];
  end
   `endif
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit[47:0] vrandom::draw ();
   bit[63:0] accu;
   bit [15:0] temp[2];
   accu = _rand48_mult[0] * seed__[0] +  _rand48_add;
   temp[0] = accu[15:0];        /* lower 16 bits */
   accu >>= 16;
   accu += (_rand48_mult[0] * seed__[1]) +  (_rand48_mult[1] * seed__[0]);
   temp[1] = accu [15:0];        /* middle 16 bits */
   accu >>= 16;
   accu += (_rand48_mult[0] * seed__[2]) + (_rand48_mult[1] * seed__[1]) + 
	   (_rand48_mult[2] * seed__[0]);
   seed__[0] = temp[0];
   seed__[1] = temp[1];
   seed__[2] = accu[15:0];
  // $display ("%m draw returned %0d", {seed__[2], seed__[1], seed__[0]});
   return {seed__[2], seed__[1], seed__[0]};
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit[31:0] random_range::draw_val2 (bit[31:0] low, bit[31:0] up);
  if (low == up) return low;
begin
`ifdef ncsim
  bit[31:0] lower;
  bit[31:0] upper;
   bit [31:0] delta;
   bit[47:0] value;
   bit [31:0] returned;

   lower=  ((low < up) ? low : up);
   upper = ((low < up) ? up : low);
   delta = (up - low + 1);
   returned = (value % (delta)) + lower;
   value = draw ();
`else
  bit[31:0] lower=  ((low < up) ? low : up);
  bit[31:0] upper = ((low < up) ? up : low);
  bit[31:0] delta = (up - low + 1);
   bit [47:0] value = draw ();
   bit [31:0] returned = (value % (delta)) + lower;
`endif
// $display ("%m returning %0d", returned);
   return returned;
end
   
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function random_range::new (string file_name, bit[31:0] line);
   super.new (file_name, line);
endfunction




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task vrandom::init_with_file (string master_seed_path);
`ifdef ncsim
   integer    file_id;
   bit 	      found;
file_id = $fopen (master_seed_path, "r");
found = 0;
`else
      integer   file_id = $fopen (master_seed_path, "r");
   bit 	     found = 0;
`endif
   if (file_id) begin //cannot rely on short circuit &&
      while ((!found) && (! $feof(file_id))) begin
	 integer unused;
	 string dummy; 

	 unused = $fscanf (file_id, "%s", dummy);
	 if (dummy == "master_seed") begin
	    found = 1;
	    unused = $fscanf (file_id, "%d %d %d", master_seed_[0], master_seed_[1], master_seed_[2]);
//	    $display ("found master_seed: %0d %0d %0d", master_seed_[0], master_seed_[1], master_seed_[2]);	
	 end
      end
      $fclose (file_id);
   end // if (file_id)
   
   if (! found) begin //write it there, so next run will find it
      //     integer foo = $time();
      //     $ramdom (foo);
`ifdef ncsim
      integer foo;
      master_seed_[0] = $urandom (foo); master_seed_[1] = $urandom (foo); master_seed_[2] = $urandom (foo);
`else
      master_seed_[0] = $urandom (); master_seed_[1] = $urandom (); master_seed_[2] = $urandom ();
`endif      
      begin
`ifdef ncsim
	 integer file_id;
	 string msg;
	 integer dummy;
	 file_id = $fopen (master_seed_path, "w+");
	 dummy = $ferror (file_id, msg);
`else
	 integer file_id = $fopen (master_seed_path, "w+");
	 string msg;
	 integer dummy = $ferror (file_id, msg);
`endif
	 if (! file_id) begin
	    $display ("%t %m %s", $time, msg);
	    $finish (04101962);
	 end
	 
	 $fwrite (file_id, "%0d %0d %0d", master_seed_[0], master_seed_[1], master_seed_[2]);
	 $fclose (file_id);
      end
   end
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task vrandom::init_with_seed (bit[63:0] m);
  m *=  1103515245; //generate big difference from close to same seed
  master_seed_[0] = m;
  master_seed_[1] = (m >> 8); 
  master_seed_[2] = (m >> 16);

  master_seed_[0] ^= (m >> 24);
  master_seed_[1] ^= (m >> 32); 
  master_seed_[2] ^= (m >> 40);

  master_seed_[0] ^= (m >> 48);
  master_seed_[1] ^= (m >> 56); 
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function random_range_8::new  (string file_name, bit[31:0] line);
   super.new ();
   set (file_name, line);
endfunction // random_range_8


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit[7:0] random_range_8::draw_val ();
   bit [47:0] value;
   bit [7:0]  returned;
   value = draw ();
`ifdef kjljjjkljkljl
   returned[0] = ^{value[0], value[8],  value[16], value[24], value[32], value[40]};
   returned[1] = ^{value[1], value[9],  value[17], value[25], value[33], value[41]};
   returned[2] = ^{value[2], value[10], value[18], value[26], value[34], value[42]};
   returned[3] = ^{value[3], value[11], value[19], value[27], value[35], value[43]};
   returned[4] = ^{value[4], value[12], value[20], value[28], value[36], value[44]};
   returned[5] = ^{value[5], value[13], value[21], value[29], value[37], value[45]};
   returned[6] = ^{value[6], value[14], value[22], value[30], value[38], value[46]};
   returned[7] = ^{value[7], value[15], value[23], value[31], value[39], value[47]};
`else
   returned = value;
`endif // !`ifdef kjljjjkljkljl
   
   return returned;
endfunction // bit



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function random_range_32::new  (string file_name, bit[31:0] line);
   super.new ();
   set (file_name, line);
endfunction // random_range_8


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit[31:0] random_range_32::draw_val ();
   bit [47:0] value;
   bit [31:0] returned;
   
   value = draw ();
`ifdef kl
   
//now distribute the 48-32 16 bits
   returned[0] ^= value[32];
   returned[2] ^= value[33];
   returned[4] ^= value[34];
   returned[6] ^= value[35];
   returned[8] ^= value[36];
   returned[10] ^= value[37];
   returned[12] ^= value[38];
   returned[14] ^= value[39];
   returned[16] ^= value[40];
   returned[18] ^= value[41];
   returned[20] ^= value[42];
   returned[22] ^= value[43];
   returned[24] ^= value[44];
   returned[26] ^= value[45];
   returned[28] ^= value[46];
   returned[30] ^= value[47];
`else // !`ifdef kl
   returned = value;
`endif
   return returned;
endfunction // bit

