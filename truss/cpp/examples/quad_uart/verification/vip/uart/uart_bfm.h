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
#ifndef __uart_bfm__
#define __uart_bfm__

#include "teal.h"
#include "truss.h"

#include "uart_configuration.h"

namespace uart {

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  //character gap, match char, force xon/off tx, remove xon/off in higher level channel
  struct word {
    word (teal::uint8 data_size, teal::uint8 bit_delay);
    virtual ~word ();

    typedef enum {ok = 0, parity_mismatch = 1, dsr_dtr_timeout = 2,
		  cts_timeout = 4, stop_bit_mismatch = 8, overrun_error = 16, framing_error = 32} status;
    teal::uint32 status_;  //if non-zero, data may be invalid

    bool operator== (const word &);
    teal::vout& operator<< (teal::vout&) const;
      
    /*rand*/ teal::uint8 bit_start_delay; //how many bit times to delay
    truss::random_reg data;

    virtual void randomize (const std::string& prefix,
			    teal::uint32 min_delay, teal::uint32 max_delay, 
			    const teal::reg& min_data, const teal::reg& max_data);

    //allow normal ctor and assigment
  };

  inline teal::vout& operator<< (teal::vout& c, const word& rhs) {
    return rhs.operator<< (c); //let virtual function take over
  }


  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  class port {
  public:
    port (truss::port <configuration::signals>::pins strings);
    virtual ~port () {};
    //allow standard copy and assign

    teal::vreg rx;
    teal::vreg tx;
    teal::vreg baud_rate_clock;
    teal::vreg dtr;
    teal::vreg dsr;
    teal::vreg cts;
    teal::vreg rts;
  };


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class bfm : public truss::verification_component, public truss::multi_thread  {
  public:
    bfm (const std::string& name, truss::port <configuration::signals>::pins port, const uart::configuration* c, teal::uint64 clock_frequency);
  
    virtual ~bfm () {}

    virtual void time_zero_setup ()     {}
    virtual void out_of_reset (reset)   {};
    virtual void randomize ()           {};

    virtual void write_to_hardware ();
    virtual void wait_for_completion () {};
    virtual void report (const std::string prefix) const {};

    virtual void send_word (const word&);

    //These set the DUT wires accordingly, where true maps to active
    virtual void dtr (bool);
    virtual void dsr (bool);

    virtual void cts (bool);
    virtual void rts (bool);

    virtual void start ();
    virtual void stop ();

  protected:
    virtual void receive_completed_ (const word&) = 0;

    const std::string name_;
    const configuration* configuration_;
      
    port port_;
      
    //valid after start
    teal::uint32 one_bit_; 
    teal::uint32 one_half_bit_; 
    teal::uint64 clock_frequency_;

    virtual void pause_ (teal::uint32);
    //    virtual void start_ ();

  private:
    void operator= (const bfm&);
    bfm (const bfm&);

    virtual void do_rx_thread ();
  };

};
#endif
