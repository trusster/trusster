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

`include "uart_checker.svh"

import truss::*;
import teal::*;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function uart_checker::new (string name);
   super.new ();
   log_ = new (name);
   done_ = new (name, 1);
   word_count_ = 0;
endfunction 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_checker::start_ ();
  uart_block current_rx_block = new (0);
  uart_block current_tx_block = new (0);
   int current_rx_index = 0;
   int current_tx_index = 0;
   string msg;
   int foo;
   

  forever begin
    if (current_rx_index == current_rx_block.size ()) begin
      get_actual_ (current_rx_block);
       current_rx_index = 0;
    end
    if (current_tx_index == current_tx_block.size ()) begin
      get_expected_ (current_tx_block);
       current_tx_index = 0;
    end

    ++word_count_;

     msg = $psprintf (" Checking word %0d on actual block %0d actual: word[%0d] %s expected:  word %0d [%0d] %s", word_count_,
		     actual_check_count_, current_tx_index, current_tx_block.words_[current_tx_index].sreport(),
		     expected_check_count_, current_rx_index, current_rx_block.words_[current_rx_index].sreport ());
     		    
    if (current_tx_block.words_[current_tx_index].equal (current_rx_block.words_[current_rx_index])) begin
       log_.info ({"EXPECTED ", msg});
    end
    else begin
       log_.error (msg);
    end

    ++current_rx_index;
    ++current_tx_index;
    if (current_rx_index == current_rx_block.size ()) begin note_actual_check ();end
    if (current_tx_index == current_tx_block.size ()) begin note_expected_check ();end

     begin
	bit more; more_ (more);
	if ((current_tx_index == current_tx_block.size ()) && (!more)) begin done_.signal (); end
     end
     
  end
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void uart_checker::report (string prefix);
  string msg;
  msg = $psprintf ("%s Number of words checked: %0d", prefix, word_count_);
  log_.info (msg);
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_checker::wait_for_completion ();
  done_.pause ();
endtask
