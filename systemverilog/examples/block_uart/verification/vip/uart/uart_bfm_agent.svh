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
`ifndef __uart_bfm_agent__
`define __uart_bfm_agent__

`include "teal.svh"
`include "truss.svh"

`include "uart_configuration.svh"
`include "uart_bfm.svh"


  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
class uart_block;
   uart_word words_[$];
   teal::uint32 block_delay_;
   teal::vout log_;

   function new (teal::uint32 d); block_delay_ = d; log_ = new ("uart_bfm_agent, line 44"); log_.show_debug_level (99); endfunction

   task add_word (uart_word w); words_.push_back (w); endtask

   function int max_offset (); `truss_assert (words_.size ()); return (words_.size () - 1);   endfunction
   function int size (); return (words_.size ());   endfunction

   function string sreport ();
      string msg;
      msg = $psprintf ("Block of %0d words. ", words_.size ());
      for (int i = 0; i < words_.size (); ++i) msg = {msg, " ", words_[i].sreport ()};
      return msg;
   endfunction
      
  endclass

`include "uart_channel.svh"

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //adds channel interface, interface is in blocks as opposed to words
class uart_bfm_agent extends uart_bfm;
    protected uart_channel to_be_transmitted_;
    protected uart_channel received_from_wire_;

    extern function new (string name, virtual uart_interface ui, uart_configuration c, teal::uint64 clock_frequency,
			 	 uart_channel to_be_transmitted,  uart_channel received_from_wire);

    extern virtual task start ();

    extern protected virtual task receive_completed_ (uart_word current_rx_word);

    extern local task do_tx_thread ();
endclass
`endif
