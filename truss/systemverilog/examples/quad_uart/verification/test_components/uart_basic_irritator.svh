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
`ifndef __uart_basic_irritator__
`define __uart_basic_irritator__

`include "uart_basic_test_component.svh"
`include "uart_checker.svh"

//See the basic exerciser for the control words.

class uart_basic_irritator extends truss::irritator;
  uart_basic_test_component basic_test_component_;

   function new (string n, uart_generator g, truss::verification_component b, uart_checker c);
      super.new (n);
      basic_test_component_ = new (n, g, b, c);
   endfunction
   

    virtual function void report (string prefix);
       basic_test_component_.report (prefix);
    endfunction
     
  virtual task time_zero_setup (); basic_test_component_.time_zero_setup ();endtask
  virtual task out_of_reset (truss::reset r); basic_test_component_.out_of_reset (r);endtask
  virtual task write_to_hardware (); basic_test_component_.write_to_hardware ();endtask
  virtual task wait_for_completion (); basic_test_component_.wait_for_completion ();endtask

  //could do a random delay, but how about just making sure there is a constant amount of data flowing
  //could also have an initial generate burst
  virtual protected task inter_generate_gap (); basic_test_component_.checker_.wait_actual_check ();endtask

  virtual function void randomize2 (); basic_test_component_.randomize2 ();endfunction
  virtual protected task wait_for_completion_ (); basic_test_component_.wait_for_completion_ ();endtask
  virtual protected task start_components_ (); basic_test_component_.start_components_ ();endtask
  virtual protected task generate2 (); basic_test_component_.generate2 ();endtask

 endclass
`endif
