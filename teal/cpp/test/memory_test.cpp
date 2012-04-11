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

void verification_top ()
{
  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  vreg init_done ("top.init_done");
  at (posedge (init_done));

  vout log ("memory_test");
  log.message_display (vlog::thread_name, false);

  memory::add_map ("top.a_top_bank", 0x44000, 0x44000 + 1717); //specify full path
  memory::add_map ("top.a_sub_module", 0x48000, 0x48000 + 312);  //let system add bank0

  reg sub_data (0, 123);
  reg top_data (0,18);

  memory::read (0x44000 + 44, &top_data);
  log << ((top_data == 44) ? teal_info : teal_error) <<
    "Initial  top level[44] value is "  << top_data << endm;
  vreg clk ("top.clk");
  //    at (posedge (clk));

  memory::read (0x48000 + 175, &sub_data);
  log << ((sub_data == 175) ? teal_info : teal_error) << " Initial sub value[175] is " << sub_data.format_hex_string () << endm;


  memory::write (0x44000 + 44, 0xF2345); //test bit clipping
  memory::write (0x48000 + 175, 0x8988);   //test bit expansion

  memory::read (0x44000 + 44, &top_data);
  log << ((top_data == 0x32345) ? teal_info : teal_error) << " after set top level[44] value is " << hex << top_data  << endm;

  memory::read (0x48000 + 175, &sub_data);
  if (sub_data != 0x8988) {
    log << teal_error << " after set sub level value[175] is " << hex << sub_data << endm;
  }
  else {
    log << teal_info << " after set sub level value[175] is " << hex << sub_data << endm;
  }

  //now the direct way...
  memory::memory_bank* top_level = teal::memory::lookup ("top.a_top_bank");  
  memory::memory_bank* sub_level = teal::memory::lookup ("top.a_sub_module");
  
  top_level->from_memory (1717, &top_data);
  log << ((top_data == 1717) ? teal_info : teal_error) << " Initial setting. top level[1717] value is " << hex << top_data << endm;

  sub_level->from_memory (312, &sub_data);
  log << ((sub_data == 312) ? teal_info : teal_error) <<  " Initial setting. sub level value[312] is " << hex << sub_data << endm;

  top_level->to_memory (1717,0x3);
  sub_level->to_memory (312, 0x3232888);

  top_level->from_memory (1717, &top_data);
  log << ((top_data == 3) ? teal_info : teal_error) << " top level[1717] value is " << hex << top_data << endm;

  sub_level->from_memory (312, &sub_data);
  if (sub_data != 0x3232888) {
    log << teal_error << " sub level value[312] is " << hex << sub_data << endm;
  }
  else {
    log << teal_info << " sub level value[312] is " << hex << sub_data << endm;
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
