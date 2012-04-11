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

#include "uart_configuration.h"

#include "truss.h"
using namespace teal;
using namespace uart;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vout& operator<< (vout& v, const configuration::parity& p) 
{
  switch (p) {
    case configuration::none : v << "none"; break;
    case configuration::even : v << "even"; break;
    case configuration::odd : v << "odd"; break;
    case configuration::mark : v << "mark"; break;
    case configuration::space : v << "space"; break;
  default: v << "unknown";
  }
  return v;
}


vout& operator<< (vout& v, const configuration::stop_bits& s) 
{
  switch (s) {
    case configuration::one : v << "1"; break;
    case configuration::one_and_one_half : v << "1.5"; break;
    case configuration::two : v << "2"; break;
  default: v << "unknown";
  }
  return v;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vout& operator<< (vout& v, const configuration::baud_rate& b) 
{
  switch (b) {
    case configuration::b_150 : v << "150"; break;
    case configuration::b_300 : v << "300"; break;
    case configuration::b_600 : v << "600"; break;
    case configuration::b_1200 : v << "1200"; break;
    case configuration::b_2400 : v << "2400"; break;
    case configuration::b_4800 : v << "4800"; break;
    case configuration::b_9600 : v << "9600"; break;
    case configuration::b_19200 : v << "19200"; break;
    case configuration::b_38400 : v << "38400"; break;
    case configuration::b_57600 : v << "57600"; break;
    case configuration::b_115200 : v << "115200"; break;
    case configuration::b_230400 : v << "230400"; break;
    case configuration::b_460800 : v << "460800"; break;
    case configuration::b_921600 : v << "921600"; break;
  default: v << "unknown";
  }
  return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::configuration::configuration (const std::string& n) :
  parity_ (none), baud_rate_ (b_921600), data_size_ (8), stop_bits_ (one),
  use_dtr_dsr_ (false), use_cts_rts_ (false),
  name (n), log_ (n)
{}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& uart::configuration::operator<< (teal::vout& v) const
{
  v << " parity: ";  ::operator<< (v, parity_);
  v << " baud_rate "; ::operator<< (v, baud_rate_);
  v << " data_size_  " << teal::dec << data_size_;
  v << " stop_bits "; ::operator<< (v, stop_bits_);
  v << " use_dtr_dsr: " << use_dtr_dsr_;
  v << " use cts_rts: " << use_cts_rts_;
  return (v);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::configuration::parity operator++ (configuration::parity& p) {
  return p = static_cast<configuration::parity> (p + 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::configuration::stop_bits operator++ (configuration::stop_bits& s) 
{
  return s = static_cast<configuration::stop_bits> (s + 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::configuration::baud_rate operator++ (configuration::baud_rate& m) {
  vout test("foo");
  test << teal_debug << "baud operator++ " << m << endm;
  switch (m) {
  case configuration::b_150: return m = configuration::b_300;
  case configuration::b_300: return m = configuration::b_600;
  case configuration::b_600: return m = configuration::b_1200;
  case configuration::b_1200: return m = configuration::b_2400;
  case configuration::b_2400: return m = configuration::b_4800;
  case configuration::b_4800: return m = configuration::b_9600;
  case configuration::b_9600: return m = configuration::b_19200;
  case configuration::b_19200: return m = configuration::b_38400;
  case configuration::b_38400: return m = configuration::b_57600;
  case configuration::b_57600: return m = configuration::b_115200;
  case configuration::b_115200: return m = configuration::b_230400;
  case configuration::b_230400: return m = configuration::b_460800;
  case configuration::b_460800: return m = configuration::b_921600;
  default: {vout log_ = vout ("uart::configuration::baud_rate operator++"); truss_assert (0);};
  };
  test << teal_debug << "baud operator++ switch fall through " << m << endm;
  return m = configuration::b_150;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart::configuration::randomize ()
{
  static baud_rate min_baud = static_cast<baud_rate> (dictionary::find 
						      (name + "_min_baud", 150));
  static baud_rate max_baud = static_cast<baud_rate> (dictionary::find 
						      (name + "_max_baud", 921600));
  vout test ("configuration: " + name);
  test << teal_debug << "generate baud from [" << dec << min_baud << "..." << max_baud << "]" << endm;

  std::vector<baud_rate> choices_b;
  uint32 dead (0);
  for (baud_rate i(min_baud) ; i < max_baud; ++i) {
    choices_b.push_back (i);
    test << teal_debug << "add baud " << dec << i  << " size is " << choices_b.size () << endm;
    if (++dead >= 100000) break;
  }
  if (dead >= 100000) test << dec << teal_error << " unable to randomize uart configuration baud_rate" << endm;
  choices_b.push_back (max_baud);
  uint32 b_now(0); RAND_RANGE(b_now, 0, choices_b.size () - 1);
  baud_rate_ = choices_b[b_now];


  static parity min_parity = static_cast<parity> (dictionary::find (name + "_min_parity", 0));
  static parity max_parity = static_cast<parity> (dictionary::find (name + "_max_parity", 4));
  std::vector<parity> choices_p;
  dead = 0;
  for (parity i(min_parity) ; i < max_parity; ++i) {
    choices_p.push_back (i);
    test << teal_debug << "add parity " << dec << i  << " size is " << choices_p.size () << endm;
    if (dead++ >= 100000) break;
    if (i == space) break;
  }
  choices_p.push_back (max_parity);
  if (dead >= 100000) test << teal_error << dec << "unable to randomize uart configuration parity" << endm;
  uint32 p_now; RAND_RANGE(p_now, 0, choices_p.size () - 1);
  parity_ = choices_p[p_now];

  static uint32 min_data_size = dictionary::find (name + "_min_data_size", 1);
  static uint32 max_data_size = dictionary::find (name + "_max_data_size", 12);
  RAND_RANGE(data_size_, min_data_size, max_data_size);

  static stop_bits min_stop_bits = static_cast<stop_bits> (dictionary::find (name + "_min_stop_bits", 0));
  static stop_bits max_stop_bits = static_cast<stop_bits> (dictionary::find (name + "_max_stop_bits", 2));
  std::vector<stop_bits> choices_s;
  dead = 0;
  for (stop_bits i(min_stop_bits) ; i < max_stop_bits; ++i ) {
    choices_s.push_back (i);
    if (dead++ >= 100000) break;
    test << teal_debug << "add stop " << dec << i  << " size is " << choices_s.size () << endm;
    if (i == two) break;
  }
  choices_s.push_back (max_stop_bits);
  if (dead >= 100000) test << teal_error << dec << "unable to randomize uart configuration stop_bits" << endm;
  uint32 s_now; RAND_RANGE(s_now, 0, choices_s.size () - 1);
  stop_bits_ = choices_s[s_now];

  static uint32 use_dtr_dsr_probability = dictionary::find (name + "use_dtr_dsr_probability", 0);
  uint8 use_dtr_dsr_probability_now; RAND_RANGE(use_dtr_dsr_probability_now, 0, 99);
  use_dtr_dsr_ = (use_dtr_dsr_probability_now < use_dtr_dsr_probability);

  static uint32 use_cts_rts_probability = dictionary::find (name + "use_cts_rts_probability", 0);
  uint8 use_cts_rts_probability_now; RAND_RANGE(use_cts_rts_probability_now, 0, 99);
  use_cts_rts_ = (use_cts_rts_probability_now < use_cts_rts_probability);
  test << teal_debug << "Done Randomize" << endm;
  report ("After Randomize:");
}

 void uart::configuration::report (const std::string prefix) const
{
  log_ << teal_info << prefix << *this << endm;
}
