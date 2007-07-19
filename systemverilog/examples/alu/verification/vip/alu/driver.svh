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
`ifndef __alu_driver__
`define __alu_driver__

`include "truss.svh"


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class alu_operation;
    bit [31:0] operand_a;
    bit [31:0] operand_b;
    bit [7:0] op_code;

   function new (bit [31:0] a, bit [31:0] b, bit [7:0] code);
      operand_a = a;
      operand_b = b;
      op_code = code;
   endfunction
   
  function string sreport ();
     string msg;
`ifndef MTI
    unique 
`endif
   case (op_code) 
	     0: begin msg = $psprintf ("%0d + %0d", operand_a, operand_b); end
	     1: begin msg = $psprintf ("%0d - %0d", operand_a, operand_b); end
	     2: begin msg = $psprintf ("%0d & %0d", operand_a, operand_b); end
	     3: begin msg = $psprintf ("%0d | %0d", operand_a, operand_b); end
	   endcase
     return msg;
  endfunction 
endclass 

    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class alu_driver extends truss::verification_component;
    local virtual alu_input alu_input_;

    extern function new (string name, virtual alu_input a);

    extern virtual task time_zero_setup ();
    extern virtual task out_of_reset (truss::reset r);
    extern virtual function void randomize2 ();
    extern virtual task start ();
    extern virtual task stop ();
    extern virtual task write_to_hardware ();
    extern virtual task wait_for_completion ();
    extern virtual function void report (string prefix) ;

    extern task send_operation (alu_operation an_operation);

endclass

`endif
