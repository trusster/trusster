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
`ifndef __uart_configuration__
`define __uart_configuration__

`include "teal.svh" //for uint(s)


package uart;
parameter int max_uart_width = 32;

   typedef enum {ok = 0, parity_mismatch = 1, dsr_dtr_timeout = 2,
		  cts_timeout = 4, stop_bit_mismatch = 8, overrun_error = 16, framing_error = 32} status;

   typedef  enum {none=0, even, odd, mark, space} parity;
   typedef  enum { b_150 = 150, b_300 = 300, b_600 = 600, b_1200 = 1200,
		   b_2400 = 2400, b_4800 = 4800, b_9600 = 9600, 
		   b_19200 = 19200, b_38400 = 38400, 
		   b_57600 = 57600, b_115200 = 115200,
		   b_230400 = 230400, b_460800 = 460800, b_921600 = 921600
		   } baud_rate;

   typedef  enum {one=0, one_and_one_half, two} stop_bits;
   typedef  enum {data_terminal_equipment, data_communications_equipment} equipment;

   typedef  teal::uint32 data_size;
   typedef  bit [max_uart_width:0] data_type;
endpackage


  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
class uart_configuration;
    /*rand*/ uart::parity parity_;

    /*rand*/ uart::baud_rate baud_rate_;
    /*rand*/ teal::uint8 data_size_;
    /*rand*/ uart::stop_bits stop_bits_;

    /*rand*/ bit use_dtr_dsr_;
    /*rand*/ bit use_cts_rts_;
    teal::uint32 cts_timeout_; //in bit times before a time out event occurs
    teal::uint32 dtr_timeout_; //in bit times before a time out event occurs
    teal::uint32 dsr_timeout_; //in bit times before a time out event occurs

    uart::equipment equipment_;

    string name;
       
    protected teal::vout log_;

    extern function new  (string n);

    extern virtual function void randomize2 ();

    extern virtual function void report (string prefix);
    extern virtual function string sreport ();
  endclass

`endif
