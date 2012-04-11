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
#ifndef __verification_component__
#define __verification_component__


#include <string>

#include "teal.h"

namespace truss {

  //The swiss army knife. Although common base classes are tricky, by using protected and private inheritance,
  //common implementation idioms can be expressed.
class verification_component {
 public: 
  verification_component (const std::string& n) :  name (name_), name_ (n), log_ (n) {}

  virtual ~verification_component () {log_ << teal_debug << " dtor. " << teal::endm;};

  virtual void randomize () = 0;

  //for non-time consuming tasks only - Caution: teal_at() is disabled
  virtual void time_zero_setup () = 0;

  typedef enum {cold, warm} reset;

  //take your part of the DUT out of reset
  //may NOT be called in a loop/rerun/multiple test scenerio
  virtual void out_of_reset (reset) = 0;

  //after this call, the component is ready for traffic 
  //(why not start of BFM calls this? , because generally usefull method, subcomponents)
  virtual void write_to_hardware () = 0;

  //on start(), do what makes sense for the object type.
  //for big picture verifictaion components, engage your configuration, start monitors and BFMs
  //do NOT asume that the resisters are at reset, unless you know that for sure
  virtual void start () = 0;

  //stop_all_threads, get back to the state just before the start call
  virtual void stop () = 0;

  //or not supposed to be in the generic thing? But needed in a checker
  //called after stop (automatically, or by owner?)
  virtual void wait_for_completion () = 0;  //or stop?

  //can mean different things, but generally report your state.
  virtual void report (const std::string prefix) const = 0;

  const std::string& name;

 protected:
  //ADD copy/assign privates, as appropriate
  const std::string name_; //should be non const?
  mutable teal::vout log_;
};

};

#endif
