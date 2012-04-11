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

//#include "teal_vreg_internal.h"
#include "teal_vbus.h"
#define MARK __FILE__ << " " <<std::dec << __LINE__ <<  " [ " << teal::vtime () << "] " 

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
vreg* teal::vreg::operator[] (int index) {
  //  std::cout << MARK << " ::vreg::op[] on index " << index << " " << *this << std::endl;
return new vreg (rep_->operator[](index));}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
std::string teal::vreg::name () const {return rep_->name();}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::operator<< (vout& v, const vreg& s)
{
  return s.operator<< (v);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::vreg::operator<< (vout& v) const
{
  v << name() << " " ;
  return this->reg::operator<<(v);
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg::vreg (const std::string& n, uint32 bit_length, signal_control control) : reg(0,bit_length)
{
 switch (bit_length) {
   case 1: rep_ = new teal::vreg_internal  (n, control); break;
   case 2: rep_ = new vreg_internal2 (n, control); break;
   case 3: rep_ = new vreg_internal3 (n, control); break;
   case 4: rep_ = new vreg_internal4 (n, control); break;
   case 5: rep_ = new vreg_internal5 (n, control); break;
   case 6: rep_ = new vreg_internal6 (n, control); break;
   case 7: rep_ = new vreg_internal7 (n, control); break;
   case 8: rep_ = new vreg_internal8 (n, control); break;
   case 9: rep_ = new vreg_internal9 (n, control); break;
   case 10: rep_ = new vreg_internal10 (n, control); break;
   case 11: rep_ = new vreg_internal11 (n, control); break;
   case 12: rep_ = new vreg_internal12 (n, control); break;
   case 13: rep_ = new vreg_internal13 (n, control); break;
   case 14: rep_ = new vreg_internal14 (n, control); break;
   case 15: rep_ = new vreg_internal15 (n, control); break;
   case 16: rep_ = new vreg_internal16 (n, control); break;
   case 17: rep_ = new vreg_internal17 (n, control); break;
   case 18: rep_ = new vreg_internal18 (n, control); break;
   case 19: rep_ = new vreg_internal19 (n, control); break;
   case 20: rep_ = new vreg_internal20 (n, control); break;
   case 21: rep_ = new vreg_internal21 (n, control); break;
   case 22: rep_ = new vreg_internal22 (n, control); break;
   case 23: rep_ = new vreg_internal23 (n, control); break;
   case 24: rep_ = new vreg_internal24 (n, control); break;
   case 25: rep_ = new vreg_internal25 (n, control); break;
   case 26: rep_ = new vreg_internal26 (n, control); break;
   case 27: rep_ = new vreg_internal27 (n, control); break;
   case 28: rep_ = new vreg_internal28 (n, control); break;
   case 29: rep_ = new vreg_internal29 (n, control); break;
   case 30: rep_ = new vreg_internal30 (n, control); break;
   case 31: rep_ = new vreg_internal31 (n, control); break;
   case 32: rep_ = new vreg_internal32 (n, control); break;
   case 33: rep_ = new vreg_internal33 (n, control); break;
   case 34: rep_ = new vreg_internal34 (n, control); break;
   case 35: rep_ = new vreg_internal35 (n, control); break;
   case 36: rep_ = new vreg_internal36 (n, control); break;
   case 37: rep_ = new vreg_internal37 (n, control); break;
   case 38: rep_ = new vreg_internal38 (n, control); break;
   case 39: rep_ = new vreg_internal39 (n, control); break;
   case 40: rep_ = new vreg_internal40 (n, control); break;
   case 41: rep_ = new vreg_internal41 (n, control); break;
   case 42: rep_ = new vreg_internal42 (n, control); break;
   case 43: rep_ = new vreg_internal43 (n, control); break;
   case 44: rep_ = new vreg_internal44 (n, control); break;
   case 45: rep_ = new vreg_internal45 (n, control); break;
   case 46: rep_ = new vreg_internal46 (n, control); break;
   case 47: rep_ = new vreg_internal47 (n, control); break;
   case 48: rep_ = new vreg_internal48 (n, control); break;
   case 49: rep_ = new vreg_internal49 (n, control); break;
   case 50: rep_ = new vreg_internal50 (n, control); break;
   case 51: rep_ = new vreg_internal51 (n, control); break;
   case 52: rep_ = new vreg_internal52 (n, control); break;
   case 53: rep_ = new vreg_internal53 (n, control); break;
   case 54: rep_ = new vreg_internal54 (n, control); break;
   case 55: rep_ = new vreg_internal55 (n, control); break;
   case 56: rep_ = new vreg_internal56 (n, control); break;
   case 57: rep_ = new vreg_internal57 (n, control); break;
   case 58: rep_ = new vreg_internal58 (n, control); break;
   case 59: rep_ = new vreg_internal59 (n, control); break;
   case 60: rep_ = new vreg_internal60 (n, control); break;
   case 61: rep_ = new vreg_internal61 (n, control); break;
   case 62: rep_ = new vreg_internal62 (n, control); break;
   case 63: rep_ = new vreg_internal63 (n, control); break;
   case 64: rep_ = new vreg_internal64 (n, control); break;
   case 96: rep_ = new vreg_internal96 (n, control); break;

   case 192: rep_ = new vreg_internal192 (n, control); break;
     //beyond this us teal::vreg (name, new vbus<name, your_length>,control
     //I gotta template that!!!)
 default: {teal::vout log ("vreg ctor"); log << teal_fatal << "Cannot alloc a vreg of length: " << bit_length << ". Modify this case statment" << teal::endl;}
 } 
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//steal the value
void /*vreg&*/ teal::vreg::operator= (const reg& r) {
  rep_->operator= (r);
  //  return *this;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg::~vreg ()
{
  //	teal::vout x(""); x << teal_info << " 000 " << endm;
  //leak rep. It's ok you cannot go making wires on the fly, only at elab time
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::read_check () const
{
  rep_->read_check ();
  //now need to propigate to my reg storage
   for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i] = rep_->teal_acc_vecval_[i];}      
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::write_through () const {
  //no copy needed as operator=() calls rep's one!
  rep_->write_through ();
}


const sc_core::sc_event& teal::vreg::value_changed_event () const {return rep_->value_changed_event();};
const sc_core::sc_event& teal::vreg::posedge_event ()       const {return rep_->posedge_event();};
const sc_core::sc_event& teal::vreg::negedge_event ()       const {return rep_->negedge_event();};
