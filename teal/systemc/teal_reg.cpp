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
#include <iostream>
#include <algorithm>  //arrgh char traits/bool gets inferred from include "stdio.h"
#include <sstream>
#include "stdio.h" //sprintf

using namespace teal; //lazyness has no bounds


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
vout& teal::operator<< (vout& c, const teal_acc_vecval& rhs)
{
  c << "aval: " << rhs.aval << " bval: " << rhs.bval;
  return c;
}

vout& teal::teal_acc_vecval::operator<< (vout& c) const {return (c << *this);};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#if 0
This is a pretty tricky routine (read possible bugs). Here is the theory:

First, create a copy of the rhs, shifted up to align with the slice.

  Then figure out which words have messy assignments.

  Then, sweep through the sliced register, copying the normalized values across.

  It gets funky at the lower_ word, because you want to preserve the lower bits.
  It gets funky at the upper_ word, because you want to preserve the upper bits.

  If this is a big slice, and there are "pure" words inbetween, just copy them over.

Whew.

#endif
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::ostream& teal::operator<< (std::ostream& c, const reg_slice& rhs)
{
  
  c << std::dec <<" upper " << rhs.upper_ << 
    " lower " << rhs.lower_ << " reg is " << rhs.reg_ << std::endl;
  return c;
}

std::ostream& teal::reg_slice::operator<< (std::ostream& c) const {return (c << *this);};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
vout& teal::operator<< (vout& c, const reg_slice& rhs)
{
  reg foo (rhs);
  c << foo;
  return c;
}

