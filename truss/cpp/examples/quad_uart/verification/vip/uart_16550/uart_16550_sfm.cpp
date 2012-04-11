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

#include "uart_16550_sfm.h"
#include "uart_16550_registers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::uart_16550_sfm::uart_16550_sfm (const std::string& name,   teal::uint32 id, 
				    truss::port <configuration_16550::signals>::pins  port,
				    const uart::configuration* c, teal::uint64 clock_frequency) :
  verification_component (name), 
  multi_thread (name),
  interrupt_request_ (port[uart::configuration_16550::interrupt], 1, teal::vreg::observe_only),
  baud_rate_clock_   (port[uart::configuration_16550::reference_clock], 1, teal::vreg::observe_only),
  configuration_ (c),
  clock_frequency_ (clock_frequency),
  id_ (id)
{
    log_.show_debug_level (99); 
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::do_rx_thread_ () {
  for (;;) {
    log_ << "Wait for interrupt" << teal::endm;
    teal::at (teal::posedge (interrupt_request_));
    log_ << "GOT 16550 interrupt" << teal::endm;

    word current_rx (configuration_->data_size_, 0);
    //DO status part
    pause_ (one_bit_); //race condition in rtl???
    teal::reg data (0,8);
    truss_reg_read8 (bus_address (UART_REG_II), &data);
    log_ << teal_debug << " Interrupt Identification:" << data << teal::endm;
#if 0
#if 1
    if ((truss_field_get (data, interrupt_identification) == interrupt_identification_received_data) ||
	(truss_field_get (data, interrupt_identification) == interrupt_identification_timeout)) {
      truss_reg_read8 (bus_address (UART_REG_RB), &current_rx.data);
      log_ << teal_debug << " Received a data byte:" << current_rx.data << teal::endm;
      do_receive_completed_ (current_rx);
    }
    else {
      log_ << teal_fatal << " Spurrious interrupt (possibly timeout)." << teal::endm;
    }

#else
    truss_reg_read8 (bus_address (UART_REG_LS), &data);  
    log_ << teal_debug << " RS Line Status:" << data << teal::endm;
    if  (truss_field_get (data, data_received) == 1) {
      truss_reg_read8 (bus_address (UART_REG_RB), &current_rx.data);
      log_ << teal_debug << " Received a data byte:" << current_rx.data << teal::endm;
    }
    if (truss_field_get (data, parity_error) == 1) {
      log_ << teal_debug << " RS Line Stat555us:" << data << teal::endm;
      current_rx.status_ |= uart::word::parity_mismatch;
    }
    if (truss_field_get (data, overrun_error) == 1) {
      log_ << teal_debug << " RS Line S666tatus:" << data << teal::endm;
      current_rx.status_ |= uart::word::overrun_error;
    }
    if (truss_field_get (data, framing_error) == 1) {
      log_ << teal_debug << " RS Line7777 Status:" << data << teal::endm;
      current_rx.status_ |= uart::word::framing_error;
    }

    do_receive_completed_ (current_rx);
    //how to clear interrupt???
#endif
#else
      truss_reg_read8 (bus_address (UART_REG_RB), &current_rx.data);
    do_receive_completed_ (current_rx);
#endif
#if 0
    truss_reg_read8 (bus_address (UART_REG_II), &data);
#if 1
    if ((truss_field_get (data, interrupt_identification) == interrupt_identification_received_data) ||
	(truss_field_get (data, interrupt_identification) == interrupt_identification_timeout)) {
#else
      if (interrupt_request_ == 1) { //still pending?
#endif
	loop_body_ ();
      }
    }
#endif
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::do_receive_completed_ (const word& current_rx_word) { 
  log_ << teal_info << " received word: " << current_rx_word << teal::endm;
  block current_rx = block (0);
  current_rx.add_word (current_rx_word);
  receive_completed_ (current_rx);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::send (const block&  current_tx)
{
    log_ << teal_info << " Block to be transmitted!" << teal::endm;
    if (current_tx.block_delay_) { pause_ (one_bit_ * current_tx.block_delay_);  }
    for (std::deque<word>::const_iterator it (current_tx.words_.begin ()); (it != current_tx.words_.end ()); ++it) {
      truss_assert ((*it).status_ == 0); //how to inject errors in to real DUT?
      truss_reg_write8 (bus_address (UART_REG_TR), (*it).data);  
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::dtr (bool new_value)
{  //some write register?
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::dsr (bool new_value)
{  //some write register?
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::cts (bool new_value)
{  //some write register?
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::rts (bool new_value)
{  //some write register?
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::write_to_hardware ()
{
  one_bit_ = (clock_frequency_ + configuration_->baud_rate_ - 1) / configuration_->baud_rate_;
  truss_assert (one_bit_); //otherwise there is not enough clock resolution

  teal::uint64 divisor = (clock_frequency_ + configuration_->baud_rate_ - 1) / (16 * configuration_->baud_rate_);
  truss_assert (divisor); //otherwise there is not enough clock resolution
  truss_assert ((divisor >> 16) == 0); //otherwise there is too fast, cannot divide down to get baudrate.

  log_ << teal_debug << " one bit is 0x" << teal::hex << one_bit_ << teal::endm;

  teal::reg data (0, 8); //byte register! - important to perform 8 bit writes.
  data = 0;

  //allow interrupt's of interest
  data = 0;
  truss_field_put (data, interrupt_enable_receive, 1)
  truss_reg_write8 (bus_address (UART_REG_IE), data); 
  log_ << teal_debug << " DDDDDDDDD HACK" << teal::hex << one_bit_ << teal::endm;

  //set flow control trigger level
  data = 0;
  truss_reg_write8 (bus_address (UART_REG_FC), data); 

  //write8 configuration
  data = 0; 
  truss_assert (configuration_->data_size_ >= 5);
  truss_assert (configuration_->data_size_ <= 8);
  truss_field_put (data, data_size, configuration_->data_size_ - 5);

  if (configuration_->stop_bits_ != uart::configuration::one) {
    truss_field_put (data, stop_bits, 1);
  }
    
  switch (configuration_->parity_) {
  case uart::configuration::none : break;
  case uart::configuration::even: {
    truss_field_put (data, parity_enable, 1);
    truss_field_put (data, parity_type, 01);
    break;
  }
  case uart::configuration::odd : {
    truss_field_put (data, parity_enable, 1);
    truss_field_put (data, parity_type, 0);
    break;
  }
  case uart::configuration::mark : {
    truss_field_put (data, parity_enable, 1);
    truss_field_put (data, parity_type, 2);
    break;
  }
  case uart::configuration::space: {
    truss_field_put (data, parity_enable, 1);
    truss_field_put (data, parity_type, 3);
    break;
  }
  default: truss_assert (0); 
  }

  truss_field_put (data, access_clock_divide, 1);
  truss_reg_write8 (bus_address (UART_REG_LC), data);      //could have done this in a filter below here and above memory_bank
  teal::reg lc_save = data;
  
  truss_reg_read8 (bus_address (UART_REG_LC), &data);      //could have done this in a filter below here and above memory_bank
  //  truss_assert (data == lc_save);

  data = divisor;
  truss_reg_write8 (bus_address (UART_REG_DL1), data); 
  data = divisor >> 8;
  truss_reg_write8 (bus_address (UART_REG_DL2), data);

  truss_field_put (lc_save, access_clock_divide, 0);
  truss_reg_write8 (bus_address (UART_REG_LC), lc_save);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::start () 
{
  run (&uart_16550_sfm::do_rx_thread_, "do_rx_thread");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::stop () 
{
  log_ << teal_debug << " rx_stopping  " << teal::endm;
  multi_thread::stop ("do_rx_thread");
  log_ << teal_debug << " rx_stopped  " << teal::endm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::uart_16550_sfm::pause_ (teal::uint32 count) 
{
  log_ << teal_debug << teal::level (2) << " in pause for " << count << teal::endm;
  for (teal::uint32 i (count); i; --i) {
    teal::at (teal::posedge (baud_rate_clock_));
  }
  log_ << teal_debug << teal::level (2) << " return pause for " << count << teal::endm;
}
