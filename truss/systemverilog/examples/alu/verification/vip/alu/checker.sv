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

`include "checker.svh"

//`include "alu_model.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function alu_checker::new (string name, alu_channel g, truss::int_channel a);
   generated_ = g;
   actual_ = a;
   completed_ = 0;
   completed_flag_ = new (name, 1);
   log_ = new (name);
   log_.show_debug_level (4);
endfunction 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task alu_checker::start_ ();
   forever begin
      alu_operation generated;
      int actual;

      log_.debug (" wait for an generated ");
      generated_.get (generated);
      log_.debug ({" Got generated value of ", generated.sreport (), " Now wait for actual."});
      actual_.get (actual);
      begin
	 string msg;
	 msg = $psprintf ( " Got actual value of %0d", actual);
	 log_.debug (msg);
      end
      

      if (alu_model (generated.operand_a, generated.operand_b, generated.op_code) == actual)  begin
	 string msg;
	 msg = $psprintf ( " Expected sent %s == %0d", generated.sreport (), actual);
	 log_.info (msg);
      end
      else begin
	 string msg;
	 msg = $psprintf ( " Sent %s != %0d", generated.sreport (), actual);
	 log_.error (msg);
      end

      begin
	 int count_; generated_.count(count_);
	 if (!count_) begin
	    completed_flag_.signal ();
	    return;
	 end
      end
   end // for (;;)
endtask // alu_checker



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task alu_checker::wait_for_completion ();
  completed_flag_.pause ();
  completed_ = 1;
endtask



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void alu_checker::report (string prefix);
  if (completed_) begin
     log_.info ("Completed.");
  end else begin
    log_.error ("Did not complete.");
  end
endfunction


//move to dpi!!!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function int alu_checker::alu_model (int opa, int opb, int op);       
  case (op)
    0: return opa + opb;
    1: return opa - opb;
    2: return opa & opb;
    3: return opa | opb;
    default: return 0;
  endcase
  return -1;
endfunction
