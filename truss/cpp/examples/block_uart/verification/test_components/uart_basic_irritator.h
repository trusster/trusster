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
#ifndef __uart_basic_irritator__
#define __uart_basic_irritator__

#include "uart_basic_test_component.h"

//See the basic exerciser for the control words.

namespace uart {

class basic_irritator : public truss::irritator, public basic_test_component {
 public:
  basic_irritator (const std::string& n, uart::generator* g, truss::verification_component* b, uart::checker* c) : 
    truss::thread (n), truss::irritator (n), basic_test_component  (n, g,b,c), test_component (n) {
    teal::vout log_ (n); log_ << teal_debug << " ctor" << teal::endm;
  }

  virtual ~basic_irritator () {}

  virtual void start () {    teal::vout log_ (name); log_ << teal_debug << " start begin " << teal::endm; irritator::start (); log_ << teal_debug << " start end" << teal::endm;}
  virtual void stop () {irritator::stop ();}
  virtual void report (const std::string prefix) const {basic_test_component::report (prefix);}
  virtual void time_zero_setup () {basic_test_component::time_zero_setup ();}
  virtual void out_of_reset (reset r) {basic_test_component::out_of_reset (r);}
  virtual void write_to_hardware () {basic_test_component::write_to_hardware ();}
  virtual void wait_for_completion () {basic_test_component::wait_for_completion ();}

 protected:
  //could do a random delay, but how about just making sure there is a constant amount of data flowing
  //could also have an initial generate burst
  virtual void inter_generate_gap () {checker_->wait_actual_check ();}

  virtual void randomize () {basic_test_component::randomize ();}
  virtual void wait_for_completion_ () {basic_test_component::wait_for_completion_ ();}
  virtual void start_components_ () {basic_test_component::start_components_ ();}
  virtual void generate () {basic_test_component::generate ();}

 private:
  basic_irritator (const basic_irritator&);
  basic_irritator operator= (const basic_irritator&);
};

};

#endif
