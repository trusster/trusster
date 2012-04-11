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
/*Copyright (C) 2004 Mike Mintz

This file is part of the Apple Valley Verification IP Library. 

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

#ifndef __truss__
#define __truss__



#include "truss_verification_component.h"
#include "truss_thread.h"
#include "truss_watchdog.h"
#include "truss_shutdown.h"
#include "truss_test_base.h"
#include "truss_testbench_base.h"
#include "truss_vout.h"
#include "truss_checker.h"
#include "truss_test_component.h"

#include "teal.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define truss_assert(x) if (!(x)) log_ << teal_fatal << " assertion falure:" << #x  << teal::endm;
#include "truss_channel.h"


 namespace truss {
  extern std::string truss_version;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <class data_type> 
      struct port  {
	typedef std::map < data_type , std::string> pins;
      };



   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
  class random_reg : public teal::reg {
  public:
  random_reg ( teal::random_range* rander, teal::uint32 bit_size) :
    reg (0,bit_size), rand_ (rander) {
    teal::vout test ("truss::random_reg::random_reg (rander, bitsize)");
    //    test << teal_debug << "ctor" << (int)this << teal::endm;
  }

  virtual ~random_reg () {
    teal::vout test ("truss::random_reg::~random_reg");
    //    test << teal_debug << "dtor" << (int)this << teal::endm;
  }
  random_reg (const random_reg& rhs) :
    reg (rhs), rand_ (rhs.rand_)
    {
    teal::vout test ("truss::random_reg::random_reg (const random_reg&)");
    //    test << teal_debug << "copy ctor" << (int)this << teal::endm;
  }

  random_reg operator= (const random_reg& rhs)
    {
    teal::vout test ("truss::random_reg::operator= (const random_reg&)");
    //    test << teal_debug << "op=" << (int)this << teal::endm;
    reg::operator=(rhs);
    rand_ = rhs.rand_;
    return *this;
  }


    void randomize () {
      //for all aval/bvals, then clean abouve
      //aval[0] = (uint32)(r.draw () * (double) (1<<31));};
    }
    void randomize (const teal::reg & low, const reg& high) {
      teal::vout test ("russ::random_reg::randomize(min,max)");
      for (teal::uint32 i(0); (i < word_length_); i++) {
	teal::teal_acc_vecval low_vecval = low.vecval (i);
	teal::teal_acc_vecval high_vecval = high.vecval (i);
	
	//	test << teal_debug << " at: " << i << " low: " << low_vecval << " high: " << high_vecval << teal::endm;
	teal_acc_vecval_[i].aval = rand_->draw (low_vecval.aval, high_vecval.aval);
	teal_acc_vecval_[i].bval = rand_->draw (low_vecval.bval, high_vecval.bval);
	teal_acc_vecval_[i].aval |= teal_acc_vecval_[i].bval;
	//test << teal_debug << " at: " << i << " returned vecval: " << teal_acc_vecval_[i] << teal::endm;
      }      
      teal_acc_vecval_[word_length_-1].aval &= 
	~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
      teal_acc_vecval_[word_length_-1].bval &= 
	~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
      //test << teal_debug << " value is: " << *this << teal::endm;
    }
    void randomize_no_xz () {
      //for all aval/bvals, then clean abouve
      //aval[0] = (uint32)(r.draw () * (double) (1<<31));};
    }

    void randomize_no_xz (const teal::reg & low, const reg& high) { //bval is ignored, needed for arb bit length
      //for all aval/bvals, then clean abouve
      //aval[0] = (uint32)(r.draw () * (double) (1<<31));};
    }

    reg& operator= (const reg & r) {
      reg::operator= (r);
      return *this;
    }

  private:
    teal::random_range* rand_;
  };


 }; //TRUSS namespace

#endif
