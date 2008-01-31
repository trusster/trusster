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
#include <fstream>
#include <math.h>

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
void verification_top ()
{
  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  //  reg a_bit;
  reg a_uint32 (0, 32);
  vreg go ("top.run_test");
  // reg a_uint64 (77);
  vout log ("vrandom_test");
  at (posedge (go,7));

  //in production, choose one of the two
  vrandom::init_with_file ("dictionary.txt"); 
  {
    //    std::istringstream file (dictionary::find ("seed"));
    //    uint64 seed (dictionary::find ("seed"));
    //    std::istringstream file (dictionary::find ("two_ints"));
    //    file >> a >> b;
    vrandom::init_with_seed (dictionary::find ("seed", 0));
  }
  std::map<uint8,uint32> histogram_8;
  std::map<uint32, uint32> histogram_range;
  std::map<uint32, uint32> histogram_32;

  uint32 x;

  for (uint32 i(0); i < 20000; ++i) {
    RAND_8 (x);
    ++histogram_8[x];
    RAND_RANGE (x, 33, 12345);
    ++histogram_range[x];
    RAND_32 (x);
    ++histogram_32[x];
  }
  std::ofstream foo ("data.txt");
  for (uint32 i(0); (i < 256); ++i) {
    //    foo << "histogram[" << i << "]: " << histogram_8[i] << std::endl;
    foo << histogram_8[i] << std::endl;
  }

  //check that distribution is normal
  //compute standard distribution and verify that rand is < 1 sigma
  uint64 mean(0);
  uint64 mean_of_squares (0);
  for (uint32 i(0); i < 256; ++i) {
    mean += histogram_8[i];
    mean_of_squares += histogram_8[i] * histogram_8[i];
  }
  mean /= 256;
  mean_of_squares /= 256;
  double standard_deviation = sqrt (abs_f ((double) mean_of_squares - (mean*mean)));
  std::cout << "mean " << mean << " mean of squares " << mean_of_squares << 
    " standard deviation: " << standard_deviation <<  std::endl;

  //  uint32 num_below_mean (0);
  uint32 num_within_one (0);
  uint32 num_within_two (0);
  for (uint32 i(0); i < 256; ++i) {
    if (abs_f ((double)histogram_8[i] - (double)mean) <= standard_deviation) ++num_within_one;
    if (abs_f ((double)histogram_8[i] - (double)mean) <= (2 *standard_deviation)) ++num_within_two;
  }

  log << ((num_within_one >= (.68 * 256)) ?  teal_info : teal_error) << 
    num_within_one << " were within one standard deviation. Expected at least: " << (.68 * 256) << endm;

  log << ((num_within_two >= (.95 * 256)) ? teal_info : teal_error) <<
    num_within_two << " were within one standard deviation. Expected at least: " << (.95 * 256) << endm;

  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }
  finish();
}


#include "../teal_hdl_connect.cpp"
