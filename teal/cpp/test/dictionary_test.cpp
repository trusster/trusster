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

//#include <utility>

#define vout_predicate(c,val) \
  c << ((val) ? teal_info : teal_error)


///////////////////////////////////////////////
///////////////////////////////////////////////
void verification_top ()
{
  dictionary::read ("dictionary.txt");

  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  vreg go ("top.run_test");
  at (posedge (go, 2));

  vout log ("dictionary_test");

  //The find's can appear in any order, endemendent of where/how they appear 
  //in the dictionary text file.

  std::string a_string (dictionary::find ("a_string"));
  log << ((a_string == "Hello_World!") ? teal_info : teal_error)
     << " string_value: expected \"Hello_World!\", received \"" << a_string << "\"" << teal::endm;

  std::string b_string (dictionary::find ("out_file"));
  vout_predicate (log, (b_string == "dictionary_test_results.txt"))
     << " string_value: expected \"dictionary_test_results.txt\", received \"" << b_string << "\"" << teal::endm;

  double a_double (dictionary::find ("a_double", 0.0));
  vout_predicate (log, a_double == 1.22345) 
    << " a_double: expected 1.22345, received " << a_double << teal::endm;

  uint32 version (dictionary::find ("version", (uint32)99));
  vout_predicate (log, version == 1)
    << " version: expected 1, received " << version << teal::endm;

  bool found = dictionary::put ("not_present", "a_value");
  std::string a_string2 (dictionary::find ("not_present"));
  log << ((!found) ? teal_info : teal_error)
     << " string_value: not found as expected." << teal::endm;
  log << ((a_string2 == "a_value") ? teal_info : teal_error)
     << " string_value: expected \"a_value\", received \"" << a_string2 << "\"" << teal::endm;

  bool found2 = dictionary::put ("to_be_overridden", "another_value", true);
  std::string a_string3 (dictionary::find ("to_be_overridden"));
  log << ((found2) ? teal_info : teal_error)
     << " string_value: \"to_be_overriden\" found." << teal::endm;
  log << ((a_string3 == "another_value") ? teal_info : teal_error)
     << " string_value: expected \"another_value\", received \"" << a_string3 << "\"" << teal::endm;

  uint32 a (0);
  uint32 b (0);
  std::istringstream file (dictionary::find ("two_ints"));
  file >> a >> b;
  vout_predicate (log, (a == 45) && (b == 8890))
    << "a or b: expected 45 and 8890, received " << teal::dec << a << " and " << b << teal::endm;

  uint64 hex_value (0xaaa);
  std::istringstream file3 (dictionary::find ("hex_value"));
  //really, should be older gcc
#if defined (vcs)
  vout_predicate (log, dictionary::find ("hex_value") == "0xabcdef0") 
    << " hex_value: expected 0xabcdef0, received " << teal::hex << hex_value << teal::endm;
#else
  file3 >> std::hex >> hex_value;
  vout_predicate (log, hex_value == 0xabcdef0) 
    << " hex_value: expected 0xabcdef0, received " << teal::hex << hex_value << teal::endm;
#endif

  dictionary::clear ();
  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }
  finish ();
}


#include "../teal_hdl_connect.cpp"
