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
`ifndef __uart_basic_test_component__
`define __uart_basic_test_component__

`include "uart_bfm.svh"
`include "uart_generator.svh"
`include "uart_checker.svh"

`include "truss.svh"

//simple fixed form, run n bytes through a uart
//The following teal::dictionary control knows are used. All the prefixed with the name of the generator.
// _min_num_words, _max_num_words  
//_min_block_delay, _max_block_delay

//see uart::basic_irretator for an infinate run irretator form

class uart_basic_test_component extends truss::test_component;
    protected truss::verification_component       bfm_; 

   extern function new (string n, uart_generator g, truss::verification_component b, uart_checker c);

    extern virtual function void randomize2 ();

    virtual task  time_zero_setup (); bfm_.time_zero_setup (); endtask

    virtual task  out_of_reset (truss::reset r); bfm_.out_of_reset (r); endtask

    virtual task  write_to_hardware (); bfm_.write_to_hardware (); endtask

    extern virtual protected task  generate2 ();
    extern virtual protected task  wait_for_completion_ ();
    extern virtual protected task  start_components_ ();

    protected uart_generator                      generator_;
     uart_checker                        checker_;


    rand protected teal::uint8 word_count_;
    local teal::uint8 min_word_count_;
    local teal::uint8 max_word_count_;
    constraint word_count_valid {word_count_ >= min_word_count_; word_count_ <= max_word_count_;}

    rand protected teal::uint8 block_delay_;
    local teal::uint8 min_block_delay_;
    local teal::uint8 max_block_delay_;
    constraint block_delay_valid {block_delay_ >= min_block_delay_; block_delay_ <= max_block_delay_;}
  endclass
`endif
