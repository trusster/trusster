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
#ifndef __uart_1655__
#define __uart_1655__

#include "uart_16550_configuration.h"

#include "uart_bfm_agent.h" //to get block;

#include "truss.h"

namespace uart {
  //WARNING: MUST ALWAYS BE data_terminal_equipment
  //(1) This uart DUT looks to always be a data_terminal_equipment. (not clear from documentation)
  //(2) Would have to add to this code and vip to swizzle lines in software and have assigns in testbench

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class uart_16550_sfm : public truss::verification_component, public truss::multi_thread {
  public:
    uart_16550_sfm (const std::string& name,
	       truss::port <configuration_16550::signals>::pins port, //must also contain interrupt wire 
	 const configuration* c, teal::uint64 clock_frequency);

    virtual ~uart_16550_sfm () {}

    virtual void time_zero_setup ()     {}
    virtual void out_of_reset (reset)   {};
    virtual void randomize ()           {};
    virtual void start ();
    virtual void stop ();
    virtual void write_to_hardware ();
    virtual void wait_for_completion () {};
    virtual void report (const std::string prefix) const {};

    //These set the DUT wires accordingly, where true maps to active
    virtual void dtr (bool);
    virtual void dsr (bool);

    virtual void cts (bool);
    virtual void rts (bool);

    void send (const block&);

  protected:
    virtual void receive_completed_ (const uart::block&) = 0;
    

  private:
    void operator= (const uart_16550_sfm&);
    uart_16550_sfm (const uart_16550_sfm&);

    virtual void do_receive_completed_ (const uart::word&);

    teal::vreg interrupt_request_;
    teal::vreg baud_rate_clock_;

    const configuration* configuration_;

    //valid after start
    teal::uint32 one_bit_; 
    teal::uint64 clock_frequency_;

    virtual void pause_ (teal::uint32);
    void do_rx_thread_ ();
  };
};


#endif