vout& teal::reg_slice::operator<< (vout& c) const {return (c << *this);};


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::reg_slice::operator= (const reg& rhs) 
{
  rhs.read_check ();
  //std::cout << "teal::reg_slice::operator= (const reg&) begin rhs =" << (uint*)&rhs << " " << rhs << std::endl;
  //std::cout << "teal::reg_slice::operator= (const reg&) begin this is  =" << *this << std::endl;
  //std::cout << "*** lower_: " << lower_ << " rhs.bit_length_:" << rhs.bit_length_ << std::endl;
  reg normalized (0, (lower_ + rhs.bit_length_)); //big to allow for shifting
  normalized = lower_ ? (rhs << lower_) : rhs;
  //std::cout << "teal::reg_slice::operator= (const reg&) after shift normalized is  =" << normalized << std::endl;

  //be careful, these might be the same value.
  const uint32 upper_word_index (upper_ / 32);
  const uint32 lower_word_index (lower_ / 32);

  for (uint32 i (lower_word_index); (i <= upper_word_index); i++) {
    if ((i == upper_word_index) || (i == lower_word_index)) {  //might be some in-between words
      uint32 mask (0);
      uint32 upper_bit=(i==upper_word_index?(upper_ % 32):31);
      uint32 lower_bit=(i==lower_word_index?(lower_ % 32):0);
      for(uint32 j=lower_bit;j<=upper_bit;j++) {
        mask|=reg::mask_bit_(j);
      }

      reg_.teal_acc_vecval_[i].aval = (normalized.teal_acc_vecval_[i].aval & mask) | (reg_.teal_acc_vecval_[i].aval & ~mask);
      reg_.teal_acc_vecval_[i].bval = (normalized.teal_acc_vecval_[i].bval & mask) | (reg_.teal_acc_vecval_[i].bval & ~mask);
      //std::cout << "teal::reg_slice::operator= (const reg&) in shift mask is: " << mask << " this is  =" << *this << std::endl;
    }
    else {
      reg_.teal_acc_vecval_[i] = normalized.teal_acc_vecval_[i];
    }
  }
  //std::cout << "teal::reg_slice::operator= (const reg&) at end this is  =" << *this << std::endl;
  reg_.write_through ();
}



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//no memory leak because reg will be dtored when gi\oes out of scope
//course, the slice is now referring to a trashed reg, but onlw allow reg_slices in temp assignments (protected ctor)
void teal::reg_slice::operator= (uint64 r) {*this = reg (r);}; 



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg_slice::reg_slice (uint32 u, uint32 l, reg& r) : upper_ (u), lower_ (l), reg_ (r){};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32 teal::reg::words_ (uint32 b) {return ((b + 31) / 32);};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::reg (uint64 val, uint64 bit_length) :
  bit_length_ (bit_length),
  word_length_ (words_ (bit_length_)),
  teal_acc_vecval_ (new teal_acc_vecval [word_length_])
{
  //   std::cout << "teal::reg::reg (uint64, length) this =" << (uint*)this << " " << *this << std::endl;

  for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i] = 0;}
  for (uint32 j(0); (j < bit_length_); j++, val >>= 1) {  //sleazy simple implementation
    if (val & 1) {
      //      std::cout << "setting bit " << j << " mask is " << std::hex << mask_bit_ (j) << std::endl;
      teal_acc_vecval_[which_word_(j)].aval |= mask_bit_ (j); //produces a 1 at the right place
    } 
  }
  //    std::cout << "teal::reg::reg(uint64, length) this =" << (uint*)this << " " << *this << std::endl;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::reg () :
  bit_length_ (1),
  word_length_ (words_ (bit_length_)),
  teal_acc_vecval_ (new teal_acc_vecval [word_length_])
{
  //  std::cout << "teal::reg::reg() this =" << (uint*)this << " " << *this << std::endl;

  teal_acc_vecval_[word_length_-1].aval  = 0;
  teal_acc_vecval_[word_length_-1].bval  = 1;

  //  teal_acc_vecval_[word_length_-1].aval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
  //  teal_acc_vecval_[word_length_-1].bval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::reg (reg::four_state s) :
  bit_length_ (1),
  word_length_ (words_ (bit_length_)),
  teal_acc_vecval_ (new teal_acc_vecval [word_length_])
{
  //    std::cout << "teal::reg::reg(four_state) this =" << (uint*)this << " " << *this << std::endl;
  switch (s) {
  case reg::one: {
      teal_acc_vecval_[word_length_-1].aval = 1; 
      teal_acc_vecval_[word_length_-1].bval = 0; 
      break;
    }
  case reg::zero : {
      teal_acc_vecval_[word_length_-1].aval = 0; 
      teal_acc_vecval_[word_length_-1].bval = 0; 
      break;
    }
  case reg::X : {
      teal_acc_vecval_[word_length_-1].aval = 1; 
      teal_acc_vecval_[word_length_-1].bval = 1; 
      break;
    }
 case reg::Z : {
      teal_acc_vecval_[word_length_-1].aval = 0; 
      teal_acc_vecval_[word_length_-1].bval = 1; 
      break;
    }
  }
  //    std::cout << "teal::reg::reg(four_state) at end this =" << (uint*)this << " " << *this << std::endl;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::reg (const reg_slice& slice) :
  bit_length_ (slice.upper_ - slice.lower_ + 1),
  word_length_ (words_ (bit_length_)),
  teal_acc_vecval_ (new teal_acc_vecval [word_length_])
{
  //vout local_log ("Teal::reg");
  //   local_log << note << "teal::reg::reg (const reg_slice&) slice is =" << slice << endm;
  //      std::cout << "teal::reg::reg (const reg_clice&) slice is =" << slice << std::endl;
  for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i] = 0;}
  //      std::cout << "teal::reg::reg (const reg_clice&) after clean this =" << (uint*)this << " " <<  *this << std::endl;
  //  local_log << note << "teal::reg::reg (const reg_slice&) slice4 is =" << slice.reg_ << endm;
  reg foo (slice.reg_  >> slice.lower_);
  //      std::cout << "teal::reg::reg (const reg_clice&) foo is =" << foo << std::endl;
  *this = foo;  
   //     std::cout << "end teal::reg::reg (const reg_clice&) this =" << (uint*)this << " " <<  *this << std::endl;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
template <class data_t>
  data_t min_f (data_t lhs,data_t rhs) 
{
  return ((lhs < rhs) ? lhs : rhs);
}
		
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
template <class data_t>
  data_t max_f (data_t lhs,data_t rhs) 
{
  return ((lhs > rhs) ? lhs : rhs);
}
		
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::reg (const reg& rhs) :
  bit_length_ (rhs.bit_length_),
  word_length_ (words_ (bit_length_)),
  teal_acc_vecval_ (new teal_acc_vecval [word_length_])
{
  //      std::cout << "teal::reg::reg (const reg&) this =" << (uint*)this << " " << *this << std::endl;
  //      std::cout << "teal::reg::reg (const reg&) rhs =" << (uint*)&rhs << " " << rhs << std::endl;
  rhs.read_check ();  
  uint32 i(0);
  for (; i < word_length_; i++) {
    teal_acc_vecval_[i] = rhs.teal_acc_vecval_[i];
  }
  //vout local_log ("Teal::reg");
//      local_log << note << "end teal::reg::reg (const reg&) this =" << (uint)this << " " << *this << endm;
//      std::cout << "end teal::reg::reg (const reg&) this =" << (uint*)this << " " << *this << std::endl;
      //      local_log << note << "end teal::reg::reg (const reg&) rhs =" << (uint)&rhs << " " << rhs << endm;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::reg::~reg () {
  //    std::cout << "teal::reg::~reg () this " << (uint*)this << " " << *this << std::endl;
      delete [] teal_acc_vecval_;
      //    std::cout << "after teal::reg::~reg () this " << (uint*)this << std::endl;
};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg_slice teal::reg::operator() (uint32 u, uint32 l)
{
  read_check ();
  return reg_slice (u, l, *this);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg& teal::reg::operator= (const reg& rhs)
{
  rhs.read_check ();
  //    std::cout << "teal::reg::operator=() begin rhs =" << (const uint*) (&rhs) << " " << rhs << std::endl;
  //    std::cout << "begin teal::reg::operator= (const reg&) this =" << (uint)this << " " << *this << std::endl;
  uint32 i(0);
  for (; i < (min_f (word_length_, rhs.word_length_) - 1); i++) {
    teal_acc_vecval_[i] = rhs.teal_acc_vecval_[i];
  }

  //now clean off the upper bits or a hanging word(in case I am smaller than assignment)
  uint32 highest_bit (min_f (bit_length_, rhs.bit_length_) % 32);
  uint32 mask (highest_bit ? (~0UL << highest_bit) : 0);
  teal_acc_vecval_[i].aval = (teal_acc_vecval_[i].aval & mask) | (rhs.teal_acc_vecval_[i].aval & ~mask);
  teal_acc_vecval_[i].bval = (teal_acc_vecval_[i].bval & mask) | (rhs.teal_acc_vecval_[i].bval & ~mask);
  i++;

  
  //now clean off any whole word parts, I am zero above assignment
  if (i < word_length_) {
    //work to do
    for (; i < word_length_; i++) {
      //  std::cout << "teal::reg::operator=() clearing word =" << i << std::endl;
      teal_acc_vecval_[i].aval = 0;
      teal_acc_vecval_[i].bval = 0;
    }
  }
  //    std::cout << "teal::reg::operator=() completed this =" << (uint*)this << " " << *this << std::endl;
  write_through ();
  return *this;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::reg::operator() (uint32 u, uint32 l) const
{
  read_check ();
  //  std::cout << "teal::reg::operator(int,int) (" << u << l << ") " << *this << std::endl;
  reg a_constant (*this);  //reg slice wants a non const reg, since it can potentially whack it.
  reg_slice foo (u,l, a_constant);
  reg foo2 = foo;  //different size assignment muck, should make a reg_slice to reg operator=
  return foo2;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
static reg::four_state bits_to_four_state (char a, char b)
{
  if (b == 0) {
    return (a ? reg::one : reg::zero);
  }
  return (a ? reg::X : reg::Z);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg::four_state teal::reg::operator() (uint32 b) const
{
  read_check ();
  //teal_assert (b < bit_length_);
  return bits_to_four_state ((teal_acc_vecval_[which_word_(b)].aval >> (b % 32)) & 1,
			      (teal_acc_vecval_[which_word_(b)].bval >> (b % 32)) & 1);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
char teal::reg::to_int (uint32 b) const
{
  //  std::cout << "teal::reg::to_int (int) this =" << *this << " bit: " << b << std::endl;
  read_check ();
  //teal_assert (b < bit_length_);
  return (teal_acc_vecval_[which_word_(b)].aval >> (b % 32)) & 1;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32 teal::reg::which_word_ (uint32 b) {return (b / 32);}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32 teal::reg::mask_bit_ (uint32 b) {return (1UL << (b % 32));}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
vout& teal::operator<< (vout& v, reg::four_state s) 
{
  switch (s) {
    case reg::one : v << "one"; break;
    case reg::zero : v << "zero"; break;
    case reg::X : v << "X"; break;
    case reg::Z : v << "Z"; break;
  }
  return v;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool teal::operator== (const reg& lhs, const reg& rhs)
{
  return (triple_equal (lhs, rhs) == reg::one);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg::four_state teal::triple_equal (const reg& lhs, const reg& rhs)
{
  lhs.read_check ();
  rhs.read_check ();
  //vout local_log ("Teal::reg");
  //      log << note << " Operator == rhs: " << rhs << endm;
  //      local_log << note << " Operator == lhs: " << lhs << endm;

  reg::four_state returned (reg::one); //a sunny outlook
  for (uint32 i(0); i < max_f (lhs.word_length_, rhs.word_length_); i++) {
    teal_acc_vecval lhs_val ( (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i] : 0);
    teal_acc_vecval rhs_val ( (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i] : 0);

    //    local_log << note << "operator== lhs_val " << lhs_val.aval << "," << lhs_val.bval << 
    //              " rhs: " << rhs_val.aval << "," << rhs_val.bval << endm;
    if ((lhs_val.bval != rhs_val.bval) && (lhs_val.bval || rhs_val.bval))  {
      // local_log << note << "operator== returning X" << endm;
      return reg::X;
    }
    if (lhs_val.aval != rhs_val.aval) {
      //  local_log << note << "operator== returning zero" << endm;
      returned = reg::zero; //must keep looking for XZ, they take precedence
    }
  }
  //local_log << note << "operator== returning " << returned << endm;
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg::four_state teal::operator< (const reg& lhs, const reg& rhs)
{
  reg::four_state returned (reg::Z); 
  for (uint32 i(0); i < max_f (lhs.word_length_, rhs.word_length_); i++) {
    teal_acc_vecval lhs_val ( (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i] : 0);
    teal_acc_vecval rhs_val ( (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i] : 0);

    if (lhs_val.bval || rhs_val.bval) {
      return reg::X;
    }
    if ((lhs_val.aval < rhs_val.aval) && (returned == reg::Z)) {
      returned = reg::one;
    }
  }
  return (returned == reg::Z) ? reg::zero : returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::reg::resize (uint32 bits)
{
  //  std::cout << "teal::reg::resize (int) this =" << *this << " new length: " << bits << std::endl;
  bit_length_ = bits;
  word_length_ = words_ (bit_length_);
  delete [] teal_acc_vecval_;
  teal_acc_vecval_ = new teal_acc_vecval [word_length_];
  teal_acc_vecval_[word_length_-1].aval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
  teal_acc_vecval_[word_length_-1].bval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
  //  std::cout << "teal::reg::resize (int) done this =" << *this << " new length: " << bits << std::endl;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/*
This is a bit of a tricky routine.  It uses the fact that a 64 bit entity can be handled by the compiler 
as an inetgral type. It also relies on the fact that the vecval uses 32 bits. Because of these two conditions,
 the alogrithm can store the intermediae result of a shift.

Theory of Operation:
There are teo basic shifts going one here. One is the word level shift, which makes sure the lhs is copies into 
the right word of the shift. The other is ts modulo 32 shift, which takes care of the factrional bar of a shift.

The basic idea is to fill in the shifted register with the upper 32 bits of the intermediate 64 bit register. 
When the alogithm does a "val <<= shift", it is shifting out the previous 32 bit word (copied beforehand 
since the algorithm moves from high to low) and the part to be used is in the upper 32 bits.  After the assignment 
to the shifted register, the val is shifted up to put any non modulo 32 bits in the upper 32 bits.

Note that since the shift up brings in zeroes, we can just "or in" our the next 32 bit value.
*/
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator<< (const reg & lhs, uint32 shift)
{
  //std::cout << "teal::operator<<(lhs,int) by " << shift << " lhs  ="  <<  lhs << std::endl;
  if (shift ==0) return lhs;

  lhs.read_check ();

  reg returned (0, (lhs.bit_length_ + shift));
  returned = 0;

  int shift_from (lhs.word_length_ -1);
  int shift_to   (returned.word_length_ -1);

  shift = shift % 32; //the shift_to and from take care of the word part


  uint64 aval (0);
  uint64 bval (0);

  //pre-load val with shifted input as an initial value if the shift in the loop would not put it there
  if ( (!shift)||((lhs.bit_length_ %32) && (((lhs.bit_length_ %32) + shift) <= 32))) { //the shift would not cross a 32 bit boundary...
    aval = (shift_from >= 0) ? static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].aval)) << 32 : 0;
    bval = (shift_from >= 0) ? static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].bval)) << 32 : 0;
    --shift_from;
  }

  //std::cout << "teal::reg::operator<<(reg, int) initial shift " << shift_from << " a: " << aval << " b: " << bval << 
  //  " returned  ="  <<  returned << std::endl;

  //copy the words,
  for (; (shift_to >= 0); --shift_to, --shift_from) {
    aval |= (shift_from >= 0) ?  (uint32)lhs.teal_acc_vecval_[shift_from].aval : 0;
    aval <<= shift;
    returned.teal_acc_vecval_[shift_to].aval = aval >> 32; 
    aval <<= (32 - shift);

    bval |= (shift_from >= 0) ?  (uint32)lhs.teal_acc_vecval_[shift_from].bval : 0;
    bval <<= shift;
    returned.teal_acc_vecval_[shift_to].bval = bval >> 32; 
    bval <<= (32 - shift);

    //std::cout << "teal::reg::operator<<(reg, int) after shift " << shift << " a: " << aval << " b: " << bval << 
    //  " returned  ="  <<  returned << std::endl;
  }

  //std::cout << "teal::reg::operator<<(reg, int) returned  ="  <<  returned << std::endl;
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/*
This is a bit of a tricky routine, Read the left shift first to get an idea of how this will work.

Theory of Operation:
Just like the left shift, this uses a 64 bit intermediate to hold the previous shift remainder.  It copies from low to high and keeps the interesting bits in the lower half.
*/
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator>> (const reg & lhs, uint32 shift)
{
  //  std::cout << "teal::reg::operator>>() by " << shift << " lhs  ="  <<  lhs << std::endl;
  if (shift == 0) return lhs;
  
  lhs.read_check ();

  reg returned (0, lhs.bit_length_ + shift);
  for (uint32 i(0); (i < returned.word_length_); i++) {returned.teal_acc_vecval_[i] = 0;}

  uint32 shift_from (reg::which_word_ (shift));

  shift = shift % 32; //the shift_to and from take care or the word part

  //pre-load val with shifted input as an initial value
  uint64 aval ((shift_from < lhs.word_length_) ? static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].aval)): 0ULL);
  uint64 bval ((shift_from < lhs.word_length_) ? static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].bval)): 0ULL);
  shift_from++;

  //  std::cout << "teal::reg::operator>>() initial shift_from " << shift_from << " a is: " << aval << " b is: " << bval << 
  //      " returned  ="  <<  returned << std::endl;

  //copy the words,
  for (uint32 shift_to (0); (shift_to < lhs.word_length_); ++shift_to, ++shift_from) {
    aval |= (shift_from < lhs.word_length_) ?  static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].aval)) << 32 : 0ULL;
    aval >>= shift;
    returned.teal_acc_vecval_[shift_to].aval = aval;
    aval >>= (32 - shift);

    bval |= (shift_from < lhs.word_length_) ?  static_cast <uint64> (static_cast<uint32>(lhs.teal_acc_vecval_[shift_from].bval)) << 32 : 0ULL;
    bval >>= shift;
    returned.teal_acc_vecval_[shift_to].bval = bval;
    bval >>= (32 - shift);

    //    std::cout << "teal::reg::operator>>() after shift " << shift << " a: " << aval << " b: " << bval << 
    //      " returned  ="  <<  returned << std::endl;
  }

  //  std::cout << "teal::reg::operator>>(int) returnning " << returned << std::endl;

  return returned;
}




