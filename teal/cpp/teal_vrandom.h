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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h


  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////// Random /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class vrandom {
  public:
    static void init_with_file (const std::string& master_seed_path);
    static void init_with_seed (uint64 master_seed);
    //    static void init (); //uses the dictionary for master_seed=
    vrandom (const std::string& file, uint32 line);
    virtual ~vrandom () {};

    double draw ();

  private:
    unsigned short seed_[3];
    static unsigned short master_seed_[3];
  };


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //NOTE: The min,max values are not put in the ctor to allow different ranges after the 
  //object is created. This is important to allow one range,random to affect another.

  //CONSIDER: templated data type?, or at least 64? how to rand init a reg?

  class random_range : public vrandom {
  public:
    random_range (const std::string&, uint32);
    int32 draw (int32, int32); //any order
  };

#define RAND_8(x) {static teal::random_range r(__FILE__, __LINE__); x = r.draw (0,255);};
#define RAND_32(x) {static teal::vrandom r(__FILE__, __LINE__); x = (uint32)(r.draw () * (~0UL));};
#define RAND_RANGE(x,y,z) {static teal::random_range r(__FILE__, __LINE__); x = r.draw (y,z);};
