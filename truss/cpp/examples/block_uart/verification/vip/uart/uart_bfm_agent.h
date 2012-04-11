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
#ifndef __uart_bfm_agent__
#define __uart_bfm_agent__

#include "teal.h"
#include "truss.h"

#include "uart_configuration.h"
#include "uart_bfm.h"

namespace uart {
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  struct block {
    block (teal::uint32 d) : block_delay_ (d) {};
    virtual ~block () {};

    void add_word (const word& w) {words_.push_back (w);}


    std::deque<uart::word> words_;
    teal::uint32 block_delay_;
  };



  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  typedef truss::channel_put<block> received_from_wire;
  typedef truss::channel_get<block> to_be_transmitted;
  typedef truss::channel<block> channel;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //adds channel interface, interface is in blocks as opposed to words
  class bfm_agent : public bfm {
  public:

    bfm_agent (const std::string& name,
	 truss::port <configuration::signals>::pins port,
	 const configuration* c,
	 to_be_transmitted* to_be_transmitted,  
	 received_from_wire* received_from_wire,  
	 teal::uint64 clock_frequency);

    virtual ~bfm_agent () {}

    virtual void start ();
    virtual void stop ();
    
  protected:
    virtual void receive_completed_ (const uart::word&);

    to_be_transmitted* to_be_transmitted_;
    received_from_wire*  received_from_wire_;
      
  private:
    void operator= (const bfm_agent&);
    bfm_agent (const bfm_agent&);

    void do_tx_thread ();
  };


};
#endif
