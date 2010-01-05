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

#include "driver.h"

using namespace teal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
alu::driver::driver (const std::string& name, truss::port<driver_configuration::signals>::pins wires) :
  verification_component (name), 
  operand_a_ (wires[driver_configuration::operand_a], 32, teal::vreg::observe_and_control),
  operand_b_ (wires[driver_configuration::operand_b], 32, teal::vreg::observe_and_control),
  op_code_   (wires[driver_configuration::op_code], 8, teal::vreg::observe_and_control),
  op_valid_   (wires[driver_configuration::op_valid], 1, teal::vreg::observe_and_control),
  op_done_   (wires[driver_configuration::op_done], 1, teal::vreg::observe_only)
{  
  log_.show_debug_level (4); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alu::driver::send_operation (const operation& an_operation)
{
  log_ << teal_info << "Drive operation start" << teal::endm;
  operand_a_ = an_operation.operand_a;
  operand_b_ = an_operation.operand_b;
  op_code_   = an_operation.op_code;
  op_valid_ = 1;
  at (posedge (op_done_));
  op_valid_ = 0;
  at (negedge (op_done_));
  log_ << teal_info << "Drive operation done" << teal::endm;
}

void alu::driver::time_zero_setup ()                       {log_ << teal_debug << " time_zero_setup" << teal::endm;}
void alu::driver::out_of_reset (reset)                     {log_ << teal_debug << " out of reset" << teal::endm;}
void alu::driver::start ()                                 {log_ << teal_debug << " start " << teal::endm;};
void alu::driver::stop ()                                  {log_ << teal_debug << " stop" << teal::endm;};
void alu::driver::randomize ()                             {log_ << teal_debug << " randomize" << teal::endm;}
void alu::driver::write_to_hardware ()                     {log_ << teal_debug << " write to hardware" << teal::endm;}
void alu::driver::wait_for_completion ()                   {log_ << teal_debug << " wait_for_completion" << teal::endm;}
void alu::driver::report (const std::string prefix) const {log_ << teal_debug << prefix << " report" << teal::endm;}

