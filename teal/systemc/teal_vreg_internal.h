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
//Note: This is a stand-alone header file, but part of the Teal release.

//It is used for connecting to systemC.

#if ! defined (__teal_vreg_internal_h__)
#define __teal_vreg_internal_h__

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////// Vreg_Internal /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  class signal_match;
// class vout;

  class vreg_internal : public reg {
  public:
    //WHY SC_MODULE NAME???
    //    vreg_internal (const std::string& path_and_name, signal_control control, sc_core::sc_module_name);
    vreg_internal (const std::string& path_and_name, teal::vreg::signal_control x);
    virtual ~vreg_internal ();

    //TDY -- add a ctor that takes std::vector<vreg_internal*> to allow building of new vreg_internals

    virtual vreg_internal* operator[] (int index) {if (bit_length() >= 2) return wires_[index]; else return this;}

    std::string name () const {return path_and_name_;}

    vreg_internal& operator= (const reg&);

    void write (uint64 val) { *this = val;}

    /*override*/ void read_check () const; 
    /*override */ void write_through () const;

    //logically private (with the change objects as friends), but may be useful
    virtual const sc_core::sc_event& value_changed_event () const {return one_bit_wr_->value_changed_event();};
    virtual const sc_core::sc_event& posedge_event ()       const {return one_bit_wr_->posedge_event();};
    virtual const sc_core::sc_event& negedge_event ()       const {return one_bit_wr_->negedge_event();};


    //called by the pli callback to change the global current state value.
    static void invalidate_all_vreg_internals ();
    
    vreg_internal& operator= (const vreg_internal&);

   //disambiguifiers for systemC bind muck
    teal::reg_slice operator() (uint32 u, uint32 l) {return this->teal::reg::operator() (u,l);}
   teal::reg operator() (uint32 u, uint32 l) const {return this->teal::reg::operator() (u,l);}
   teal::reg::four_state operator() (uint32 b) const {return teal::reg::operator() (b);}

    explicit vreg_internal (sc_core::sc_module_name n);

  protected:


    std::string path_and_name_;
    mutable teal::uint64 update_time_; //needed by the vbus logic
    virtual void read_check2 ();

    virtual teal::vout& operator<< (teal::vout& c) const;
    mutable std::vector<vreg_internal*> wires_;
    mutable sc_core::sc_signal_resolved * one_bit_rd_;
    mutable sc_core::sc_signal_resolved * one_bit_wr_;

    mutable sc_core::sc_signal_resolved * one_bit_;
    mutable bool in_read_check_;

  private:
    mutable uint32 state_;
    static uint32 master_state_;
    bool enabled_;

    friend teal::vout& operator<< (teal::vout&, const vreg_internal&);
  };
 teal::vout& operator<< (teal::vout&, const vreg_internal&);
#endif
