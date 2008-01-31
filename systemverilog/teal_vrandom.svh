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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class vrandom;
   extern function new  ();  //no overloaded new, no static methods, so two phase it is!
      
    extern function void set (string file, bit[31:0] line);
    extern function bit[47:0] draw ();

   extern task init_with_file (string master_seed_path);
   extern task init_with_seed (bit [63:0] m);
       
       local bit[15:0] _rand48_seed[3];  //keep variable names consistent with the original
      local bit[15:0] _rand48_mult[3];
      local bit[63:0] _rand48_add;
    local bit[15:0] seed__[3];
     
      
      
    static bit[15:0] master_seed_[3];
   endclass // vrandom


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task vrandom_init_with_file (string master_seed_path);
`ifdef ncsim
      vrandom hack;
      hack = new ();
`else
      vrandom hack = new ();
`endif
      hack.init_with_file (master_seed_path);
   endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task vrandom_init_with_seed (bit [63:0]  master_seed);
`ifdef ncsim
      vrandom hack;
      hack = new ();
`else
      vrandom hack = new ();
`endif
      hack.init_with_seed (master_seed);
   endtask // init_with_seed



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //NOTE: The min,max values are not put in the ctor to allow different ranges after the 
  //object is created. This is important to allow one range,random to affect another.
class random_range_8 extends  vrandom;
   extern function new  (string file_name, bit[31:0] line);
   extern function bit[7:0] draw_val ();
endclass // random_range

class random_range_32 extends  vrandom;
   extern function new  (string file_name, bit[31:0] line);
   extern function bit[31:0] draw_val ();
endclass // random_range

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //NOTE: The min,max values are not put in the ctor to allow different ranges after the 
  //object is created. This is important to allow one range,random to affect another.
class random_range extends  random_range_32;
   extern function new  (string file_name, bit[31:0] line);
   extern function bit[31:0]draw_val2 (bit[31:0] low, bit[31:0] up); //any order
endclass 
     

`ifdef ncsim
`define RAND_8(x) begin teal::random_range_8 r; r = new ("x",11); x = r.draw_val (); end
`define RAND_32(x) begin teal::random_range_32 r; r = new ("x", 3); x = r.draw_val (); end
`define RAND_RANGE(x,y,z) begin teal::random_range r; r = new ({"x", "y", "z"}, 11); x = r.draw_val2 (y,z); end
`else
`define RAND_8(x) begin static teal::random_range_8 r = new ("x",11); x = r.draw_val (); end
`define RAND_32(x) begin static teal::random_range_32 r = new ("x", 3); x = r.draw_val (); end
`define RAND_RANGE(x,y,z) begin static teal::random_range r = new ({"x", "y", "z"}, 11); x = r.draw_val2 (y,z); end
`endif
`include "teal_vrandom.sv"
