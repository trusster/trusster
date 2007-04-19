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
namespace {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  teal::uint32 get_uart_index (teal::uint32 min_v, teal::uint32 max_v) {
    teal::uint32 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }
};


#include "quad_uart_irritators.h"
#include "uart_basic_test_component.h"
#include "uart_basic_irritator.h"

#include "uart_bfm.h"
#include "uart_16550_sfm.h"
#include "uart_generator.h"
#include "uart_checker.h"
#include "uart_16550_configuration.h"
#include "uart_interface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
quad_uart_irritators::quad_uart_irritators (testbench* tb, truss::watchdog* w, const std::string& n) :  
  truss::test_base (n, w), testbench_ (tb)
{
  truss_assert (number_of_uarts >= 2);
}


namespace {
  void standard_configuration (const std::string& name) {
    //add configuration default constraints
    teal::dictionary::put (name + "_min_baud", "4800",    teal::dictionary::default_only);
    teal::dictionary::put (name + "_min_data_size", "5",  teal::dictionary::default_only);
    teal::dictionary::put (name + "_max_data_size", "8", teal::dictionary::default_only);
  }

  void standard_generator (const std::string& name) {
    //add generator default constraints
    teal::dictionary::put (name + "_min_word_delay", "1", teal::dictionary::default_only);
    teal::dictionary::put (name + "_max_word_delay", "1", teal::dictionary::default_only);
  }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::randomize () {
  log_ << teal_info << "uart_test randomize() begin " << teal::endm;
  truss_assert (number_of_uarts >= 2);


  //First, for the  main point of the test.....
  static teal::uint32 min_uart_index = teal::dictionary::find (name_ + "_min_uart_index", 0);
  static teal::uint32 max_uart_index = teal::dictionary::find (name_ + "_max_uart_index", (number_of_uarts - 1));
  teal::uint32 selected = get_uart_index (min_uart_index, max_uart_index);
  log_ << teal_info << "Selected Test component index of " << selected <<  teal::endm;
  std::ostringstream o; o << selected; 
  std::string id = o.str();

  standard_configuration (testbench_->uart_interface[selected]->uart_configuration->name);

  uart_test_component_ingress_ = new uart::basic_test_component ("uart_test_component_ingress " + id, 
								 testbench_->uart_interface[selected]->uart_ingress_generator,  testbench_->uart_interface[selected]->uart_program_sfm, 
								 testbench_->uart_interface[selected]->uart_ingress_checker);
  standard_generator (testbench_->uart_interface[selected]->uart_ingress_generator->name);
  uart_test_component_ingress_->randomize ();


  uart_test_component_egress_  = new uart::basic_test_component ("uart_test_component_egress " + id, 
								 testbench_->uart_interface[selected]->uart_egress_generator, testbench_->uart_interface[selected]->uart_protocol_bfm, 
								 testbench_->uart_interface[selected]->uart_egress_checker);
  standard_generator (testbench_->uart_interface[selected]->uart_egress_generator->name);
  uart_test_component_egress_->randomize ();


  //now for the irritators...
  teal::uint32 count = 0;
  for (teal::uint32 i(0); i < number_of_uarts; ++i) {
    std::ostringstream o; o << i; 
    std::string id = o.str();

    truss_assert (count <= number_of_uart_irritators);
    if (i != selected) {
      uart_irritator_ingress_[count] = new uart::basic_irritator ("uart_irritator_ingress " + id, 
								       testbench_->uart_interface[i]->uart_ingress_generator,  testbench_->uart_interface[i]->uart_program_sfm, 
								       testbench_->uart_interface[i]->uart_ingress_checker);
      uart_irritator_egress_[count]  = new uart::basic_irritator ("uart_irritator_egress " + id, 
								       testbench_->uart_interface[i]->uart_egress_generator, testbench_->uart_interface[i]->uart_protocol_bfm, 
								       testbench_->uart_interface[i]->uart_egress_checker);

      standard_configuration (testbench_->uart_interface[i]->uart_configuration->name);
      standard_generator (testbench_->uart_interface[i]->uart_egress_generator->name);
      standard_generator (testbench_->uart_interface[i]->uart_ingress_generator->name);
    count++;
    }
  }

  log_ << teal_info << "uart_test randomize() end " << teal::endm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::time_zero_setup () {
  uart_test_component_egress_->time_zero_setup ();

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_egress_[i]->time_zero_setup ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::out_of_reset (reset r) {
  uart_test_component_egress_->out_of_reset (r);

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_egress_[i]->out_of_reset (r);
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::write_to_hardware () {
  uart_test_component_egress_->write_to_hardware ();
  uart_test_component_ingress_->write_to_hardware ();

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_egress_[i]->write_to_hardware ();
    uart_irritator_ingress_[i]->write_to_hardware ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::start () {
  uart_test_component_ingress_->start ();
  uart_test_component_egress_->start ();

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_ingress_[i]->start ();
    uart_irritator_egress_[i]->start ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::stop () {
  uart_test_component_ingress_->stop (); 
  uart_test_component_egress_->stop ();

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_ingress_[i]->stop (); 
    uart_irritator_egress_[i]->stop ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::wait_for_completion () {
  uart_test_component_ingress_->wait_for_completion (); 
  uart_test_component_egress_->wait_for_completion ();

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_ingress_[i]->stop_generation (); 
    uart_irritator_egress_[i]->stop_generation ();
  }
  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_ingress_[i]->wait_for_completion (); 
    uart_irritator_egress_[i]->wait_for_completion ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void quad_uart_irritators::report (const std::string prefix) const {
  uart_test_component_ingress_->report (prefix); 
  uart_test_component_egress_->report (prefix);

  for (teal::uint32 i(0); i < number_of_uart_irritators; ++i) {
    uart_irritator_ingress_[i]->report (prefix); 
    uart_irritator_egress_[i]->report (prefix);
  }
}
  
