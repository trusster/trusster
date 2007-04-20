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



///////////////////////////////////////////////
///////////////////////////////////////////////
void verification_top ()
{
  vout log ("vreg_test");
  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  vreg init_done ("top.init_done");
  at (posedge (init_done));

  vreg top_level ("top.a_top_vreg");  
  vreg sub_level ("top.a_sub_module.a_sub_vreg");
  reg start_sub_level = 0x234567891200ULL;
  vreg a_wire ("top.a_wire");
  
  log << ((top_level == 0) ? teal_info : teal_error )
      << " top level is " << hex << top_level << endm;
  log << ((sub_level == start_sub_level) ? teal_info : teal_error )
      << " sub level is " << hex << sub_level << endm;

  log << ((vreg::present ("a_bogus_path") == 0) ? teal_info : teal_error )
      << " no signal named \"a_bogus_path\"" << hex << top_level << endm;

  log << ((vreg::present ("top.sub1.sub2.a_good_path")) ? teal_info : teal_error )
      << " found signal named \"top.sub1.sub2.a_good_path\"" << hex << top_level << endm;

  top_level = 3;
  sub_level = 0x12345;
  reg expected_wire (0, 6);

  for (uint32 ii (2); ii < 6; ++ii) {
    expected_wire(ii,ii) = reg (reg::Z);
  }
  expected_wire (0,0) = 1;
  expected_wire (1,1) = 0;
  reg initial_info_wire (expected_wire);

  log << ((triple_equal (a_wire, expected_wire) == reg::one) ? teal_info : teal_error)
      << " initial a_wire is " << bin << a_wire << " expected " << expected_wire << endm;
  log << ((top_level == 3) ? teal_info : teal_error)
    << " after set top level is " << hex << top_level.format_hex_string () << endm;
  log << ((sub_level == 0x12345) ? teal_info : teal_error)
    << " after set sub level is " << hex << sub_level.format_hex_string () << endm;


  //wait on a clk just for fun, like driving wires synched to a clock
    vreg clk ("top.clk");
    at (posedge (clk));

  a_wire(0,0) = reg (reg::zero);
  a_wire(1,1) = reg (reg::one);
  expected_wire (0,0) = reg (reg::zero);
  expected_wire (1,1) = reg (reg::one);
    at (posedge (clk));
  log << ((triple_equal (a_wire, expected_wire) == reg::one) ? teal_info : teal_error)
    << " invert pull values a_wire is " << bin << a_wire << endm;

  a_wire(0,0) = reg (reg::Z);
  a_wire(1,1) = reg (reg::X);
  expected_wire (0,0) = reg (reg::Z);
  expected_wire (1,1) = reg (reg::X);
  at (posedge (clk));
  log << ((triple_equal (a_wire, expected_wire) == reg::one) ? teal_info : teal_error)
    << " X/Z pull values a_wire is " << bin << a_wire << endm;

  at (posedge (clk));
  a_wire.release ();
  at (posedge (clk));

#if !defined (ivl)
  log << ((triple_equal (a_wire, initial_info_wire) == reg::one) ? teal_info : teal_error)
      << " release to initial value a_wire is " << bin << a_wire << " expected: " << initial_info_wire << endm;
#endif

  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }
  finish();
}

#include "../teal_hdl_connect.cpp"
