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

`include "uart_configuration.svh"
//I am lazy
import uart::*;
   
`include "truss.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//any way to make this a local scoped class?
class __uart_utility;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function string sreport_parity (parity p) ;
     case (p) 
        none : return "none"; 
       even : return "even"; 
     odd : return "odd"; 
     mark : return "mark";
     space : return "space";
  default: return "unknown";
     endcase // case(p)
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function string sreport_stop_bits (stop_bits s);
  case (s) 
     one : return "1"; 
     one_and_one_half : return "1.5"; 
     two : return "2"; 
    default: return "unknown";
  endcase
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function string sreport_integer (integer d);
      string msg;
      msg = $psprintf ("%0d", d);
      return msg;
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function string sreport_baud_rate (uart::baud_rate b);
  case (b) 
     b_150 : return "150"; 
     b_300 : return "300"; 
     b_600 : return "600"; 
     b_1200 : return "1200"; 
     b_2400 : return "2400"; 
     b_4800 : return "4800"; 
     b_9600 : return "9600"; 
     b_19200 : return "19200"; 
     b_38400 : return "38400"; 
     b_57600 : return "57600"; 
     b_115200 : return "115200"; 
     b_230400 : return "230400"; 
     b_460800 : return "460800"; 
     b_921600 : return "921600"; 
  default: return "unknown";
  endcase
   endfunction // string

endclass // __uart_utility


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class __uart_configuration_chooser;
   rand stop_bits stop_bits_;
   local stop_bits min_stop_bits;
   local stop_bits max_stop_bits;
   constraint valid_stop_bits {stop_bits_ >= min_stop_bits; stop_bits_ <= max_stop_bits;}
   
   rand baud_rate baud_rate_;
   local baud_rate min_baud_rate;
   local baud_rate max_baud_rate;
   constraint valid_baud_rate {baud_rate_ >= min_baud_rate; baud_rate_ <= max_baud_rate;}
   
   
   rand parity parity_;
   local parity min_parity;
   local parity max_parity;
   constraint valid_parity {parity_ >= min_parity; parity_ <= max_parity;}  
   
   rand data_size data_size_;
   local data_size min_data_size;
   local data_size max_data_size;
   constraint valid_data_size {data_size_ >= min_data_size; data_size_ <= max_data_size;}

   bit use_dtr_dsr_;
   local teal::uint8 use_dtr_dsr_probability;
   local rand teal::uint8 use_dtr_dsr_probability_now;
   constraint use_dtr_dsr_probability_valid {use_dtr_dsr_probability_now >= 0; use_dtr_dsr_probability_now <= 100;}

   bit use_cts_rts_;
   local teal::uint8 use_cts_rts_probability;
   local rand teal::uint8 use_cts_rts_probability_now;
   constraint use_cts_rts_probability_valid {use_cts_rts_probability_now >= 0; use_cts_rts_probability_now <= 100;}
   
   local teal::vout log_;

   function new (string n);
      log_ = new (n);
      
      min_baud_rate = baud_rate' (teal::dictionary_find_integer ({n, "_min_baud"}, 150));
      max_baud_rate = baud_rate' (teal::dictionary_find_integer ({n, "_max_baud"}, 921600));
      min_parity = parity' (teal::dictionary_find_integer ({n, "_min_parity"}, 0));
      max_parity = parity' (teal::dictionary_find_integer ({n, "_max_parity"}, 4));
      min_data_size = data_size' (teal::dictionary_find_integer ({n, "_min_data_size"}, 2));
      max_data_size = data_size' (teal::dictionary_find_integer ({n, "_max_data_size"}, 4));
      min_stop_bits = stop_bits' (teal::dictionary_find_integer ({n, "_min_stop_bits"}, 0));
      max_stop_bits = stop_bits' (teal::dictionary_find_integer ({n, "_max_stop_bits"}, 2));
      use_dtr_dsr_probability = teal::dictionary_find_integer ({n, "use_dtr_dsr_probability"}, 0);
      use_cts_rts_probability = teal::dictionary_find_integer ({n, "use_cts_rts_probability"}, 0);
   endfunction // new

   function string sreport ();
      string msg1;
      string msg2;
      string msg3;
      string msg4;
      string msg5;
      string msg6;
      int foo;
      msg1 = $psprintf ("Generate baud from [%0d ... %0d]", min_baud_rate,  max_baud_rate);
      msg2 = $psprintf ("Generate parity from [%0d ... %0d]", min_parity,  max_parity);
      msg3 = $psprintf ("Generate data_size from [%0d ... %0d]", min_data_size,  max_data_size);
      msg4 = $psprintf ("Generate stop_bits from [%0d ... %0d]", min_stop_bits,  max_stop_bits);
      msg5 = $psprintf ("Generate use_dtr_dsr_probability is %0d", use_dtr_dsr_probability);
      msg6 = $psprintf ("Generate use_cts_rts_probability is %0d", use_cts_rts_probability);
      return {msg1,msg2, msg3, msg4, msg5, msg6};
   endfunction // string
   
   function void post_randomize ();
      use_dtr_dsr_ = (use_dtr_dsr_probability_now < use_dtr_dsr_probability);
      use_cts_rts_ = (use_cts_rts_probability_now < use_cts_rts_probability);
      log_.info ({"post_randomize: ", sreport ()});
   endfunction // void
   
endclass // uart_configuration_chooser


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function  uart_configuration::new (string n);
  parity_ = none; 
   baud_rate_ = b_921600; 
   data_size_ = 8; 
   stop_bits_ = one;
   use_dtr_dsr_ = 0; 
   use_cts_rts_ = 0;
   log_ = new (n);
   name = n;
endfunction // new

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void uart_configuration::randomize2 ();
   __uart_configuration_chooser chooser = new (log_.name ());
   `truss_assert (chooser.randomize ());
   baud_rate_ = chooser.baud_rate_;
   parity_ = chooser.parity_;
   data_size_ = chooser.data_size_;
   stop_bits_ = chooser.stop_bits_;
   use_dtr_dsr_ = chooser.use_dtr_dsr_;
   use_cts_rts_ = chooser.use_cts_rts_;
   report ("After Randomize:");
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void uart_configuration::report (string prefix);
   log_.info ({prefix, sreport ()});
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string uart_configuration::sreport ();
   __uart_utility uu = new ();
  return  {" parity: ",    uu.sreport_parity ( parity_),
	   " baud_rate ",  uu.sreport_baud_rate ( baud_rate_),
	   " data_size_  ", uu.sreport_integer (data_size_),
	   " stop_bits ",  uu.sreport_stop_bits (stop_bits_),
	   " use_dtr_dsr: ", uu.sreport_integer (use_dtr_dsr_),
	   " use cts_rts: ", uu.sreport_integer (use_cts_rts_)};
endfunction
