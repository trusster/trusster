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

#include "teal.h"
using namespace teal;

namespace {
  uint8 get_bit_delay (uint8 min_v, uint8 max_v) {
    uint8 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }

  uint8 get_block_size (uint8 min_v, uint8 max_v) {
    uint8 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }

};


#include "uart_basic_test_component.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::basic_test_component::randomize ()
{
  log_ << teal_debug << " Randomize: begin" << teal::endm;
  static uint8 min_words = dictionary::find (name + "_min_num_words", 2);
  static uint8 max_words = dictionary::find (name + "_max_num_words", 4);
  static uint8 min_bit_delay = dictionary::find (name + "_min_block_delay", 0);
  static uint8 max_bit_delay = dictionary::find (name + "_max_block_delay", 10);
  block_size_ = get_block_size (min_words, max_words);
  block_delay_ = get_bit_delay (min_bit_delay, max_bit_delay);
  log_ << teal_debug << " Randomize done. block_size:" << block_size_ << " block_delay: " << block_delay_ << teal::endm;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::basic_test_component::start_components_ () 
{
  log_ << teal_debug << "Staring components." << teal::endm;
  bfm_->start (); 
  checker_->start ();
}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::basic_test_component::generate () 
{
  generator_->send_block (block_size_, block_delay_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::basic_test_component::wait_for_completion_ () 
{
  log_ << teal_debug << "wait_for_completion() begin" << teal::endm;
  checker_->wait_for_completion ();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::basic_test_component::basic_test_component (const std::string& n, generator* g, verification_component* b, checker* c) : 
  thread (n), test_component (n), 
  generator_ (g), bfm_ (b), checker_ (c), block_size_ (0), block_delay_ (0) 
{
  truss_assert (g);
  truss_assert (b);
  truss_assert (c);
  //add test_component defaults
  teal::dictionary::put (generator_->name + "_min_word_delay", "0", teal::dictionary::replace_entry);
  teal::dictionary::put (generator_->name + "_max_word_delay", "3", teal::dictionary::replace_entry);
}

