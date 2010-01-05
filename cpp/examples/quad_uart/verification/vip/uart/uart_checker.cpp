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

#include "uart_checker.h"

using namespace truss;
using namespace teal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::checker::checker (const std::string& name) : thread (name), log_ (name), done_ (name),  word_count_ (0), done_flag_(false)
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::checker::start_ ()
{
  uart::block current_rx_block = uart::block (0);
  uart::block current_tx_block = uart::block (0);
  std::deque<uart::word>::iterator current_rx = current_rx_block.words_.end ();
  std::deque<uart::word>::iterator current_tx = current_tx_block.words_.end ();

  for (;;) {

    if (current_rx == current_rx_block.words_.end ()) {
      get_actual_ (&current_rx_block);
      current_rx = current_rx_block.words_.begin ();
    }
    if (current_tx == current_tx_block.words_.end ()) {
      get_expected_ (&current_tx_block);
      current_tx = current_tx_block.words_.begin ();
    }

    ++word_count_;

    if (*current_tx == *current_rx) {
      log_ << teal_info << "EXPECTED check of word  " << word_count_  << " "
	   <<  *current_tx  << " === "  << *current_rx  << teal::endm;
    }
    else {
      log_ << teal_error << " While checking word " << word_count_
	   << " on expected block " << expected_check_count_ << " word [" << (current_tx - current_tx_block.words_.begin ()) << "] "
	   <<  *current_tx   << " != " 
	   << "actual block " << actual_check_count_ << " word [" << (current_rx - current_rx_block.words_.begin ()) << "] "
	   << *current_rx  << teal::endm;
    }

    ++current_rx;
    ++current_tx;
    if (current_rx == current_rx_block.words_.end ()) { note_actual_check ();}
    if (current_tx == current_tx_block.words_.end ()) { note_expected_check ();}
    
    if ((current_tx == current_tx_block.words_.end ()) && (!more_ ())) {
      log_ << teal_info << " Signalling done. " << teal::endm;
      done_flag_ = true;
      done_.signal (); 
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::checker::report (const std::string prefix) const
{
  log_ << teal_info << prefix << " Number of words checked: " << word_count_ << endm;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::checker::wait_for_completion () 
{
  if (done_flag_) return;
  done_.wait ();
}
