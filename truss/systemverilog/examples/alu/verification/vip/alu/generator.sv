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
`include "generator.svh"

//mention three ways to constrain constraints, passed in config, dictionary, test empty constraint 
//pickup the dictionary constraints and stay within them.
class alu_generator_chooser;
   rand int operand_a;
   rand int operand_b;
   rand bit [7:0] op_code;
      

   local int min_operand_a;
   local int max_operand_a;
   
   local int min_operand_b;
   local int max_operand_b;

   local bit [7:0] min_op_code;
   local bit [7:0] max_op_code;

   constraint opearand_a_ {operand_a >= min_operand_a; operand_a <= max_operand_a;}
   constraint opearand_b_ {operand_b >= min_operand_b; operand_b <= max_operand_b;}
   constraint op_code_ {op_code >= min_op_code; op_code <= max_op_code;}								   
//mention public constraint test;


   function new (string name);
      min_operand_a = teal::dictionary_find_integer ({name, "_min_operand_a"}, 'h032862);
      max_operand_a = teal::dictionary_find_integer ({name, "_max_operand_a"}, 'h41062);
      min_operand_b = teal::dictionary_find_integer ({name, "_min_operand_b"}, 'h033099);
      max_operand_b = teal::dictionary_find_integer ({name, "_max_operand_b"}, 'h62896);
      min_op_code = teal::dictionary_find_integer ({name, "_min_op_code"}, 0);
      max_op_code = teal::dictionary_find_integer ({name, "_max_op_code"}, 3);
   endfunction // new
   
endclass




  task alu_generator::do_operations (int count);
     alu_operation operations [] = new [count];
     alu_generator_chooser chooser = new (log_.name());
     
     `truss_assert (count);
     for (int i = 0; i < count; ++i) begin
	alu_operation op;
	chooser.randomize ();
	op = new (chooser.operand_a, chooser.operand_b, chooser.op_code);
	operations[i] = op;
     end
     do_operations_ (operations);
endtask
