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
/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
from: http://www.ics.uci.edu/~eppstein/projects/pairs/Source/testbed/rand48

I've mixed classic erand48 with Bjarne Stoustrup's rand class (see sect 22.7, p 685, The C++ programming language)
CONSIDER: MD5 implementation
NOTE: some systems come with an erand48, but not all ;-)
 */

#include "teal.h"

#include "time.h"
#include <iostream>
#include <fstream>

unsigned short teal::vrandom::master_seed_[3] = {0,0,0};

#include <math.h>
#include <stdlib.h>

#define RAND48_SEED_0   (0x330e)
#define RAND48_SEED_1   (0xabcd)
#define RAND48_SEED_2   (0x1234)
#define RAND48_MULT_0   (0xe66d)
#define RAND48_MULT_1   (0xdeec)
#define RAND48_MULT_2   (0x0005)
#define RAND48_ADD      (0x000b)

unsigned short _rand48_seed[3] = {
        RAND48_SEED_0,
        RAND48_SEED_1,
        RAND48_SEED_2
};
unsigned short _rand48_mult[3] = {
        RAND48_MULT_0,
        RAND48_MULT_1,
        RAND48_MULT_2
};
unsigned short _rand48_add = RAND48_ADD;

static void _dorand48(unsigned short xseed[3])
{
        unsigned long accu;
        unsigned short temp[2];

        accu = (unsigned long) _rand48_mult[0] * (unsigned long) xseed[0] +
         (unsigned long) _rand48_add;
        temp[0] = (unsigned short) accu;        /* lower 16 bits */
        accu >>= sizeof(unsigned short) * 8;
        accu += (unsigned long) _rand48_mult[0] * (unsigned long) xseed[1] +
         (unsigned long) _rand48_mult[1] * (unsigned long) xseed[0];
        temp[1] = (unsigned short) accu;        /* middle 16 bits */
        accu >>= sizeof(unsigned short) * 8;
        accu += _rand48_mult[0] * xseed[2] + _rand48_mult[1] * xseed[1] + _rand48_mult[2] * xseed[0];
        xseed[0] = temp[0];
        xseed[1] = temp[1];
        xseed[2] = (unsigned short) accu;
}

static double internal_erand48(unsigned short xseed[3])
{
        _dorand48(xseed);
        return ldexp((double) xseed[0], -48) +
               ldexp((double) xseed[1], -32) +
               ldexp((double) xseed[2], -16);
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
void teal::vrandom::init_with_file (const std::string& p)
{
  std::ifstream f(p.c_str());
  bool found = false;

  while (f.good() && (! f.eof())) {
    std::string dummy; f >> dummy;
    if (dummy == "master_seed") {
      found = true;
      f >> master_seed_[0] >> master_seed_[1] >> master_seed_[2];
    }
  }

  if (! found) {
    time_t foo;
    time (&foo);
    //    srand (clock ());
    srand ((long)foo);
    master_seed_[0] = rand (); master_seed_[1] = rand (); master_seed_[2] = rand ();
    std::ofstream f2 (p.c_str(), std::ios::app);
    f2 << "master_seed " << master_seed_[0] << " " << master_seed_[1] << " " << master_seed_[2] << std::endl;
  }
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
void teal::vrandom::init_with_seed (uint64 m)
{
  m *=  1103515245; //generate big difference from close to same seed
  master_seed_[0] = m;
  master_seed_[1] = (m >> 8); 
  master_seed_[2] = (m >> 16);

  master_seed_[0] ^= (m >> 24);
  master_seed_[1] ^= (m >> 32); 
  master_seed_[2] ^= (m >> 40);

  master_seed_[0] ^= (m >> 48);
  master_seed_[1] ^= (m >> 56); 
}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
teal::vrandom::vrandom (const std::string& p, uint32 l)
{
  //If this assert triggers, you have probably forgot to init_with-seed(). This is bad, as multiple runs will not have different random values.
  //Note that this means that a zero seed is not allowed
  assert (master_seed_[0]  || master_seed_[1] || master_seed_[2]);
  seed_[0] = master_seed_[0];
  seed_[1] = master_seed_[1];
  seed_[2] = master_seed_[2];
  
  //first, fold in the string part
  for (uint32 i(0); i < p.length (); ++i) {
    //CONSIDER: some ascii function like ! is_printable ()
    if (! isalnum (p[i])) continue;  //skip the non-meat
    seed_[i % 3] ^= p[i];
  }
  //now the integer part. Seperate close integers by mult with a big prime
  l *= 1103515245;
  seed_[0] ^= l & 0xFF;
  seed_[1] ^= (l >> 8) & 0xFF; 
  seed_[2] ^= (l >> 16);

#if 0
  //add this when all of teal is in
  //now the thread name so that same instances in multiple threads are unique
  std::string t (teal::thread_name (pthread_self ()));
  for (uint32 j(0); j < t.length (); ++j) {
    //CONSIDER: some ascii function like ! is_printable ()
    if (! isalnum (t[j])) continue;  //skip the non-meat
    seed_[j % 3] ^= t[j];
  }
#endif
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
double teal::vrandom::draw ()
{
  return (internal_erand48 (&seed_[0]));
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
teal::uint64 teal::random_range::draw (teal::uint64 low, teal::uint64 up)
{
  if (low == up) return low;

  teal::uint64 lower ((low < up) ? low : up);
  teal::uint64 upper ((low < up) ? up : low);
  teal::uint64 delta (up - low + 1);
  teal::uint64 returned = (teal::uint64) (lower + (vrandom::draw () * (double)delta));
  if (returned > upper) {
    returned = upper;
  }
  return returned;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
teal::random_range::random_range (const std::string& p, uint32 l) :  
  vrandom (p,l) 
{}


