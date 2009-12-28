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
#include <systemc.h>

using namespace teal;

//Notes for Debugging this code: Do not use teal's looger as ot calls read_check(). That will whack the state of the reg
//use cout as it purposely does not do a read check
#define MARK __FILE__ << " " <<std::dec << __LINE__ << " " << path_and_name_ <<  " [ " << teal::vtime () << "] " 


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::operator<< (vout& v, const teal::vreg_internal& s)
{
  return s.operator<< (v);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::vreg_internal::operator<< (vout& v) const
{
  v << name() << " " ;
  return this->reg::operator<<(v);
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg_internal::vreg_internal (sc_core::sc_module_name n) : 
  path_and_name_(n), in_read_check_ (false), update_time_(0), state_ (master_state_ -1) {
  one_bit_wr_ = new sc_core::sc_signal_resolved (sc_gen_unique_name("one_bit_wr_"));   
  one_bit_wr_->write (sc_dt::Log_Z);
  one_bit_rd_ = one_bit_wr_;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//teal::vreg_internal::vreg_internal (const std::string& n, signal_control control, sc_core::sc_module_name) : 
teal::vreg_internal::vreg_internal (const std::string& n, teal::vreg::signal_control control) : 
  path_and_name_ (n),	 update_time_(0), in_read_check_ (false), state_ (master_state_ -1)
{
  //  one_bit_rd_ = new sc_core::sc_signal_resolved (sc_gen_unique_name("one_bit_rd_"));
  one_bit_rd_ = new sc_core::sc_signal_resolved (n.c_str());
  //  one_bit_wr_ = new sc_core::sc_signal_resolved (sc_gen_unique_name("one_bit_wr_"));
  one_bit_wr_ = new sc_core::sc_signal_resolved (n.c_str());

  teal::vout x("ctor"); //x << teal_info  << "\"" << n << "\" " << *this << endm;


#if defined (MTI_SYSTEMC) 
  if (control == teal::vreg::observe_and_control) {
    bool success = (n == "") or one_bit_wr_->control_foreign_signal (n.c_str ());
    if (!success) x << teal_fatal << "cannot find signal \"" << n << "\"" << teal::endm;

    one_bit_wr_->write (sc_dt::Log_Z);

    success = (n == "") or one_bit_rd_->observe_foreign_signal (n.c_str ());
    if (!success) x << teal_fatal << "cannot find signal \"" << n << "\"" << teal::endm; //check again
  }
  else {
    bool success = (n == "") or one_bit_rd_->observe_foreign_signal (n.c_str ());
    if (!success) x << teal_fatal << "cannot find signal \"" << n << "\"" << teal::endm;
 
    one_bit_wr_ = one_bit_rd_;
 }
#endif
  //    one_bit_->write (sc_dt::Log_Z);
  teal_acc_vecval_[word_length_-1].aval = 0; 
  teal_acc_vecval_[word_length_-1].bval = 1; 
  //    std::cout << MARK << " ::vreg_internal::ctor " << *this << std::endl;
}



//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#if defined (TEAL_SC_FOREIGN)
teal::vreg_internal::vreg_internal (sc_core::sc_in  <sc_core::sc_logic>& con, sc_core::sc_module_name n) : in_read_check_ (false), update_time_(0), state_ (master_state_ -1)
{one_bit_ = new sc_core::sc_signal_resolved ("one_bit_"); con.bind (*one_bit_); one_bit_->write (sc_dt::Log_Z);}
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#if defined (TEAL_SC_FOREIGN)
teal::vreg_internal::vreg_internal (sc_core::sc_out <sc_core::sc_logic>& con, sc_core::sc_module_name n): in_read_check_ (false), update_time_(0), state_ (master_state_ -1)
{
  one_bit_ = new sc_core::sc_signal_resolved ("one_bit_"); 
  con.bind (*one_bit_); 
  one_bit_->write (sc_dt::Log_Z);
}
#endif

#if defined (TEAL_SC_FOREIGN)
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//HUmm hookup to the new one, or just use its value???
vreg_internal& teal::vreg_internal::operator= (const vreg_internal& v)
{
  //WARNING!!! NOT TESTED YET!!!!!!!

  in_read_check_  = false;
  wires_.clear ();
  one_bit_ = v.one_bit_;
  for (int i (bit_length()); i > 0; --i) {
    wires_.push_back (v.wires_[i]);
  }
  return *this;
}
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//steal the value
vreg_internal& teal::vreg_internal::operator= (const reg& r) {
  //    std::cout << MARK << " ::operator= begin set to " << r << std::endl;
  this->reg::operator= (r);
  //    std::cout << MARK << " ::operator= done " << *this << std::endl;
  return *this;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg_internal::~vreg_internal ()
{
  //	teal::vout x(""); x << teal_info << " 000 " << endm;
}

uint32 teal::vreg_internal::master_state_(0);



//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//if single bit, just read() and get that value, if multi-bit 
//gather them all up and use that value
void teal::vreg_internal::read_check () const
{
  // if (bit_length() == 9)    std::cout << MARK << " read_check begin " << *this << std::endl;
  vreg_internal* nc = (vreg_internal*)this;
  nc->read_check2 ();
  state_ = master_state_;
  //  if (bit_length() == 9)    std::cout << MARK << " read_check end " << *this << std::endl;
}



//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg_internal::invalidate_all_vreg_internals ()
{
  master_state_++;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//if single bit, just read() and get that value, if multi-bit 
//gather them all up and use that value
void teal::vreg_internal::read_check2 ()
{
  if (vtime() == update_time_) return;
  update_time_ = vtime();

  if (in_read_check_) return; //just for debug printing
  in_read_check_ = true;

  //      std::cout <<MARK << "read_check2 begin " << *this << std::endl;

  //propigate down to reg manually, not via its interfaces otherwise another read check will occur!
  if (bit_length() == 1) {
    teal_acc_vecval_[0].aval = ((one_bit_rd_->read() == sc_dt::Log_1) || (one_bit_rd_->read() == sc_dt::Log_X) ? 1:0);
    teal_acc_vecval_[0].bval = ((one_bit_rd_->read() == sc_dt::Log_X) || (one_bit_rd_->read() == sc_dt::Log_Z) ? 1:0);
  }
  else {
    //first clear, so only concerned about 1,X, Z
    for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i] = 0;}      
    for (uint32 i (0); i < bit_length(); ++i) {
      sc_dt::sc_logic_value_t data = (wires_[i]->one_bit_rd_->read()).value ();
      if ((data == sc_dt::Log_1) || (data == sc_dt::Log_X)) {
	teal_acc_vecval_[which_word_(i)].aval |= mask_bit_ (i);
      }
      if ((data == sc_dt::Log_X) || (data == sc_dt::Log_Z)) {
	teal_acc_vecval_[which_word_(i)].bval |= mask_bit_ (i);
      }
    }
  }

  in_read_check_ = false;
  //    std::cout << MARK << "read_check2 end " << *this << std::endl;
}



//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg_internal::write_through () const {
  //  std::cout << MARK << " write_through()  this =" << (uint*)this << " " << *this << std::endl;

  if (bit_length() == 1) {
    update_time_ = vtime ();  //prevent read_check() from trashing the value (sc return previous time value)
  
    char aval = teal_acc_vecval_[0].aval;
    char bval = teal_acc_vecval_[0].bval;
    sc_dt::sc_logic_value_t data;
    if (!bval) {
      data = aval ? sc_dt::Log_1 : sc_dt::Log_0;
    }
    else data = aval ? sc_dt::Log_X : sc_dt::Log_Z;
      
    one_bit_wr_->write (data);
  }
  else {
    if (update_time_ == vtime ()) { //remove echo from self. vbus will set the big one
      return;
    }
    for (uint32 i(0); i < bit_length(); ++i) {
      char aval = (teal_acc_vecval_[which_word_(i)].aval >> (i % 32)) & 1;
      char bval = (teal_acc_vecval_[which_word_(i)].bval >> (i % 32)) & 1;
      sc_dt::sc_logic_value_t data;
      if (!bval) {data = aval ? sc_dt::Log_1:sc_dt::Log_0;}
      else data = aval ? sc_dt::Log_X : sc_dt::Log_Z;

      (wires_[i])->one_bit_wr_->write (data);
    }
  }
  //   std::cout << MARK << " teal::vreg_internal::write_through()  done this =" << (uint*)this << " " << *this << std::endl;
}

#undef MARK

