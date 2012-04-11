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
  uint32 get_operand_a (uint32 min_v, uint32 max_v) {
    uint32 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }

  uint32 get_operand_b (uint32 min_v, uint32 max_v) {
    uint32 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }

  uint8 get_op_code (uint8 min_v, uint8 max_v) {
    uint8 returned;
    RAND_RANGE (returned, min_v, max_v); 
    return returned;
  }
}



#include "generator.h"

#include <vector>


void alu::generator::do_operations (teal::uint32 count)
{
  truss_assert (count);
  std::vector<operation> operations;
  for (unsigned int i(0); i < count; ++i) {
    log_ << teal_info << "gen a rand operation " << i << " out of " << count << teal::endm;
    static uint32 min_operand_a = dictionary::find (name + "_min_operand_a", 0x032862);
    static uint32 max_operand_a = dictionary::find (name + "_max_operand_a", 0x41062);
    uint32 operand_a = get_operand_a (min_operand_a, max_operand_a);

    static uint32 min_operand_b = dictionary::find (name + "_min_operand_b", 0x033099);
    static uint32 max_operand_b = dictionary::find (name + "_max_operand_b", 0x62896);
    uint32 operand_b = get_operand_b (min_operand_b, max_operand_b);

    static uint8 min_op_code = dictionary::find (name + "_min_op_code", 0);
    static uint8 max_op_code = dictionary::find (name + "_max_op_code", 3);
    uint8 op_code = get_op_code (min_op_code, max_op_code);

    operations.push_back (operation (operand_a, operand_b, op_code));
  }
  do_operations_ (operations);
}
