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


namespace {
  static vout local_log ("Teal::vreg");


#if defined (vpi_2_0)
  vpiHandle connect__ (const std::string & path) {
    return (vpi_handle_by_name ((char*) path.c_str (), 0)); 
  }
#else
  handle connect__ (const std::string & path) {
    return (acc_handle_by_name ((char*) path.c_str (), 0));
  }
#endif
};

uint32 teal::vreg::master_state_(0);


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
bool teal::vreg::present (const std::string& path_and_name)
{
  return (connect__ (path_and_name) == 0 ? 0 : 1); //for older gcc compilers
  //  return (connect__ (path_and_name));
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::operator<< (vout& v, const vreg& s)
{
  //  v << s.path_and_name_ << " " << (reg&)s;
  //  return v;
  return   s.operator<< (v);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vout& teal::vreg::operator<< (vout& v) const
{
  v << path_and_name_ << " " ;
  return this->reg::operator<<(v);
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//Note: (1/31/2004 mfm) added static mutex because some simulators do not like 
//more than one thread making calls.

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg::vreg (const std::string& path_and_name) :  
  path_and_name_ (path_and_name), handle_ (0),
  state_ (master_state_ -1), enabled_ (path_and_name != "")
{
  if (enabled_) connect_ ();
  //   local_log << teal_note  << " vreg ctor at " << (long)this << " " << *this << endm;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg::vreg (const vreg& v) :  
  path_and_name_ (v.path_and_name_), 
  handle_ (0), enabled_ (v.enabled_)
{
  //  local_log << teal_note  << " vreg copy ctor at " << (long)this << " " << *this << endm;
  if (enabled_) connect_ ();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
vreg& teal::vreg::operator= (const vreg& v)
{
  path_and_name_ = v.path_and_name_;
  enabled_ = v.enabled_;
  if (enabled_) connect_ ();
  return *this;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::name (const std::string& path_and_name) 
{
  if (enabled_) disconnect_ ();
  path_and_name_ = path_and_name;
  enabled_ = path_and_name_ != "";
  if (enabled_) connect_(); else disconnect_ ();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::disconnect_ ()
{
  handle_ = 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::connect_ () {

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock (&mutex);


  handle_ = connect__ (path_and_name_);
  //        local_log << teal_note << " handle for " << path_and_name_ << " is  " << (int) handle_ << endm;

#if defined (vpi_2_0)
  if (! handle_) {
    local_log << teal_error << "Unable to get handle for \"" << path_and_name_ << "\" size:" << path_and_name_.size () << endm;
    vpi_control (vpiFinish);
  }

  int type (vpi_get (vpiType, handle_));
  if ((type != vpiReg) && (type != vpiNet)) {
    local_log << teal_error  << " vreg() " << path_and_name_ << " is not a register (" << type << "). Operation may fail." << endm;
  }

  write_policy_ = (type == vpiNet) ? vpiForceFlag : vpiInertialDelay;
#if defined (ivl) 
  if (type == vpiReg) immediate_write (true); //behavour difference, ivl does not give immediate acc_change value for 0 inertial delay
#endif

  resize (vpi_get (vpiSize, handle_));
#else
  if (! handle_) {
        local_log << teal_error << "Unable to get handle for " << path_and_name_ << endm;
    tf_dofinish ();
  }

  int type (acc_fetch_type (handle_));
    if ((type != accReg) && (type != accNet)) {
      local_log << teal_error  << " vreg() " << path_and_name_ << " is not a register.Type is: "
		<< type << " Operation may fail." << endm;
      }
  write_policy_ = (type == accNet) ? accForceFlag : accInertialDelay;
#if defined (ivl) 
  if (type == accReg) immediate_write (true);
#endif


  resize (acc_fetch_size (handle_));
#endif
  pthread_mutex_unlock (&mutex);
  state_ = master_state_ - 1; //force a read;
  //    local_log << teal_note  << " vreg connect at " << (long)this << " type is: " << type << " " << *this << endm;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#if defined (vpi_2_0)
teal::vreg::vreg (vpiHandle h) :  
  path_and_name_ (vpi_get_str (vpiFullName, h)),
  handle_ (h),
  state_ (master_state_ - 1), //force a read;
  enabled_ (true)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock (&mutex);

  int type (vpi_get (vpiType, handle_));
  if (type != vpiMemoryWord) {
    local_log << teal_error  << " vreg() " << path_and_name_ << " is not a memory word (" << type << "). Operation may fail." << endm;
  }
  write_policy_ = (type == vpiNet) ? vpiForceFlag : vpiInertialDelay;

  resize (vpi_get (vpiSize, handle_));
  pthread_mutex_unlock (&mutex);
  read_check ();
  //  local_log << teal_note  << " vreg(handle) ctor at " << (long)this << " " << *this << endm;
}
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
teal::vreg::~vreg ()
{
  //  vout::get (note) << " vreg dtor at " << (long)this << " " << *this << endm;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::read_check () const
{
    if (!enabled_) return;
#if 1
          if (state_ != master_state_) {
#else
    if (handle_){
#endif
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock (&mutex);

#if defined (vpi_2_0)
    s_vpi_value v;
    v.format = vpiVectorVal;
    vpi_get_value (handle_, &v);

    for (uint32 i(0); (i < word_length_); i++) {
      teal_acc_vecval_[i].aval = v.value.vector[i].aval;
      teal_acc_vecval_[i].bval = v.value.vector[i].bval;
    }    
#else
    s_acc_value v;
    v.format = accVectorVal;
    v.value.vector = teal_acc_vecval_;
    acc_fetch_value (handle_, "%%", &v);
#endif

    //(some) simulators weirdness, X's may be above our bits, if not on 32 bit boundry
    if (bit_length_ % 32) {
      teal_acc_vecval_[word_length_-1].aval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
      teal_acc_vecval_[word_length_-1].bval &= ~ (~0UL << (bit_length_ % 32)); //clean above, make math easier
    }

    state_ = master_state_;
    pthread_mutex_unlock (&mutex);
    //    local_log << teal_note  << " read_check at end " << *this << endm;
  }
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::invalidate_all_vregs ()
{
  master_state_++;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
vreg& teal::vreg::operator= (const reg& r)
{
  //    local_log << teal_note  << " operator=" << path_and_name_ << " (reg) enabled: " << enabled_ << " rhs " << r << endm;
  if (! enabled_) return *this;
  reg::operator= (r);
  return *this;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void teal::vreg::write_through () const
{
  if (!enabled_) return;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock (&mutex);
  //WARNING! Do not call operator<<() in here, it will bash the value!!!
  //  vout a_log ("vreg::write_through"); a_log << teal_note  << " write_through() at begin (policy is " << write_policy_ << " ) " << path_and_name_ << endm;
#if defined (vpi_2_0)
  s_vpi_value v;
  v.format = vpiVectorVal;
  v.value.vector = (s_vpi_vecval*)teal_acc_vecval_;
  s_vpi_time delay;
  delay.type = vpiSimTime;
  delay.low = 0;
  delay.high = 0;
  vpi_put_value (handle_, &v, &delay, write_policy_);
  s_vpi_error_info error_data;
  if (vpi_chk_error (&error_data)) {
    local_log << teal_error  << "Error in vpi_put_value: " << error_data.message << endm;
    local_log << teal_error  << "Error in vpi_put_value:  at " << error_data.file << " " << error_data.line << endm;
  }
#else
  s_acc_value v;
  v.format = accVectorVal;
  v.value.vector = teal_acc_vecval_;
  s_setval_delay delay;
  delay.model = write_policy_;
  delay.time.type = accSimTime;
  delay.time.low = 0;
  delay.time.high = 0;
  int result (acc_set_value (handle_, &v, &delay));
  if (result) {
    local_log << teal_error  << " at write_through result = " << result << " " << *this << endm;
  }
#endif
  state_ = master_state_;
  pthread_mutex_unlock (&mutex);
  //    local_log << teal_note  << " write_through() at end (policy is " << write_policy_ << " ) " << *this << endm;
}


