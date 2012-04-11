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

#if 0
./run -c -$SIM -clean -t memory_test
#endif

#include "teal.h"

using namespace teal;

///////////////////////////////////////////////
///////////////////////////////////////////////
void verification_top ()
{
  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  //  reg a_bit;
  reg a_uint32 (0,32);
    vreg go ("top.run_test");
  // reg a_uint64 (77);
  vout log ("reg_test");

  at (posedge (go, 7));
  for (uint32 i(0); i < 100; ++i) {
    a_uint32 = i;
    std::istringstream foo (a_uint32.format_hex_string ().c_str());
    uint32 check (88);
    uint32 size;
    char foo1, foo2;
    foo >> std::hex >> size >> foo1 >> foo2 >> check;
    log << (((check == i) && (a_uint32.to_int() == i) ) ? teal_info : teal_error)
      << "Base of " << i << " = " << a_uint32 << " check " << check 
      << " to int " << a_uint32.to_int() << endm;

    log << (((a_uint32 >> 1) == reg (i >> 1)) ? teal_info : teal_error)
	<< "at index of " << i << " = " << (a_uint32 >> 1)  << " check " << (i >> 1)
      << " original " << a_uint32 << endm;
  }

  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }
  finish();
}

#include "../teal_hdl_connect.cpp"
