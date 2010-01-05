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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   fixed position random generators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
  teal::random_range* get_data_generator () {
    static teal::random_range* foo = new teal::random_range (__FILE__, __LINE__);
    return foo;
  }
};

#include "uart_bfm.h"
using namespace teal;
using namespace truss;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Word
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::word::word (teal::uint8 word_length, 
					     teal::uint8 bit_delay) :
  status_ (0), bit_start_delay (bit_delay), data (get_data_generator (),  word_length)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::word::~word () {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ignore delay, since its a tx thing (although one could concievably measure and check this)
bool uart::word::operator== (const word & w)
{
  return (status_ == w.status_) && (data == w.data); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& uart::word::operator<< (teal::vout& v) const
{
  v << "delay of " << teal::dec << bit_start_delay << " status: " << teal::hex << status_ << " " << data;
  return (v);
}
      
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::word::randomize (const std::string& prefix,
						  uint32 min_delay, 
						  uint32 max_delay, 
						  const reg& min_data, 
						  const reg&  max_data)
{
  //  static reg min_data = dictionary::find (prefix + "min_data", min_data);   //need ctor form string, operator >>
  //  static reg max_data_size = dictionary::find (prefix + "max_data", max_data);
  //  RAND_RANGE(data, min_data, max_data);
  data.randomize (min_data, max_data);

  uint8 a_min_delay (dictionary::find (prefix + "min_bit_delay", min_delay));
  uint8 a_max_delay = dictionary::find (prefix + "max_bit_delay", max_delay);
   //    log_ << teal_debug << "use min delay of " << dec << a_min_delay << " passed in was " << min_delay << endm;
   //    log_ << teal_debug << "use max delay of " << a_max_delay << " passed in was " << max_delay << endm;
  RAND_RANGE (bit_start_delay, a_min_delay, a_max_delay); //move to function at top!
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physical later
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port  - a utility aggregration of the vreg needed for this interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::port::port (truss::port <configuration::signals>::pins strings) :
  rx (strings[configuration::rx], 1, teal::vreg::observe_only),
  tx (strings[configuration::tx], 1, teal::vreg::observe_and_control),
  baud_rate_clock (strings[configuration::baud_rate_clock], 1, teal::vreg::observe_only),
  dtr (strings[configuration::dtr], 1, teal::vreg::observe_and_control),
  dsr (strings[configuration::dsr], 1, teal::vreg::observe_and_control),
  cts (strings[configuration::cts], 1, teal::vreg::observe_and_control),
  rts (strings[configuration::rts], 1, teal::vreg::observe_and_control)
{}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   bfm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uart::bfm::bfm (const std::string& name, truss::port <configuration::signals>::pins p,
				      const uart::configuration* c, uint64 clock_frequency) :
  verification_component (name),  multi_thread (name), configuration_ (c), port_ (p), clock_frequency_ (clock_frequency)
{  
  log_.show_debug_level (4); 
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static reg generate_parity (uart::configuration::parity p, reg data)
{
  switch (p) {
  case uart::configuration::even:  { return  reg (reduce_xor (data)); break; }
  case uart::configuration::odd :  { return ~reg (reduce_xor (data)); break; }
  case uart::configuration::mark:  { return 1; break; }
  case uart::configuration::space: { return 0; break; }
  case uart::configuration::none:  { teal::vout log_ ("generate_parity"); truss_assert (0);}
  }
  return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool check_parity (uart::configuration::parity p, reg data, reg parity_received) 
{
  return generate_parity (p, data) == parity_received;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::do_rx_thread ()
{
  uint32 word_count (0);
  word current_rx (configuration_->data_size_, 0);
  for (;;) {
    //dtr/dsr on word boundry only
    if (configuration_->equipment_ == configuration::data_communications_equipment) {
      while ( (configuration_->use_dtr_dsr_) && (port_.dsr == 0)) {
	pause_ (1);
      }
    }
    else {
      while ( (configuration_->use_dtr_dsr_) && (port_.dtr == 0)) {
	pause_ (1);
      }
    }
#if 1
    reg current_bit = port_.rx;
    while (current_bit != 0) {
      while (port_.rx == 1) {
	pause_ (1);
      }
      //      log_ << teal_debug << level (3) << " possible start bit (negedge found)" << endm;
#else
    //now wait for start bit..defined as a high (1) to low (0) transition (at least one bit wide)
    //but must assume that we are putting out the last stop bit (a low)
    //so, ...find a 1
    reg current_bit = port_.rx;
    while (current_bit == 0) {
      pause_ (1);
      current_bit = port_.rx;
    }
    //now find the start of the 0.. and make sure it's there  1/2 a bit time later
    //ASSUMES that there is a pullup on the rx line
    //mfm 9/17/05 why not just use at (neg_edge (port_.rx)) ?
    //mfm Jan 8,2006 - because (1) it may already be 0 and (2) should emulate sampling
    while (current_bit != 0) {
      while (port_.rx == 1) {
	pause_ (1);
      }
      log_ << teal_debug << level (3) << " possible start bit (negedge found)" << endm;
#endif
      //assume that this is the real beginning of the start bit
      //delay and get out of the outer loop is its still zero
      pause_ (one_half_bit_);
      current_bit = port_.rx;
      if (current_bit == 0) log_ << teal_debug << " found start bit (still 0 at 1/2 bit time)" << endm;
    }

      
    //get the data
    for (uint8 i (0); i < configuration_->data_size_; ++i) {
      pause_ (one_bit_); 
      current_rx.data (i, i) = port_.rx;	
      log_ << teal_debug << " receive bit " << i << " is " << current_rx.data (i, i) << endm;
    }

    //now the parity
    if (configuration_->parity_ != configuration::none) {
      pause_ (one_bit_); 
      reg parity_received = port_.rx; 
      if (! check_parity (configuration_->parity_, current_rx.data, parity_received)) {
	current_rx.status_ |= word::parity_mismatch;
      }
    }

    //how/should we verify stop bits?
      switch (configuration_->stop_bits_) {
      case configuration::one: pause_ (one_bit_); break;
      case configuration::one_and_one_half: pause_ (one_bit_ + one_half_bit_); break;
      case configuration::two: pause_ (one_bit_ << 1); break;
      default: truss_assert (0);
      }
      //check that it is still a stop bit
      if (port_.rx != 1) {
	current_rx.status_ |= word::stop_bit_mismatch;	
      } 

      pause_ (one_half_bit_ -1); //complete the stop bit
      log_ << teal_debug << "done receiving stop bit: " << port_.rx << endm;

    receive_completed_ (current_rx);
    word_count++;
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::send_word (const word& current_tx)
{
  log_ << teal_debug << " Word to be transmitted: " << current_tx << teal::endm;
  if (current_tx.bit_start_delay) {
	log_ << teal_debug << "pause before tx " << dec << current_tx.bit_start_delay << endm;
    pause_ (one_bit_ * current_tx.bit_start_delay);
    	log_ << teal_debug << "after pause tx " << current_tx.bit_start_delay << endm;
  }
      
  //now dsr/dtr
  //dtr/dsr on word boundry only
  if (configuration_->equipment_ == configuration::data_communications_equipment) {
    while ( (configuration_->use_dtr_dsr_) && (port_.dsr == 0)) {
      pause_ (1);
    }
  }
  else {
    while ( (configuration_->use_dtr_dsr_) && (port_.dtr == 0)) {
      pause_ (1);
    }
  }

  //now cts/rts
  //who sets rts?
  uint32 cts_time_out (0);
  while ( (configuration_->use_cts_rts_) && (port_.cts == 0)) {
    pause_ (1);
    if (cts_time_out++ >= configuration_->cts_timeout_) {
      //way to tell the outside world is to create a receive status word
      word error_rx (configuration_->data_size_, 0);
      error_rx.status_ |= word::cts_timeout;
      receive_completed_ (error_rx);
      cts_time_out = 0;
    }
  }

  //send start bit
  log_ << teal_debug << "sending start bit " << endm;
  port_.tx = 0; //active state?
  pause_ (one_bit_);
  log_ << teal_debug << "done sending start bit " << endm;

  //send data , should add a MSB first feature
  for (uint32 i(0); i < configuration_->data_size_; ++i) {
    port_.tx = current_tx.data (i,i);
    log_ << teal_debug << "sending bit " << i << " " << current_tx.data (i,i) << endm;
    pause_ (one_bit_);
  }
	
  //now the parity
  if (configuration_->parity_ != configuration::none) {
    log_ << teal_debug << "sending parity " << endm;
    port_.tx = generate_parity (configuration_->parity_, current_tx.data);
    pause_ (one_bit_); 
  }

  //stop_bits
  log_ << teal_debug << "sending stop bit " << endm;
  port_.tx = 1;
  switch (configuration_->stop_bits_) {
    case configuration::one: pause_ (one_bit_); break;
    case configuration::one_and_one_half: pause_ (one_bit_ + one_half_bit_); break;
    case configuration::two: pause_ (one_bit_ << 1); break;
    default: truss_assert (0);
  }
  log_ << teal_debug << "done sending stop bit " << endm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::pause_ (uint32 count) 
{
  log_ << teal_debug << level (22) << " in pause for " << count << endm;
  for (uint32 i (count); i; --i) {
    at (posedge (port_.baud_rate_clock));
  }
  log_ << teal_debug << level (22) << " return pause for " << count << endm;

}

  //The wire values asserted for the control signals
  const teal::uint8 uart_active = 1;
  const teal::uint8 uart_inactive = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::dtr (bool new_value)
{  port_.dtr = new_value ? uart_active : uart_inactive; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::dsr (bool new_value)
{  port_.dsr = new_value ? uart_active : uart_inactive;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::cts (bool new_value)
{  port_.cts = new_value ? uart_active : uart_inactive;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::rts (bool new_value)
{  port_.rts = new_value ? uart_active : uart_inactive;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::start () 
{
  one_bit_ = (clock_frequency_ + configuration_->baud_rate_ - 1) / configuration_->baud_rate_;
  log_ << teal_debug << " one bit is " << dec << one_bit_ << endm;

  one_half_bit_ = one_bit_ / 2; //otherwise there is not enough clock resolution
  assert (one_half_bit_);

  run ( &uart::bfm::do_rx_thread, "do_rx_thread");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::stop () 
{
  multi_thread::stop ("do_rx_thread");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::bfm::write_to_hardware ()
{
  return; //hack for systemc and driver weirdness
  port_.tx = 1; 
  port_.dtr = 1;
  port_.dsr = 1;
  port_.cts = 1;
  port_.rts = 1;
}


