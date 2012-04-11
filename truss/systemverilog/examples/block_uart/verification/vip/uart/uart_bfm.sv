/*
 Trusster Open Source License version 1.0a (TRUST)
 copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved. 

 Redistribution and use in source and binary forms, with or without modification, 
 are permitted provided that the following conditions are met: 
 
 Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, 
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.
 Redistributions in any form must be accompanied by information on how to obtain 
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
`include "truss.svh"


`include "uart_bfm.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Word
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function uart_word::new (teal::uint8 data_size, teal::uint8 bit_delay);
   status_ = 0;
   bit_start_delay = bit_delay;
   data_size_ = data_size;
   //clean up above the data_size bit
   for (int i = data_size_; i < uart::max_uart_width; ++i) begin
      data[i] = 0;
   end
   
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ignore delay, since its a tx thing (although one could concievably measure and check this)
function bit uart_word::equal (uart_word  w);
   for (int i = 0; i < data_size_; ++i) begin
      if (data[i] != w.data[i]) return 0;
      end
   return (status_ == w.status_);
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function string uart_word::sreport ();
   string msg;
   msg = $psprintf ("delay of %0d status: %0d 0x%x", bit_start_delay, status_, data);
   return msg;
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void uart_word::randomize2 (teal::uint8 min_delay, teal::uint8 max_delay, 
			    uart::data_type min_data, uart::data_type max_data);
   teal::vout log_ = new ("uart_word:: randomize ()");
   data_min_ = min_data;
   data_max_ = max_data;
   bit_start_delay_min_ = min_delay;
   bit_start_delay_max_ = max_delay;
   `truss_assert (randomize ());
   //clean up above the data_size bit
   for (int i = data_size_; i < uart::max_uart_width; ++i) begin
      data[i] = 0;
   end
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physical layer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function uart_bfm::new (string name, virtual uart_interface ui, uart_configuration c, teal::uint64 clock_frequency);
   super.new (name);
   configuration_ = c;
   port_ = ui;
   clock_frequency_ = clock_frequency;
//   log_.show_debug_level (4); 
endfunction



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit uart_bfm::generate_parity_ (uart::parity p, uart::data_type data);
   case (p)
     uart::even:  begin return  ^data;  end
     uart::odd :  begin return !(^data);  end
     uart::mark:  begin return 1;  end
     uart::space: begin return 0;  end
     uart::none:  begin teal::vout log_ = new ("generate_parity"); `truss_assert (0);end
   endcase // case(p)
   return 1;
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function bit uart_bfm::check_parity_ (uart::parity p, uart::data_type data, uart::data_type parity_received) ;
   return generate_parity_ (p, data) == parity_received;
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::do_rx_thread ();
   teal::uint32 word_count = 0;
   uart_word current_rx = new (configuration_.data_size_, 0);
   bit current_bit;
   

   forever begin
      //dtr/dsr on word boundry only
      if (configuration_.equipment_ == uart::data_communications_equipment) begin
	 while ( (configuration_.use_dtr_dsr_) && (port_.dsr == 0)) begin
	    pause_ (1);
	 end
      end
      else begin
	 while ( (configuration_.use_dtr_dsr_) && (port_.dtr == 0)) begin
	    pause_ (1);
	 end
      end
      
`define slimy_way 1     
`ifdef slimy_way
      current_bit = port_.rx;
      while (current_bit != 0) begin
	 while (port_.rx == 1) begin
	    pause_ (1);
	 end
	 log_.debug_n (" possible start bit (negedge found)", 3);
`else
	 //now wait for start bit..defined as a high (1) to low = 0 transition (at least one bit wide)
	 //but must assume that we are putting out the last stop bit (a low)
	 //so, ...find a 1
	 current_bit = port_.rx;
	 while (current_bit == 0) begin
	    pause_ (1);
	    current_bit = port_.rx;
	 end
	 //now find the start of the 0.. and make sure it's there  1/2 a bit time later
	 //ASSUMES that there is a pullup on the rx line
	 //mfm 9/17/05 why not just use at (neg_edge (port_.rx)) ?
	 //mfm Jan 8,2006 - because (1) it may already be 0 and (2) should emulate sampling
	 while (current_bit != 0) begin
	    while (port_.rx == 1) begin
	       pause_ (1);
	    end
	    log_.debug_n (" possible start bit (negedge found)", 3);
`endif
	    //assume that this is the real beginning of the start bit
	    //delay and get out of the outer loop is its still zero
	    pause_ (one_half_bit_);
	    current_bit = port_.rx;
	    if (current_bit == 0) log_.debug (" found start bit (still 0 at 1/2 bit time)");
	 end

	 
	 //get the data
	 for (teal::uint8 i = 0; i < configuration_.data_size_; ++i) begin
	    pause_ (one_bit_); 
	    current_rx.data [i] = port_.rx;
	    begin
	       string msg;
	       msg = $psprintf ("receive bit %0d is %0d", i, port_.rx);
	       log_.debug (msg);
	    end
	 end

	 //now the parity
	 if (configuration_.parity_ != uart::none) begin
	    bit parity_received;
	    
	    pause_ (one_bit_); 
	    parity_received = port_.rx; 
	    if (! check_parity_ (configuration_.parity_, current_rx.data, parity_received)) begin
	       current_rx.status_ |= uart::parity_mismatch;
	    end
	 end

	 //how/should we verify stop bits?
	 case (configuration_.stop_bits_) 
	   uart::one: pause_ (one_bit_); 
	   uart::one_and_one_half: pause_ (one_bit_ + one_half_bit_); 
	   uart::two: pause_ (one_bit_ << 1); 
	   default: `truss_assert (0);
	 endcase 
	 
	 //check that it is still a stop bit
	 if (port_.rx != 1) begin
	    current_rx.status_ |= uart::stop_bit_mismatch;	
	 end 

	 pause_ (one_half_bit_ -1); //complete the stop bit
	 begin
	    string msg;
	    msg = $psprintf ("done receiving stop bit %0d", port_.rx);
	    log_.debug (msg);
	 end
	 log_.debug ({"Completed receive: " , current_rx.sreport ()});

	 receive_completed_ (current_rx);
	 word_count++;
      end
   endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::send_word (uart_word current_tx);
   log_.debug ({" Word to be transmitted: ", current_tx.sreport()});

   if (current_tx.bit_start_delay) begin
      pause_ (one_bit_ * current_tx.bit_start_delay);
   end
   
   //now dsr/dtr
   //dtr/dsr on word boundry only
   if (configuration_.equipment_ == uart::data_communications_equipment) begin
      while ( (configuration_.use_dtr_dsr_) && (port_.dsr == 0)) begin
	 pause_ (1);
      end
   end
   else begin
      while ( (configuration_.use_dtr_dsr_) && (port_.dtr == 0)) begin
	 pause_ (1);
      end
   end

   //now cts/rts
   //who sets rts?
   begin
      teal::uint32 cts_time_out = 0;
      while ( (configuration_.use_cts_rts_) && (port_.cts == 0)) begin
	 pause_ (1);
	 if (cts_time_out++ >= configuration_.cts_timeout_) begin
	    //way to tell the outside world is to create a receive status word
	    uart_word error_rx = new (configuration_.data_size_, 0);
	    error_rx.status_ |= uart::cts_timeout;
	    receive_completed_ (error_rx);
	    cts_time_out = 0;
	 end
      end
   end

   //send start bit
   log_.debug ("sending start bit.");
   port_.tx <= 0; //active state?
   pause_ (one_bit_);
   log_.debug ("done sending start bit");

   //send data , should add a MSB first feature
   for (teal::uint32 i = 0; i < configuration_.data_size_; ++i) begin
      port_.tx <= current_tx.data[i];
      begin
	 string msg;
	 msg = $psprintf ("sending bit %0d %0d", i, current_tx.data[i]);
	 log_.debug (msg);
      end
      pause_ (one_bit_);
   end
   
   //now the parity
   if (configuration_.parity_ != uart::none) begin
      log_.debug ("sending parity.");
      port_.tx <= generate_parity_ (configuration_.parity_, current_tx.data);
      pause_ (one_bit_); 
   end

   //stop_bits
   log_.debug ("sending stop bit.");
   port_.tx <= 1;
   case (configuration_.stop_bits_) 
     uart::one: pause_ (one_bit_); 
     uart::one_and_one_half: pause_ (one_bit_ + one_half_bit_); 
     uart::two: pause_ (one_bit_ << 1); 
     default: `truss_assert (0);
   endcase
   log_.debug ("done sending stop bit.");
endtask 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::pause_ (teal::uint32 count);
   for (teal::uint32 i = count; i; --i) begin
      @ (posedge (port_.baud_rate_clock));
   end
endtask

//The wire values asserted for the control signals
`define uart_active 1
`define uart_inactive 0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::dtr (bit new_value);  port_.dtr = new_value ? `uart_active : `uart_inactive; endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::dsr (bit new_value);  port_.dsr = new_value ? `uart_active : `uart_inactive;endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::cts (bit new_value);  port_.cts = new_value ? `uart_active : `uart_inactive;endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::rts (bit new_value);  port_.rts = new_value ? `uart_active : `uart_inactive; endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::start () ;
   one_bit_ = (clock_frequency_ + configuration_.baud_rate_ - 1) / configuration_.baud_rate_;
   begin
      string msg;
      msg = $psprintf ("one bit is %0d", one_bit_);
      log_.debug (msg);
   end
   one_half_bit_ = one_bit_ / 2; //otherwise there is not enough clock resolution
   `truss_assert (one_half_bit_);
   
   fork do_rx_thread (); join_none
endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_bfm::write_to_hardware ();
   //should't it look at communications equipment to see hwich ones to set ?
   port_.tx <= 1; 
   port_.dtr <= 1;
   port_.dsr <= 1;
   port_.cts <= 1;
   port_.rts <= 1;
endtask


