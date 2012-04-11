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
#ifndef __truss_vout__
#define __truss_vout__

#include "truss_shutdown.h"
#include "teal.h"

namespace truss {

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class error_limit_vlog : public teal::vlog {
  public:
    error_limit_vlog (teal::uint32 limit, truss::shutdown* s) : limit_ (limit), shutdown_ (s), triggered_(false) {};
  protected:
    virtual void output_message (const teal::message_list & msg) {
      teal::vlog::output_message (msg);
      if ((!triggered_) && (how_many (vlog::error) >= limit_)) { 
	triggered_ = true; 
	shutdown_->shutdown_now ("Error Threashold reached."); 
      }
    }
			     
    virtual teal::message_list output_message_ (const teal::message_list& m) {return m;}
    virtual std::string local_print_ (const std::string& val) {return val;} 

  private:
    teal::uint32 limit_;
    truss::shutdown* shutdown_;
    bool triggered_;
  };


#if 0
  //make it better before release
  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  class exclude_vlog : public vlog {
  public:
    exclude_vlog () {};
    void exclude (int id, const std::string& val = "") {excludes_.push_back (message (id, val));};
  protected:
    virtual message_list output_message_ (const message_list& m);
    virtual std::string local_print_ (const std::string& val) {return val;}
  private:
    message_list excludes_;
  };

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::message_list teal::exclude_vlog::output_message_ (const teal::message_list& m)
{
  //  for (message_list_iterator exclude_it (excludes_.begin()); (exclude_it != excludes_.end()); ++exclude_it) {
    teal::message_list_iterator it (std::find_first_of (m.begin (),m.end (), excludes_.begin(), excludes_.end(),match_message()));
    if (it != m.end()) {
      return message_list ();
    }
    //  }
	 return m;
}
#endif


};

#endif
