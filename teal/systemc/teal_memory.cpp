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
using namespace teal::memory;

namespace {
  static std::deque</*owner*/memory_bank*> banks_;
  typedef std::deque<memory_bank*>::iterator iter;
  vout* local_log_internal;


  vout& get_local_log () {
    if (! local_log_internal) {
      local_log_internal = new vout ("Teal::memory", teal::no_debug);
    }
    return *local_log_internal;
  }

  bool chatty_;
};


bool teal::memory::chatty (bool new_value) {bool returned = chatty_; chatty_ = new_value; return returned;}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
teal::vout& operator<< (teal::vout& o, const memory_bank& m)
{
  o << "\"" << m.path_ << "\" [" << m.first_address << ":" << m.last_address << "]";
  return o;
}

std::string sreport (const memory_bank& m)
{
  std::ostringstream o;
  o << "\"" << m.path_ << std::hex <<  "\" [" << m.first_address << ":" << m.last_address << "]";
  return o.str();
}


///////////////////////////////////////////////
///////////////////////////////////////////////
void teal::memory::add_map (const std::string& path, uint64 first_address, uint64 last_address)
{
  iter it (banks_.begin ());
  int found (0);
  //first scn to see if any bank already has that address!
  for (; it != banks_.end (); ++it) {
    if ((*it)->contains (first_address)) {
      if ((*it)->first_address) {
	get_local_log () << teal_fatal << "bank at " << sreport (**it) << " already contains address " << first_address << endm;
      }
      if ((*it)->last_address) {
	get_local_log () << teal_fatal << "bank at " << sreport (**it) << " already contains address " << last_address << endm;
      }
    }
  }


  for (it = banks_.begin(); it != banks_.end (); ++it) {
    if ((*it)->contains (path)) {
      if ((*it)->first_address) {
	get_local_log () << teal_error << "Remapping memory at " << (*it)->path_ << " to address " << first_address << endm;
      }
      else {
	get_local_log () << teal_info << "Mapping memory at " << (*it)->path_ << " to path " << path  << " with start address " << first_address << endm;
      }
      (*it)->first_address = first_address;
      (*it)->last_address = last_address;
      found = 1;
      //      break; //to catch multiple maps
    }
  }
  
  if (!found) {
    get_local_log () << teal_error << "No mapping for memory at " << path << endm;
  }
}


///////////////////////////////////////////////
///////////////////////////////////////////////
void teal::memory::read (uint64 global_address, reg* returned)
{
  iter it (banks_.begin ());
  for (; it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
       if (chatty_) {
	memory_bank* foo = *it;
	get_local_log () << teal_info << " Read selected bank " << sreport (*foo) << " for address " << global_address << teal::endm;
      }
     (*it)->from_memory (global_address - (*it)->first_address, returned);
      return;
    }
  }
  get_local_log () << teal_error << "Unable to read memory at " << global_address << endm;
}



/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void teal::memory::write (uint64 global_address, const reg& value)
{
  for (iter it (banks_.begin ()); it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
      if (chatty_) {
	memory_bank* foo = *it;
	get_local_log () << teal_info << " Write selected bank " << sreport (*foo) << " for address " << global_address << teal::endm;
      }
      (*it)->to_memory (global_address - (*it)->first_address, value);
      return;
    }
  }
  get_local_log () << teal_error << "Unable to write memory at " << global_address << endm;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void teal::memory::write_blocked (uint64 global_address, const reg& value)
{
  for (iter it (banks_.begin ()); it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
      if (chatty_) {
	memory_bank* foo = *it;
	get_local_log () << teal_info << " Write selected bank " << sreport (*foo) << " for address " << global_address << teal::endm;
      }
      (*it)->to_memory_blocked (global_address - (*it)->first_address, value);
      return;
    }
  }
  get_local_log () << teal_error << "Unable to write memory at " << global_address << endm;
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void teal::memory::add_memory_bank (/*owner*/ memory_bank* mem) {banks_.push_front (mem);}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*cached*/ memory_bank* teal::memory::lookup (const std::string& path)
{
  iter it (banks_.begin ());
  memory_bank* returned = 0;

  for (; it != banks_.end (); ++it) {
    if ((*it)->path_.find (path) != std::string::npos) {
      if (returned) {
	get_local_log () << teal_error << "Duplicate memory at " << (*it)->path_ << " looking up with " << path << endm;
      }
      else {
	returned = *it;
      }
      //      break; //to catch multiple maps
    }
  }
  
  if (!returned) {
    get_local_log () << teal_error << "Unable to lookup memory at " << path << endm;
  }

  return returned;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*cached*/ memory_bank* teal::memory::lookup (uint64 global_address)
{
  iter it (banks_.begin ());
  memory_bank* returned = 0;

  for (; it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
      if (returned) {
	get_local_log () << teal_error << "Duplicate memory at " << (*it)->path_ << " looking up with " << global_address << endm;
      }
      else {
	returned = *it;
      }
      //      break; //to catch multiple maps
    }
  }
  
  if (!returned) {
    get_local_log () << teal_error << "Unable to lookup memory at " << global_address << endm;
  }
  return returned;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////


