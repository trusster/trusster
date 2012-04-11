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

#include "testbench.h"

#include "truss.h"

#include "uart_bfm_agent.h"
#include "uart_16550_agent.h"
#include "uart_generator_agent.h"
#include "uart_checker_agent.h"
#include "uart_16550_configuration.h"

#include "wishbone_driver.h"
#include "wishbone_memory_bank.h"


#define uart_registers_first 0
#define uart_registers_last 7

using namespace truss;

const teal::uint64 UART_CLOCK_FREQUENCY =  29489826; //29.4Mhz

testbench::testbench (const std::string top_path) : truss::testbench_base ("testbench"), top_ (top_path)
{
  log_ << teal_debug << "testbench new() begin " << teal::endm;

  //build the channels of the bi-directional interface
  uart::channel* program_egress = new uart::channel ("program egress");
  uart::channel* program_egress_tap = new uart::channel ("protocol egress tap");
  program_egress->add_listner (program_egress_tap);

  uart::channel* program_ingress = new uart::channel ("program ingress");

  uart::channel* protocol_ingress = new uart::channel ("protocol ingress");
  uart::channel* protocol_ingress_tap = new uart::channel ("protocol ingress tap");
  protocol_ingress->add_listner (protocol_ingress_tap);

  uart::channel* protocol_egress = new uart::channel ("protocol egress");

  //build the configuration of the interface
  uart_configuration = new uart::configuration_16550 ("Program");

  //build the ports of the interface
  port <uart::configuration_16550::signals>::pins  program_port;
  program_port[uart::configuration_16550::reference_clock] = top_ + ".wb_clock"; 
  program_port[uart::configuration_16550::interrupt] = top_ + ".int_o"; 


  port <uart::configuration::signals>::pins protocol_port;
  protocol_port[uart::configuration::baud_rate_clock] = top_ + ".BAUD_RATE_CLOCK"; 
  protocol_port[uart::configuration::dsr] = top_ + ".DSR";
  protocol_port[uart::configuration::dtr] = top_ + ".DTR";
  protocol_port[uart::configuration::cts] = top_ + ".RTS";
  protocol_port[uart::configuration::rts] = top_ + ".CTS";
  protocol_port[uart::configuration::rx] = top_ + ".TX";
  protocol_port[uart::configuration::tx] = top_ + ".RX";

  //build the connection layer of the interface
  uart_protocol_bfm = new uart::bfm_agent           ("uart Protocol", protocol_port, uart_configuration, protocol_ingress, protocol_egress, UART_CLOCK_FREQUENCY);
  uart_program_sfm   = new uart::uart_16550_agent ("16550 uart",     program_port,  uart_configuration, program_egress,   program_ingress,  UART_CLOCK_FREQUENCY);

  //build and hookup the ingress and egress stimulus and scoreboards of the interface
  uart_egress_generator = new uart::generator_agent ("egress_generator", program_egress, &uart_configuration->data_size_);
  uart_egress_checker = new uart::checker_agent ("egress checker", program_egress_tap, protocol_egress);

  uart_ingress_generator = new uart::generator_agent ("ingress_generator", protocol_ingress, &uart_configuration->data_size_);
  uart_ingress_checker = new uart::checker_agent ("ingress checker", protocol_ingress_tap, program_ingress);

  //Now for the main chip register interface
  port <wishbone::configuration::signals>::pins  wishbone_port;
  wishbone_port [wishbone::configuration::reset] = top_ + ".wb_rst_ir";
  wishbone_port [wishbone::configuration::clock] = top_ + ".wb_clockr";
  wishbone_port [wishbone::configuration::address] = top_ + ".wishbone_driver.address";
  wishbone_port [wishbone::configuration::data] = top_ + ".wishbone_driver.data";
  wishbone_port [wishbone::configuration::select] = top_ + ".wishbone_driver.select";
  wishbone_port [wishbone::configuration::op_code] = top_ + ".wishbone_driver.op_code";
  wishbone_port [wishbone::configuration::do_work] = top_ + ".wishbone_driver.do_work";
  wishbone_port [wishbone::configuration::work_done] = top_ + ".wishbone_driver.work_done";

  wishbone_driver_ = new wishbone::wishbone_driver ("WB", wishbone_port);
  teal::memory::add_memory_bank (new wishbone::wishbone_memory_bank ("Wishbone", "main_bus", wishbone_driver_));
  teal::memory::add_map ("main_bus", uart_registers_first, uart_registers_last);

  log_ << teal_debug << "testbench new() done " << teal::endm;
}

testbench::~testbench () {
  delete uart_configuration;
  delete uart_protocol_bfm;
  delete uart_ingress_generator;
  delete uart_ingress_checker;
  delete uart_program_sfm;
  delete uart_egress_generator;
  delete uart_egress_checker;
  delete wishbone_driver_;
}

void testbench::time_zero_setup () {
  //  wishbone_driver_->reset = 0;
  //arrgh that multiple driver issue systemc,put pulldown on testbenvh.v
};

void testbench::out_of_reset (reset r) {
  wishbone_driver_->pause (10);
  log_ << teal_info << "begin set reset to 1" << teal::endm;
  wishbone_driver_->reset = 1;
  wishbone_driver_->pause (10);
  log_ << teal_info << "begin set reset to 0" << teal::endm;
  wishbone_driver_->reset = 0;
  //    uart->out_of_reset (r);  //what if not using a uart? or, worse, the dut's a stub?
}

void testbench::randomize () {
  uart_configuration->randomize ();
  //need to start it here because write_to_hardware of uart 16550 will use the driver
  wishbone_driver_->start ();

}

void testbench::write_to_hardware () {
}

void testbench::start () {
}

void testbench::stop () {}

//no wait_for_completion, let the test/exercisors do that
void testbench::wait_for_completion () {}

void testbench::report (const std::string prefix) const {
  uart_configuration->report (prefix);
}

