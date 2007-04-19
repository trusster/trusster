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
#ifndef __quad_uart_irritators__
#define __quad_uart_irritators__

#include "truss_test_base.h"
#include "testbench.h"

#include "truss.h"


namespace uart {
  typedef class basic_test_component;
  typedef class basic_irritator;
};

//move to book...
//I know managing the test components explicitly is tedious but
// (1) if you have more than 10 or so, you need to create clumping objects linke standard_chip1_components, etc
//(2) this is far preferable to a global (read invisible) management system

const teal::uint32 number_of_uart_irritators = number_of_uarts - 1;


class quad_uart_irritators : public truss::test_base {
 public:
  quad_uart_irritators (testbench* tb, truss::watchdog* w, const std::string& n = "quad_uart_irritators");  //ADD Watchdog!
  virtual ~quad_uart_irritators () {}

  virtual void time_zero_setup ();
  virtual void out_of_reset (reset r);
  virtual void randomize ();
  virtual void write_to_hardware ();
  virtual void start ();
  virtual void stop ();
  virtual void wait_for_completion ();
  virtual void report (const std::string prefix) const;
  
 private:
  testbench* testbench_;
  uart::basic_test_component* uart_test_component_ingress_;
  uart::basic_test_component* uart_test_component_egress_;

  uart::basic_irritator* uart_irritator_ingress_[number_of_uart_irritators];
  uart::basic_irritator* uart_irritator_egress_[number_of_uart_irritators];
};


#endif
