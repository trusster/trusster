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
#include <fstream>

using namespace teal;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
template <class data_t>
data_t abs_f (data_t lhs) 
{
  return ((lhs < 0) ? -lhs : lhs);
}


///////////////////////////////////////////////
///////////////////////////////////////////////
char nibble_to_hex (uint8 a_nibble)
{
  switch (a_nibble) {
  case 0: return '0';
  case 1: return '1';
  case 2: return '2';
  case 3: return '3';
  case 4: return '4';
  case 5: return '5';
  case 6: return '6';
  case 7: return '7';
  case 8: return '8';
  case 9: return '9';
  case 10: return 'a';
  case 11: return 'b';
  case 12: return 'c';
  case 13: return 'd';
  case 14: return 'e';
  case 15: return 'f';
  }
  return '?';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void verification_top ()
{
  vreg go ("top.run_test");
  vout log ("trandom_test");
  at (posedge (go, 7));

  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  
  log << teal_info << "Hello World" << endm;
  log << teal_info << "hex of " << dec << "\'" << 'h' << "\'" << hex << " is " << 'h' << endm;

  char a_char = 0x33;
  log << teal_info << "hex of " << dec << a_char << " is " << hex << " " << a_char << endm;

  log << teal_error << "hex of " << dec << -234 << " is " << hex << -234 << endm;
  log << teal_info <<  "hex of " << dec << (int)234<< hex << " is " << (int)234 << endm;

  uint32 binary_loop (dictionary::find ("binary_loop", (uint32)50));
  for (uint32 binary_test (0); (binary_test < binary_loop); ++binary_test) {
    std::string correct = "                                                                   ";
    std::string correct_hex = "GGGGGGGGGGGGGGGGG";
    reg xz_test (0, 67);
    bool saw_x (false);
    bool saw_z (false);
    for (uint32 i = 0; (i <= 66); ++i) {
      uint32 x_threshold; RAND_RANGE (x_threshold, 0, 99);
      static uint32 x_threshold_limit (dictionary::find ("x_threshold", (uint32)5));
      uint32 z_threshold; RAND_RANGE (z_threshold, 0, 99);
      static uint32 z_threshold_limit (dictionary::find ("z_threshold", (uint32)5));
      uint32 one_threshold; RAND_RANGE (one_threshold, 0, 99);
      static uint32 one_threshold_limit (dictionary::find ("one_threshold", (uint32)50));

      if (x_threshold <= x_threshold_limit) {
	correct[66-i] = 'x';
	xz_test (i,i) = reg (reg::X);
	saw_x = true;
      }
      else if (z_threshold <= z_threshold_limit) {
	correct[66-i] = 'z';
	xz_test (i,i) = reg (reg::Z);
	saw_z = true;
      }
      else {
	correct[66-i] = (one_threshold <= one_threshold_limit) ? '1' : '0';
	xz_test(i,i) = (one_threshold <= one_threshold_limit) ? reg (reg::one) : reg (reg::zero);
      }
      if ((i % 4) == 3) {
	if (saw_x) {
	  correct_hex[66/4-(i/4)] = 'X';
	}
	else if (saw_z) {
	  correct_hex[66/4-(i/4)] = 'Z';
	}
	else {
	  uint8 a_nibble (((reg)(xz_test (i, i-3))).to_int ());
	  correct_hex[17-((i+1)/4)] = nibble_to_hex (a_nibble);;
	}
	saw_x = false;
	saw_z =- false;
      }
    }
    if (saw_x) {
      correct_hex[0] = 'X';
    }
    else if (saw_z) {
      correct_hex[0] = 'Z';
    }
    else {
      uint8 a_nibble ((reg(xz_test (66, 64))).to_int ());
      correct_hex[0] = nibble_to_hex (a_nibble);
    }

    std::string current (xz_test.format_binary_string ());
    log << ((current == ("67'b" + correct)) ? teal_info : teal_error)
	 << " binary check (actual): " << bin << xz_test << " =?=  (actual) 67'b" << correct << endm;

    std::string current_hex (xz_test.format_hex_string ());
    log << ((current_hex == ("67'h" + correct_hex)) ? teal_info : teal_error)
	<< " hex check (actual): " << hex << xz_test << " =?= (expected)  67'h" << correct_hex << endm;
  }
  //Test some debug logging
  log.show_debug_level (teal::no_debug);
  log << teal_debug << teal_error << "Should not be displayed." << endm;
  log << teal_debug << level (5) << teal_error << "Level 5 Should not be displayed." << endm;

  log.show_debug_level (teal::debug);
  log << teal_debug << "A debug message." << endm;

  log.show_debug_level (4);
  log << teal_debug << level (5) << teal_error << "Level 5 Should not be displayed." << endm;

  log.show_debug_level (5);
  log << teal_debug << level (5) << teal_info << "A \"Level 5\" debug message." << endm;

  //normal debug should revert to the default, which is one
  log.show_debug_level (0);
  log << teal_debug << teal_error << "Should not be displayed." << endm;

  if ( (vlog::get().how_many (vlog::error) != 1) 
       || (vlog::get().how_many (vlog::debug) != 2)  ) {
    log << "Test Failed: Contained " << dec 
	<< vlog::get().how_many (vlog::error) << " error and " 
	<< vlog::get().how_many (vlog::debug) << " debug messages." << endm;
  }
  else {
    log << "Test Passed: Contained " << dec 
	<< vlog::get().how_many (vlog::error) << " error and " 
	<< vlog::get().how_many (vlog::debug) << " debug messages." << endm;
  }
  finish();
}


#include "../teal_hdl_connect.cpp"
