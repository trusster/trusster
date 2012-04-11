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
`ifndef __uart_bfm__
`define __uart_bfm__

`include "teal.svh"
`include "truss.svh"

`include "uart_configuration.svh"

`include "interfaces_uart.svh"



  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  //character gap, match char, force xon/off tx, remove xon/off in higher level channel
class uart_word;
    teal::uint32 status_;  //if non-zero, data may be invalid

    uart::data_type data_min_;
    uart::data_type data_max_;
    rand uart::data_type data;
    constraint data_valid { data >= data_min_; data <= data_max_;}

    local teal::uint8 bit_start_delay_min_;
    local teal::uint8 bit_start_delay_max_;
    rand teal::uint8 bit_start_delay; //how many bit times to delay
   constraint bit_start_delay_valid { bit_start_delay >= bit_start_delay_min_; bit_start_delay <= bit_start_delay_max_;}

    teal::uint8 data_size_;

    extern function new (teal::uint8 data_size, teal::uint8 bit_delay);

    extern function bit equal (uart_word w);
    extern function string sreport ();
	  
      
    extern virtual function void randomize2 (teal::uint8 min_delay, teal::uint8 max_delay, 
				    uart::data_type min_data, uart::data_type max_data);

  endclass


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class uart_bfm extends truss::verification_component;
    //valid after start
    protected teal::uint32 one_bit_; 
    protected teal::uint32 one_half_bit_; 
    protected teal::uint64 clock_frequency_;

    extern function new (string name, virtual uart_interface ui, uart_configuration c, teal::uint64 clock_frequency);
  
    virtual task time_zero_setup ()     ; endtask
    virtual task out_of_reset (truss::reset r)   ; endtask
    virtual function void randomize2 ()           ; endfunction

    extern virtual task write_to_hardware ();
    virtual task wait_for_completion () ; endtask
    virtual function void report (string prefix) ; endfunction

    extern virtual task send_word (uart_word current_tx);

    //These set the DUT wires accordingly, where true maps to active
    extern virtual task dtr (bit new_value);
    extern virtual task dsr (bit new_value);

    extern virtual task cts (bit new_value);
    extern virtual task rts (bit new_value);

    extern virtual task start ();

    `PURE protected virtual task receive_completed_ (uart_word current_rx_word);

    protected uart_configuration configuration_;
      
    protected virtual uart_interface port_;
      
    extern protected virtual task pause_ (teal::uint32 count);

    extern protected task do_rx_thread ();

    //local utility functions
    extern local function bit generate_parity_ (uart::parity p, uart::data_type data);
    extern function bit check_parity_ (uart::parity p, uart::data_type data, uart::data_type parity_received) ;
 endclass // bfm
`endif
