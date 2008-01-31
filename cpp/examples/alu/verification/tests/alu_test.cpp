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

#include "alu_test.h"
#include "test_component.h"
#include "testbench.h"

alu_test::alu_test (testbench* tb, truss::watchdog* wd, const std::string& n) :  
  test_base (n, wd), testbench_ (tb),
  test_component_ (new alu::test_component("test_component", tb->generator,  tb->driver,  tb->checker))
{
  log_.show_debug_level (teal::debug);
  assert (tb->generator);
      assert (tb->driver);
      assert (tb->checker);
  log_ << teal_debug << "alu_test new() begin " << teal::endm;

  //add generator default constraints
    teal::dictionary::put (test_component_->name + "_min_num_operations", "4",  teal::dictionary::default_only);
    teal::dictionary::put (test_component_->name + "_max_num_operations", "10",  teal::dictionary::default_only);
  log_ << teal_debug << "alu_test new() end " << teal::endm;
}

void alu_test::time_zero_setup () {test_component_->time_zero_setup ();}

void alu_test::out_of_reset (reset r) {test_component_->out_of_reset (r);}
void alu_test::randomize () {test_component_->randomize ();}
void alu_test::write_to_hardware () {  test_component_->write_to_hardware ();}
void alu_test::start () {  test_component_->start ();}
void alu_test::stop () {test_component_->stop (); }
void alu_test::wait_for_completion () {test_component_->wait_for_completion ();}
void alu_test::report (const std::string prefix) const {test_component_->report (prefix);}
  
