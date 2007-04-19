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
using namespace teal;

namespace {
  uint8 get_number_of_operations (uint8 min_v, uint8 max_v) {
    uint8 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }
}


#include "test_component.h"

#include "driver.h"
#include "generator.h"
#include "checker.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
alu::test_component::test_component (const std::string& n, generator* g, driver* b, checker* c) :
  truss::thread (n), truss::test_component (n), generator_ (g), driver_ (b), checker_ (c)
{
  log_.show_debug_level (debug); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::time_zero_setup () {
  log_ << teal_debug << " time_zero_setup" << teal::endm;
  driver_->time_zero_setup ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::out_of_reset (reset r) {
  log_ << teal_debug << " out_of_reset" << teal::endm;
  driver_->out_of_reset (r);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::randomize () {
  log_ << teal_debug << " randomize" << teal::endm;
  static uint8 min_words = dictionary::find (name + "_min_num_operations", (uint8)10);
  static uint8 max_words = dictionary::find (name + "_max_num_operations", (uint8)15);
  number_of_operations_ = get_number_of_operations (min_words, max_words);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::write_to_hardware () {
  log_ << teal_debug << " time_zero_setup" << teal::endm;
  driver_->write_to_hardware ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::wait_for_completion_ () {
  log_ << teal_debug << " internal wait for completion " << teal::endm;
  checker_->wait_for_completion ();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::start_components_ () 
{
  log_ << teal_debug << "Starting components." << teal::endm;
  driver_->start (); 
  checker_->start ();
}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::test_component::generate () 
{
  log_ << teal_debug << "alu::test_component::generate number of opertaions is " << dec << number_of_operations_ << endm;
  generator_->do_operations (number_of_operations_);
}