///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/*
Theory of operation:

In order to do an add, we need to strip off the X's and Z's in the aval.  Since
these are represented as 1s in the bval, just take them down to zeroes.

Then add them, setting carry if there was an overflow.

Finally, bring back the X's and Z's by OR'ing in the original ones and then bringing them (as X's) into the aval.

[Ive gotta read the spec again and see if this is correct, or if the entire value should be brought to an X.]
*/

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator+ (const reg & lhs, const reg& rhs)
{
  lhs.read_check ();
  rhs.read_check ();

  //    std::cout << "teal::operator+(reg,reg) begin rhs =" << (const uint*) (&rhs) << " " << rhs << std::endl;
  //      std::cout << "teal::operator+(reg,reg) begin lhs =" << (const uint*) (&lhs) << " " << lhs << std::endl;

  reg returned = reg (0, max_f (lhs.bit_length_ + 1, rhs.bit_length_ + 1));

  uint32 carry (0);
  for (uint32 i(0); (i < returned.word_length_); i++) {
    teal_acc_vecval lhs_vecval = (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i]: 0;
    teal_acc_vecval  rhs_vecval = (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i]: 0;

    lhs_vecval.aval = lhs_vecval.aval & ~lhs_vecval.bval;
    rhs_vecval.aval = rhs_vecval.aval & ~rhs_vecval.bval;

    returned.teal_acc_vecval_[i].aval = lhs_vecval.aval + rhs_vecval.aval + carry;
    //        std::cout << "teal::operator+(reg,reg) aval[" << i << "] =" << returned.teal_acc_vecval_[i].aval << std::endl;
    carry = ((returned.teal_acc_vecval_[i].aval < lhs_vecval.aval) ||
	     (returned.teal_acc_vecval_[i].aval < rhs_vecval.aval)) ? 1 : 0;

    returned.teal_acc_vecval_[i].bval = lhs_vecval.bval | rhs_vecval.bval;
    //std::cout << "teal::operator+(reg,reg) bval[" << i << "] =" << returned.teal_acc_vecval_[i].bval << std::endl;

    returned.teal_acc_vecval_[i].aval |= returned.teal_acc_vecval_[i].bval;
  }

  //  assert (!carry);
  returned.teal_acc_vecval_[returned.word_length_ -1].aval &= ~ (~0ULL << (returned.bit_length_ % 32));
  returned.teal_acc_vecval_[returned.word_length_ -1].bval &= ~ (~0ULL << (returned.bit_length_ % 32));
  //  std::cout << "teal::operator+(reg,reg) end returned =" << (const uint*) (&returned) << " " << returned << std::endl;
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator- (const reg & lhs, const reg& rhs)
{
  lhs.read_check ();
  rhs.read_check ();

  reg returned = reg (0, max_f (lhs.bit_length_ + 1, rhs.bit_length_ + 1));

  uint32 borrow (0);
  for (uint32 i(0); (i < returned.word_length_); i++) {
    teal_acc_vecval lhs_vecval = (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i]: 0;
    teal_acc_vecval rhs_vecval = (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i]: 0;

    lhs_vecval.aval = lhs_vecval.aval & ~lhs_vecval.bval;
    rhs_vecval.aval = rhs_vecval.aval & ~rhs_vecval.bval;

    returned.teal_acc_vecval_[i].aval = lhs_vecval.aval - rhs_vecval.aval - borrow;
    borrow = ((returned.teal_acc_vecval_[i].aval > lhs_vecval.aval) ||
	     (returned.teal_acc_vecval_[i].aval > rhs_vecval.aval)) ? 1 : 0;

    returned.teal_acc_vecval_[i].bval = lhs_vecval.bval | rhs_vecval.bval;
    returned.teal_acc_vecval_[i].aval |= returned.teal_acc_vecval_[i].bval;
  }

  //  assert (!borrow);
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator& (const reg & lhs, const reg& rhs)
{
  lhs.read_check ();
  rhs.read_check ();

  reg returned = reg (0, max_f (lhs.bit_length_ + 1, rhs.bit_length_ + 1));

  for (uint32 i(0); (i < returned.word_length_); i++) {
    teal_acc_vecval lhs_vecval = (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i]: 0;
    teal_acc_vecval rhs_vecval = (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i]: 0;

    lhs_vecval.aval = lhs_vecval.aval & ~lhs_vecval.bval;
    rhs_vecval.aval = rhs_vecval.aval & ~rhs_vecval.bval;

    returned.teal_acc_vecval_[i].aval = lhs_vecval.aval & rhs_vecval.aval;
    returned.teal_acc_vecval_[i].bval = lhs_vecval.bval | rhs_vecval.bval;
    returned.teal_acc_vecval_[i].aval |= returned.teal_acc_vecval_[i].bval;
  }

  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//Like all the rest, except that "1 or X/Y = 1"

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator| (const reg & lhs, const reg& rhs)
{
  lhs.read_check ();
  rhs.read_check ();

  reg returned = reg (0, max_f (lhs.bit_length_ + 1, rhs.bit_length_ + 1));

  for (uint32 i(0); (i < returned.word_length_); i++) {
    teal_acc_vecval lhs_vecval = (i < lhs.word_length_) ? lhs.teal_acc_vecval_[i]: 0;
    teal_acc_vecval rhs_vecval = (i < rhs.word_length_) ? rhs.teal_acc_vecval_[i]: 0;

    lhs_vecval.aval = lhs_vecval.aval & ~lhs_vecval.bval;
    rhs_vecval.aval = rhs_vecval.aval & ~rhs_vecval.bval;

    returned.teal_acc_vecval_[i].aval = lhs_vecval.aval | rhs_vecval.aval;
    returned.teal_acc_vecval_[i].bval = lhs_vecval.bval | rhs_vecval.bval;

    returned.teal_acc_vecval_[i].bval &= ~returned.teal_acc_vecval_[i].aval;
    returned.teal_acc_vecval_[i].aval |= returned.teal_acc_vecval_[i].bval;
  }

  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//ieee 1364-2001 spec page 49, Table 26

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg::four_state teal::reduce_xor (const reg& rhs)
{
  //  vout local_log ("teal::reduce_xor");
  rhs.read_check ();

  //assert (rhs.bit_length >= 2); //ow undefined

  reg::four_state returned (rhs (0)); 
  
  //  local_log << "bit 0 is : " << returned << endm;

  for (uint32 i(1); (i < rhs.bit_length_); i++) {
    if (returned == reg::X) break;

    reg::four_state current (rhs (i));
    switch (current) {
    case reg::one : returned = (returned == reg::one) ? reg::zero : reg::one; break;
    case reg::zero : ; break; //zero xor zero is 0, zero xor one is one
    case reg::X : returned = reg::X; break;
    case reg::Z : returned = reg::X; break;
    }
    //    local_log << "bit " << i << " is : " << current << " returned: " << returned << endm;
  }
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//Z's become X's 
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::operator~ (const reg & lhs)
{
  lhs.read_check ();
  reg returned = reg (0, lhs.bit_length_);

  for (uint32 i(0); (i < returned.word_length_); i++) {
    teal_acc_vecval lhs_vecval (lhs.teal_acc_vecval_[i]);

    lhs_vecval.aval = lhs_vecval.aval & ~lhs_vecval.bval;
    returned.teal_acc_vecval_[i].aval = ~lhs_vecval.aval;
    returned.teal_acc_vecval_[i].bval = lhs_vecval.bval;
    returned.teal_acc_vecval_[i].aval |= returned.teal_acc_vecval_[i].bval;
  }
  //clean since invert wacks the whole ab val
  returned.teal_acc_vecval_[returned.word_length_-1].aval &= ~ (~0UL << (returned.bit_length_ % 32)); //clean above, make math easier
  returned.teal_acc_vecval_[returned.word_length_-1].bval &= ~ (~0UL << (returned.bit_length_ % 32)); //clean above, make math easier

  //  std::cout << "teal::reg::operator~ () ret =" << returned << " lhs " << lhs << std::endl;

  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
teal::uint64 teal::reg::to_int () const
{
  read_check ();
  //vout local_log ("Teal::reg");
//std::cout << "teal::reg::to_int () this =" << *this << " base is " << local_log.base() << std::endl;
  if (word_length_ == 1) {
    return ((teal::uint32) (teal_acc_vecval_[0].aval));
  }
  else {
    return (static_cast<uint64> ((uint32) teal_acc_vecval_[1].aval) << 32) + (uint32) teal_acc_vecval_[0].aval;
  }

}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::reg::format_hex_string () const
{
  read_check ();
  //vout local_log ("Teal::reg");
  //  std::cout << "teal::reg::format_hex_string () this =" << *this << " base is " << local_log.base() << std::endl;
  std::string returned;
  uint32 bits(0);
  for (uint32 i(0); (i < word_length_); i++) {
    uint32 j(0);
    for (; (bits < bit_length_) && (j <= 7); bits += 4) {    
      uint8 a_nibble;
      uint8 b_nibble;
      get_nibble_ (i, j, &a_nibble, &b_nibble);
      //      std::cout << " bits " << bits << " a " << (int)a_nibble << " b " << (int)b_nibble << std::endl;
      if (b_nibble) {
	char temp = 'Z';
	for (uint32 k(0); k <= 3; ++k) {
	  if ((a_nibble & 1) && (b_nibble & 1)) {
	    temp = 'X';
	    break;
	  }
	  a_nibble >>= 1;
	  b_nibble >>= 1;
	}
	returned += temp;
      }
      else {
	switch (a_nibble) {
	case 0: returned += "0"; break;
	case 1: returned += "1"; break;
	case 2: returned += "2"; break;
	case 3: returned += "3"; break;
	case 4: returned += "4"; break;
	case 5: returned += "5"; break;
	case 6: returned += "6"; break;
	case 7: returned += "7"; break;
	case 8: returned += "8"; break;
	case 9: returned += "9"; break;
	case 10: returned += "a"; break;
	case 11: returned += "b"; break;
	case 12: returned += "c"; break;
	case 13: returned += "d"; break;
	case 14: returned += "e"; break;
	case 15: returned += "f"; break;
	default: returned += "?";
	}
      } //no b val
      j++;
    }
  }
  std::reverse (returned.begin (), returned.end ());
  //  if (local_log.base() == vout::hex) {
    std::ostringstream foo;
    //foo << bit_length_;  //WHY DOES THIS HANG???
    //std::cout << "string is " << foo.str() << std::endl;
    //    returned =  foo.str () +  "\'h" + returned;
    char temp[256];
    sprintf (temp, "%d\'h", bit_length_);
    returned = temp + returned;
    //  }
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::reg::format_binary_string () const
{
  read_check ();
  //vout local_log ("Teal::reg");
  //  std::cout << "teal::reg::format_binary_string () this =" << *this << " base is " << local_log.base() << std::endl;

  std::string returned;
  for (uint32 i(0); i < bit_length_; ++i) {
    switch (this->operator() (i)) {
    case reg::one : returned += "1"; break;
    case reg::zero : returned += "0"; break;
    case reg::X : returned += "x"; break;
    case reg::Z : returned += "z"; break;
    }
  }

  std::reverse (returned.begin (), returned.end ());
  //  if (local_log.base() == vout::hex) {
    std::ostringstream foo;
    char temp[256];
    sprintf (temp, "%d\'b", bit_length_);
    returned = temp + returned;
    //  }
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::reg::format_decimal_string () const
{
  read_check ();
  //vout local_log ("Teal::reg");
  //  std::cout << "teal::reg::format_decimal_string () this =" << *this << " base is " << local_log.base() << std::endl;

  std::string returned;
  if (word_length_ == 1) {
    if (!teal_acc_vecval_[0].bval) {
      std::ostringstream foo;
      foo << bit_length_ << "\'d" << teal_acc_vecval_[0].aval;
      return foo.str ();
    }
  }
  else if ((word_length_ == 2) && (!teal_acc_vecval_[0].bval) &&
	   (!teal_acc_vecval_[1].bval)) {
      std::ostringstream foo;
      foo << bit_length_ << "\'d" 
	  << (static_cast<uint64> (teal_acc_vecval_[1].aval) << 32) + 
	teal_acc_vecval_[0].aval;
      return foo.str ();
  }

  return format_hex_string (); //not supposed to be here!
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//the i picks the word, the j picks the nibble (0,7)

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::reg::get_nibble_ (uint32 i, uint32 j, uint8* a, uint8* b) const
{
  *a =  ((teal_acc_vecval_[i].aval) >> (j * 4)) & 0xf;
  *b =  ((teal_acc_vecval_[i].bval) >> (j * 4)) & 0xf;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::ostream& teal::operator<< (std::ostream& c, const reg& rhs)
{
  //NO READ CHECK!! This is for debugging only
  c << std::dec <<"bit_length " << rhs.bit_length_ << 
    " word length " << rhs.word_length_ << std::hex  <<
    " vecval at " << (uint32*)rhs.teal_acc_vecval_ << std::endl;
  for (uint32 i(0); (i < rhs.word_length_) ; ++i) {
    c << " word [" << i << "].aval " << rhs.teal_acc_vecval_[i].aval << std::endl;
    c << " word [" << i << "].bval " << rhs.teal_acc_vecval_[i].bval << std::endl;
  }
  return c;
}

std::ostream& teal::reg::operator<< (std::ostream& c) const {return (c << *this);};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
vout& teal::reg::operator<< (vout& c) const 
{
  read_check ();

  if (c.base () == vout::dec) {
    c << format_decimal_string ();
  }
  else if (c.base () == vout::binary) {
    c << format_binary_string ();
  }
  else {
    c << format_hex_string ();
  }

#if 0
  c << teal::dec << "bit_length " << bit_length_ << 
    " word length " << word_length_ << hex  <<
    " vecval at " << (long)teal_acc_vecval_ << teal::endl;
  for (uint32 i(0); (i < word_length_) ; ++i) {
    c << " word [" << i << "].aval " << teal_acc_vecval_[i].aval << teal::endl;
    c << " word [" << i << "].bval " << teal_acc_vecval_[i].bval << teal::endl;
  }
#endif
  return c;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::operator<< (vout& c, const reg& rhs)
{
  return rhs.operator<< (c); //let virtual function take over
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
reg teal::reg::max_value (uint32 num_bits)
{
  reg returned = reg (0, num_bits);
  for (uint32 i(0); i < returned.word_length_; ++i) {
    returned.teal_acc_vecval_[i].aval = -1;
    returned.teal_acc_vecval_[i].bval = 0;
  }
  returned.teal_acc_vecval_[returned.word_length_-1].aval &= 
    ~ (~0UL << (returned.bit_length_ % 32)); //clean above, make math easier
  returned.teal_acc_vecval_[returned.word_length_-1].bval &= 
    ~ (~0UL << (returned.bit_length_ % 32)); //clean above, make math easier
  return returned;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool teal::has_x (const reg& rhs)
{
  rhs.read_check ();

  for (uint32 i(1); (i < rhs.bit_length()); i++) {
    if (rhs(i) == reg::X) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool teal::has_xz (const reg& rhs)
{
  rhs.read_check ();

  for (uint32 i(0); (i < rhs.bit_length()); i++) {
    if ((rhs(i) == reg::X) or (rhs(i) == reg::Z)) return true;
  }
  return false;
}

