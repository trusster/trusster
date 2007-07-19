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
`include "uart_basic_test_component.svh"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function uart_basic_test_component::new (string n, uart_generator g, truss::verification_component b, uart_checker c);
   super.new (n);
   generator_ = g;
   bfm_ = b;
   checker_ = c;

  `truss_assert (g != null);
  `truss_assert (b != null);
  `truss_assert (c != null);
  //add test_component defaults
  teal::dictionary_put ({generator_.name, "_min_word_delay"}, "0", teal::replace_entry);
  teal::dictionary_put ({generator_.name, "_max_word_delay"}, "3", teal::replace_entry);
//   log_.show_debug_level (99);
endfunction




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void uart_basic_test_component::randomize2 ();
   string msg;
   int foo;
   min_word_count_ = teal::dictionary_find_integer ({log_.name (), "_min_num_words"}, 2);
   max_word_count_  = dictionary_find_integer ({log_.name (), "_max_num_words"}, 4);
   min_block_delay_ = dictionary_find_integer ({log_.name (), "_min_block_delay"}, 0);
   max_block_delay_ = dictionary_find_integer ({log_.name (), "_max_block_delay"}, 10);
   
   log_.debug (" Randomize: begin");
   `truss_assert (randomize ());
   msg = $psprintf (" Randomize done. block_size: %0d block_delay: %0d", word_count_, block_delay_);
   log_.debug (msg);
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_basic_test_component::start_components_ ();
  log_.debug ("Starting components.");
  bfm_.start (); 
  checker_.start ();
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_basic_test_component::generate2 ();
  generator_.send_block (word_count_, block_delay_);
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_basic_test_component::wait_for_completion_ ();
  log_.debug ("wait_for_completion() begin");
  checker_.wait_for_completion ();
endtask


