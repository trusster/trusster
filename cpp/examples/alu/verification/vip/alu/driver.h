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
#ifndef __alu_driver__
#define __alu_driver__

#include "teal.h"
#include "truss.h"


namespace alu {
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  struct operation {
    operation (teal::uint32 a, teal::uint32 b, teal::uint8 code) : operand_a (a), operand_b (b), op_code (code) {}
    teal::uint32 operand_a;
    teal::uint32 operand_b;
    teal::uint8 op_code;
  };

  inline teal::vout& operator<< (teal::vout& v, const operation& op) {
    switch (op.op_code) {
    case 0: v << op.operand_a << " + " << op.operand_b ; return v;
    case 1: v << op.operand_a << " - " << op.operand_b ; return v;
    case 2: v << op.operand_a << " & " << op.operand_b ; return v;
    case 3: v << op.operand_a << " | " << op.operand_b ; return v;
    default: v << "unknown op code of " << op.op_code << " on operands " << op.operand_a << " and " << op.operand_b; return v;
    }
    return v;
  };


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  struct driver_configuration {
    typedef enum {operand_a, operand_b, op_code, op_valid, op_done} signals;    
  };

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class driver : public truss::verification_component {
  public:
    driver (const std::string& name, truss::port<driver_configuration::signals>::pins wires);
    virtual ~driver () {}

    virtual void time_zero_setup ();
    virtual void out_of_reset (reset);
    virtual void randomize ();
    virtual void start ();
    virtual void stop ();
    virtual void write_to_hardware ();
    virtual void wait_for_completion ();
    virtual void report (const std::string prefix) const;

    void send_operation (const operation&);

  private:
    driver (const driver&);
    driver& operator= (const driver&);

    teal::vreg operand_a_;
    teal::vreg operand_b_;
    teal::vreg op_code_;
    teal::vreg op_valid_;
    teal::vreg op_done_;
  };
};
#endif
