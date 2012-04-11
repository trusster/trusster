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

#include "uart_bfm_agent.h"

using namespace uart;
using namespace teal;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::bfm_agent::bfm_agent (const std::string& name,       truss::port <configuration::signals>::pins port,
		const uart::configuration* c,  to_be_transmitted* to_be_transmitted,
		received_from_wire* received_from_wire,      uint64 clock_frequency) :
  bfm (name, port, c, clock_frequency), /*truss::multi_thread (name),*/ to_be_transmitted_ (to_be_transmitted), received_from_wire_ (received_from_wire)
{}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm_agent::receive_completed_ (const word& current_rx_word) { 
  log_ << teal_info << " received word: " << current_rx_word << teal::endm;
  block current_rx = block (0);
  current_rx.add_word (current_rx_word);
  received_from_wire_->put (current_rx);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm_agent::do_tx_thread ()
{
  for (;;) {
        log_ << teal_debug << " about to get!" << teal::endm;
    block current_tx = to_be_transmitted_->get ();
        log_ << teal_debug << " Block of " << current_tx.words_.size () << " words to be transmitted!" << teal::endm;
    if (current_tx.block_delay_) { pause_ (one_bit_ * current_tx.block_delay_);  }
    for (std::deque<word>::iterator it (current_tx.words_.begin ()); (it != current_tx.words_.end ()); ++it) {
      send_word (*it);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm_agent::start () 
{
  run (&uart::bfm_agent::do_tx_thread, "do_tx_thread");
  uart::bfm::start ();
  //  run <uart::bfm_agent> (&uart::bfm_agent::do_tx_thread);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm_agent::stop () 
{
  uart::bfm::stop ();
  multi_thread::stop ("do_tx_thread");
}

  
