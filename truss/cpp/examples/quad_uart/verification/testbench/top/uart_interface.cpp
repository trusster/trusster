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

#include "uart_interface.h"

#include "uart_bfm_agent.h"
#include "uart_16550_agent.h"
#include "uart_generator_agent.h"
#include "uart_checker_agent.h"
#include "uart_16550_configuration.h"

#include "truss.h"

const teal::uint64 UART_CLOCK_FREQUENCY =  29489826; //29.4Mhz


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::interface::interface (const std::string& top, teal::uint32 index)
{
  std::ostringstream o; o << index;
  std::string id = o.str();

  //build the channels of the bi-directional interface
  uart::channel* program_egress = new uart::channel ("program egress_" + id);
  uart::channel* program_egress_tap = new uart::channel ("protocol egress tap_" + id);
  program_egress->add_listner (program_egress_tap);

  uart::channel* program_ingress = new uart::channel ("program ingress_" + id);

  uart::channel* protocol_ingress = new uart::channel ("protocol ingress_" + id);
  uart::channel* protocol_ingress_tap = new uart::channel ("protocol ingress tap_" + id);
  protocol_ingress->add_listner (protocol_ingress_tap);

  uart::channel* protocol_egress = new uart::channel ("protocol egress_" + id);

  //build the configuration of the interface
  uart_configuration = new uart::configuration_16550 ("Configuration_" + id);


  //build the ports of the interface
  truss::port <uart::configuration_16550::signals>::pins  program_port;
  program_port[uart::configuration_16550::reference_clock] = top + ".wb_clock"; 
  program_port[uart::configuration_16550::interrupt] = top + ".uart_int_" + id; 


  truss::port <uart::configuration::signals>::pins protocol_port;
  protocol_port[uart::configuration::baud_rate_clock] = top + ".BAUD_RATE_CLOCK"; 
  protocol_port[uart::configuration::dsr] = top + ".uart_dsr_" + id;
  protocol_port[uart::configuration::dtr] = top + ".uart_dtr_" + id;
  protocol_port[uart::configuration::cts] = top + ".uart_rts_" + id;
  protocol_port[uart::configuration::rts] = top + ".uart_cts_" + id;
  protocol_port[uart::configuration::rx] = top + ".uart_tx_" + id;
  protocol_port[uart::configuration::tx] = top + ".uart_rx_" + id;

  //build the connection layer of the interface
  uart_protocol_bfm = new uart::bfm_agent           ("uart_Protocol_" + id, protocol_port, uart_configuration, protocol_ingress, protocol_egress, UART_CLOCK_FREQUENCY);
  uart_program_sfm   = new uart::uart_16550_agent ("16550_uart_" + id, index, program_port,  uart_configuration, program_egress,   program_ingress,  UART_CLOCK_FREQUENCY);

  //build and hookup the ingress and egress stimulus and scoreboards of the interface
  uart_egress_generator = new uart::generator_agent ("egress_generator_" + id, program_egress, &uart_configuration->data_size_);
  uart_egress_checker = new uart::checker_agent ("egress_checker_" + id, program_egress_tap, protocol_egress);

  uart_ingress_generator = new uart::generator_agent ("ingress_generator_" + id, protocol_ingress, &uart_configuration->data_size_);
  uart_ingress_checker = new uart::checker_agent ("ingress_checker_" + id, protocol_ingress_tap, program_ingress);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uart::interface::~interface ()
{
  delete uart_configuration;
  delete uart_protocol_bfm;
  delete uart_ingress_generator;
  delete uart_ingress_checker;
  delete uart_program_sfm;
  delete uart_egress_generator;
  delete uart_egress_checker;
}
