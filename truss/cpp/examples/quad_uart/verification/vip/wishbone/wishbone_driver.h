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
#ifndef __wishbone_driver__
#define __wishbone_driver__



#include "teal.h"
#include "truss.h"

namespace wishbone {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class configuration {
  public:
    typedef enum {reset= 0x3000, clock, address, data,select, op_code, do_work, work_done, } signals;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class wishbone_driver : public truss::thread {
 public:
  wishbone_driver (const std::string& n, truss::port <configuration::signals>::pins p);

  void write8 (teal::uint32 a, const teal::reg& d);
  teal::reg read8 (teal::uint32 address);

  //public to allow testbench to swack it. Could have used friend, or a reset_for (10) function
  teal::vreg reset;
  void pause (teal::uint32 clock_count);

  const std::string name;

  void start_ ();

 private:
  teal::vreg clock_;
  teal::vreg address_;
  teal::vreg data_;
  teal::vreg select_;
  teal::vreg  op_code_;
  teal::vreg  do_work_;
  teal::vreg  work_done_;
  teal::mutex mutex_;
  teal::vout log_;

  void write8_ (teal::uint32 a, const teal::reg& d);
  teal::reg read8_ (teal::uint32 address);
  //For the thread communication
  bool internal_read_;
  teal::uint32 internal_address_;
  teal::reg internal_data_;
  teal::condition internal_do_work_;
  teal::condition internal_done_;
};
}

#endif
