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
/*Copyright (C) 2006 Mike Mintz and Robert Ekendahl

 This file is part of the Trusster IP Library. 

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

`include "uart_16550_sfm.svh"
`include "uart_16550_registers.svh"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function uart_16550_sfm::new (string name, int index, virtual uart_16550_interface ui,
			      uart_configuration_16550 c, teal::uint64 clock_frequency);
   super.new (name);
   uart_16550_interface_ = ui;
   configuration_ = c;
   clock_frequency_ = clock_frequency;
   index_ = index;
//   log_.show_debug_level (99); 
endfunction

`define field_get(data, field) data[`field``_max:`field``_min]
`define field_put(data,field,value) data [`field``_max:`field``_min] = value

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::do_rx_thread_ ();
   forever begin
      uart_word current_rx = new (configuration_.data_size_,0);
      teal::uint8 data;
      string msg;
      int foo;
      
      log_.info ( "Wait for interrupt");
      @ (uart_16550_interface_.interrupt_request_);
      log_.info ("GOT 16550 interrupt");


      //DO status part
      teal::read  (bus_address_ (`UART_VERIF_REG_II), data, 8);
      msg = $psprintf (" Interrupt Identification: 0x%0x", data);
      log_.debug (msg);
      

      if ((`field_get (data, interrupt_identification) == `interrupt_identification_received_data) ||
	  (`field_get (data, interrupt_identification) == `interrupt_identification_timeout)) begin
	 teal::read  (bus_address_ (`UART_VERIF_REG_RB), current_rx.data);
      end

      do_receive_completed_ (current_rx);
   end // for (;;)
endtask 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::do_receive_completed_ (uart_word the_word);
   uart_block current_rx = new (0);
   log_.info ({" received word: ", the_word.sreport ()});
   current_rx.add_word (the_word);
   receive_completed_ (current_rx);
endtask



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::send (uart_block  current_tx);
  log_.info ({" Block to be transmitted! ", current_tx.sreport ()} );
   if (current_tx.block_delay_) begin pause_ (one_bit_ * current_tx.block_delay_);  end

   for (int i = 0; (i <= current_tx.max_offset ()); ++i) begin
      `truss_assert (current_tx.words_[i].status_ == 0); 
      teal::write (bus_address_ (`UART_VERIF_REG_TR), current_tx.words_[i].data[7:0], 8);  
   end
endtask


//need to implement these 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::dtr (bit new_value); `truss_assert (0); endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::dsr (bit new_value); `truss_assert (0); endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::cts (bit new_value); `truss_assert (0); endtask

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::rts (bit new_value); `truss_assert (0); endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::write_to_hardware ();
   teal::uint8 data;
   teal::uint64 divisor = (clock_frequency_ + configuration_.baud_rate_ - 1) / (16 * configuration_.baud_rate_);
   one_bit_ = (clock_frequency_ + configuration_.baud_rate_ - 1) / configuration_.baud_rate_;
   begin
      string msg;
      msg = $psprintf ("divisor %0d clock_frequency_ %0d configuration_.baud_rate_ %0d", divisor, clock_frequency_, configuration_.baud_rate_);
      log_.debug (msg);
   end
   `truss_assert (one_bit_); //otherwise there is not enough clock resolution
   `truss_assert (divisor); //otherwise there is not enough clock resolution
   `truss_assert ((divisor >> 16) == 0); //otherwise there is too fast, cannot divide down to get baudrate.


   data = 0;

   //allow interrupt's of interest
   data = 0;
   data [`interrupt_enable_receive_max:`interrupt_enable_receive_min]  =  1;
   teal::write (bus_address_ (`UART_VERIF_REG_IE), data, 8); 

   //set flow control trigger level
   data = 0;
   teal::write (bus_address_ (`UART_VERIF_REG_FC), data, 8); 

   //write8 configuration
   data = 0; 
   `truss_assert (configuration_.data_size_ >= 5);
   `truss_assert (configuration_.data_size_ <= 8);
   data [`data_size_max: `data_size_min] = configuration_.data_size_ - 5;

   if (configuration_.stop_bits_ != uart::one) begin
      data [`stop_bits_max:`stop_bits_min] = 1;
   end
   
   case (configuration_.parity_)
     uart::none : ;
     uart::even: begin
	`field_put (data, parity_enable, 1);
	`field_put (data, parity_type, 01);
     end
     uart::odd : begin
	`field_put (data, parity_enable, 1);
	`field_put (data, parity_type, 0);
     end
     uart::mark : begin
	`field_put (data, parity_enable, 1);
	`field_put (data, parity_type, 2);
     end
     uart::space: begin
	`field_put (data, parity_enable, 1); 
	`field_put (data, parity_type, 3);
     end
   endcase

   `field_put (data, access_clock_divide, 1);
   teal::write (bus_address_ (`UART_VERIF_REG_LC), data, 8);      //could have done this in a filter below here and above memory_bank
   begin
      teal::uint8 lc_save = data;
   
   teal::read (bus_address_ (`UART_VERIF_REG_LC), data, 8);      //could have done this in a filter below here and above bank


   data = divisor;
   teal::write (bus_address_ (`UART_VERIF_REG_DL1), data, 8); 
   data = divisor >> 8;
   teal::write (bus_address_ (`UART_VERIF_REG_DL2), data, 8);

   `field_put (lc_save, access_clock_divide, 0);
   teal::write (bus_address_ (`UART_VERIF_REG_LC), lc_save, 8);
      end
endtask // uart

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::start ();       fork do_rx_thread_();	   join_none endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task uart_16550_sfm::pause_ (teal::uint32 count);
  for (teal::uint32 i = count; i; --i) begin
    @ (uart_16550_interface_.baud_rate_clock_);
  end
endtask // uart

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function teal::uint32 uart_16550_sfm::bus_address_ (teal::uint32 offset);
   return (index_<<3) + offset;
endfunction
					


