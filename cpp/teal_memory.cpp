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
  vout local_log ("Teal::memory", teal::no_debug);
};



///////////////////////////////////////////////
///////////////////////////////////////////////
//PLI LAND CALLBACK!!!
#if defined (vpi_2_0)
//should this be in the unnamed namespace ?

///////////////////////////////////////////////
///////////////////////////////////////////////
class regular_memory_bank_2_0 : public memory_bank 
{
public:
  regular_memory_bank_2_0 (vpiHandle);
  
  /*override*/ void from_memory (uint64, reg*);
  /*override*/ void to_memory (uint64, const reg&);

private:
  uint32 bit_length_;
  uint32 bytes_per_group_; //divide this by 2 to get number of aval/bvals
  uint32 last_memory_address_; 
  uint8* aval_start_; //bval start is calculated as we need it.
  vpiHandle handle_;
};
#endif

#if defined (vpi_2_0)
///////////////////////////////////////////////
///////////////////////////////////////////////
#if defined (vcs)
void teal_memory_note_call_2_0 (int user_data, int reason)
#else
int teal_memory_note_call_2_0 (char*)
#endif
{
    //Is there an easier way?
  vpiHandle my_call = vpi_handle (vpiSysTfCall, 0);
  vpiHandle it = vpi_iterate (vpiArgument, my_call);
  vpiHandle first_arg = vpi_scan (it); //could make it a loop, but how to do in 1.0?
  banks_.push_back (new regular_memory_bank_2_0 (first_arg));
  return 0;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
void teal_memory_note_register ()
{
  s_vpi_systf_data task_data = {0};
  task_data.type = vpiSysTask;
  task_data.tfname = "$teal_memory_note"; 
#if defined (cver)
  task_data.calltf = (p_tffn) teal_memory_note_call_2_0;
#else
  task_data.calltf = teal_memory_note_call_2_0;
#endif
  task_data.compiletf = 0;
  vpi_register_systf (&task_data);
}

//extern "C" void teal_memory_note_call_1_0 (int,int);
//void teal_memory_note_call_1_0 (int user_data, int reason) {assert (0);}

#else
//PLI 1.0 Land!!!

///////////////////////////////////////////////
///////////////////////////////////////////////
//NOTE: Cannot make a reg of the correct length and make 
//a reg ctor that can take in a pointer to the acc_vecval,
//because memory is stored a groups of characters, which may
//or may not be 4 (i.e. 32 bit grouping)
//
//TO CONSIDER: Insteado f returning a reg(), return a subclass that is "electric".
//This subclass would handle the operator=() and read_check() much like vreg,
//except with memory symentics.
//
//TO CONSDER: What is the best way to handle un aligned read/writes, if at all?
///////////////////////////////////////////////
///////////////////////////////////////////////
class regular_memory_bank : public memory_bank 
{
public:
  regular_memory_bank (const std::string& path);
  
  /*override*/ void from_memory (uint64, reg*);
  /*override*/ void to_memory (uint64, const reg&);

private:
  uint32 bit_length_;
  uint32 bytes_per_group_; //divide this by 2 to get number of aval/bvals
  uint32 last_memory_address_; 
  uint8* aval_start_; //bval start is calculated as we need it.
};


///////////////////////////////////////////////
///////////////////////////////////////////////
//extern "C" void teal_memory_note_call_1_0 (int,int);
void teal_memory_note_call_1_0 (int user_data, int reason)
{
  banks_.push_back (new regular_memory_bank (tf_mipname()));
}
#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
void teal::memory::add_map (const std::string& path, uint64 first_address, uint64 last_address)
{
  iter it (banks_.begin ());
  int found (0);
  for (; it != banks_.end (); ++it) {
    if ((*it)->contains (path)) {
      if ((*it)->first_address) {
	local_log << teal_error << "Remapping memory at " << (*it)->path_ << " to address " << first_address << endm;
      }
      else {
	local_log << teal_debug << "Mapping memory at " << (*it)->path_ << " to path " << path  << " with start address " << first_address << endm;
      }
      (*it)->first_address = first_address;
      (*it)->last_address = last_address;
      found = 1;
      //      break; //to catch multiple maps
    }
  }
  
  if (!found) {
    local_log << teal_error << "No mapping for memory at " << path << endm;
  }
}


///////////////////////////////////////////////
///////////////////////////////////////////////
void teal::memory::read (uint64 global_address, reg* returned)
{
  iter it (banks_.begin ());
  for (; it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
      (*it)->from_memory (global_address - (*it)->first_address, returned);
      return;
    }
  }
  local_log << teal_error << "Unable to read memory at " << global_address << endm;
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void teal::memory::write (uint64 global_address, const reg& value)
{
  for (iter it (banks_.begin ()); it != banks_.end (); ++it) {
    if ((*it)->contains (global_address)) {
      (*it)->to_memory (global_address - (*it)->first_address, value);
      return;
    }
  }
  local_log << teal_error << "Unable to write memory at " << global_address << endm;
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
	local_log << teal_error << "Duplicate memory at " << (*it)->path_ << " looking up with " << path << endm;
      }
      else {
	returned = *it;
      }
      //      break; //to catch multiple maps
    }
  }
  
  if (!returned) {
    local_log << teal_error << "Unable to lookup memory at " << path << endm;
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
	local_log << teal_error << "Duplicate memory at " << (*it)->path_ << " looking up with " << global_address << endm;
      }
      else {
	returned = *it;
      }
      //      break; //to catch multiple maps
    }
  }
  
  if (!returned) {
    local_log << teal_error << "Unable to lookup memory at " << global_address << endm;
  }
  return returned;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

#if defined (vpi_2_0)
///////////////////////////////////////////////
///////////////////////////////////////////////
regular_memory_bank_2_0::regular_memory_bank_2_0 (vpiHandle handle1) :
  memory_bank (vpi_get_str (vpiFullName, handle1)),
  handle_ (handle1)
{
  if (vpi_get (vpiType, handle_) != vpiMemory) {
    local_log << teal_error << " Verilog at " << path_ << " is not a memory model." << endm;
    vpi_control (vpiFinish);
  }
  last_memory_address_ = vpi_get (vpiSize, handle_);
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/*override*/ void regular_memory_bank_2_0::from_memory (uint64 address, reg* returned)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock (&mutex);

  if (address >= last_memory_address_) {
    local_log << teal_error << "On memory " << path_ << " read address: " << address << 
      " exceeds length " << last_memory_address_ << endm;
    pthread_mutex_unlock (&mutex);
    return ;
  }
  vpiHandle memory_word = vpi_handle_by_index (handle_, address);
  s_vpi_error_info error_data;
  if (vpi_chk_error (&error_data)) {
    local_log << teal_error  << "Error in regular_memory_bank_2_0::from_memory() " << error_data.message << endm;
    local_log << teal_error  << "Error in regular_memory_bank_2_0::from_memory():  at " << error_data.file << " " << error_data.line << endm;
  }

  assert (returned);
  *returned = vreg (memory_word); //call reg::reg op=()

  pthread_mutex_unlock (&mutex);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/*override*/ void regular_memory_bank_2_0::to_memory (uint64 address, const reg& data)
{
  if (address >= last_memory_address_) {
    local_log << teal_error << "On memory " << path_ << " write address: " << address << 
      " exceeds length " << last_memory_address_ << " Write ignored." << endm;
    return; //TO CONSIDER: throw (), vpi_finish ()
  }

  vreg temp = vreg (vpi_handle_by_index (handle_, address));
  temp.immediate_write (true);
  temp = data;
}


#else

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
//WARNING: ctor assumed to be called in the misc callback.
///////////////////////////////////////////////
///////////////////////////////////////////////
regular_memory_bank::regular_memory_bank (const std::string& path) :
  memory_bank (path)
{
  s_tfnodeinfo the_node;
    if (! tf_nodeinfo (1, &the_node)) {
      local_log << teal_error << " Unable to register memory at " << path << endm;
      return; //??throw ()?
    }

  if (the_node.node_type != TF_MEMORY_NODE) {
    local_log << teal_error << " Verilog at " << path << " is not considered memory." << endm;
    return;
  }

  path_ += ".";
  path_ += the_node.node_symbol;
  aval_start_ = (uint8*) the_node.node_value.memoryval_p; 
  last_memory_address_= the_node.node_mem_size;
  bytes_per_group_ = the_node.node_ngroups;
  bit_length_ = the_node.node_vec_size;
  local_log << teal_debug << "Registering memory " << path_ << " # bits: " << dec <<  bit_length_ << 
    " bank length is " << last_memory_address_ << endm;
    local_log << teal_debug << "Memory ctor: bytes_per_group_ " <<  bytes_per_group_ << 
      " bit_length_ : " << bit_length_ << hex << " aval_start: " << (uint32)aval_start_ << endm;
}


///////////////////////////////////////////////
///////////////////////////////////////////////
/*override*/ void regular_memory_bank::from_memory (uint64 address, reg* returned2)
{
  if (address >= last_memory_address_) {
    local_log << teal_error << "On memory " << path_ << " address: " << address << 
      " exceeds length " << last_memory_address_ << endm;
    return ; //TO CONSIDER throw ()
  }

  reg returned = reg (0, bit_length_);
  //assert (bit lebngths are equal, or would that be an incorrect assertion?)
#if 1
  uint8* aval = aval_start_ + (address * bytes_per_group_ * 2); //one group length for aval, one for bval
  uint8* bval = aval + bytes_per_group_; 

  uint32 current_aval = 0;
  uint32 current_bval = 0;
  uint8 shifter = 0; //goes from 0..3, at 4 the completed 32 bit word is put into the reg's vecval;
  uint32 i(0);

  for (; (i < bytes_per_group_); ++i, ++aval, ++bval) {
    current_aval += (*aval << (8*shifter));
    current_bval += (*bval << (8*shifter));
    //    local_log << teal_debug << "Memory read char " << i << " at " << hex << (uint32)aval << " aval is : " << current_aval << endm;
    //    local_log << teal_debug << "Memory read char " << i << " at " << hex << (uint32)bval << " bval is : " << current_bval << endm;
    
    if (++shifter == 4) {
      //local_log << teal_debug << "Memory read uint32 " << i << " aval is : " << current_aval << endm;
      shifter = 0;
      returned.teal_acc_vecval_[i/4].aval = current_aval;
      current_aval = 0;
      returned.teal_acc_vecval_[i/4].bval = current_bval;
      current_bval = 0;
    }
  }
  //assign last chars and clean to the word boundry
  if (bytes_per_group_ % 4) {
        local_log << teal_debug << "Memory read (dangling) char " << i << " aval is : " << current_aval << endm;
            local_log << teal_debug << "Memory read (dangling) char " << i << " bval is : " << current_bval << endm;
    returned.teal_acc_vecval_[i/4].aval = current_aval;
    returned.teal_acc_vecval_[i/4].bval = current_bval;
    i = 4 * ((i + 3) /4); //move to word boundry
  }

  //clean above vecval
  for (uint32 j = returned.bit_length_; j < (returned.word_length_ * 32); ++j) {
    local_log << teal_debug << "before cleaning bit  " << dec << j << " ab value is: " << bin 
	      << returned.teal_acc_vecval_[reg::which_word_ (j)].aval  
	      << "/" 
	      << returned.teal_acc_vecval_[reg::which_word_ (j)].bval << endm;
    returned.teal_acc_vecval_[reg::which_word_ (j)].aval &= ~ reg::mask_bit_ (j);
    returned.teal_acc_vecval_[reg::which_word_ (j)].bval &= ~ reg::mask_bit_ (j);
    local_log << teal_debug << "after cleaning bit  " << dec << j << " value is: " << bin << returned << endm;
  }

#endif
  //  local_log << teal_debug << "Memory read. about to return " << returned << endm;
  *returned2 = returned;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/*override*/ void regular_memory_bank::to_memory (uint64 address, const reg& data)
{
  reg returned = reg (0, bit_length_);
  returned = data; //clip/expand to our length
  uint8* aval = aval_start_ + (address * bytes_per_group_ * 2); //one group length for aval, one for bval
  uint8* bval = aval + bytes_per_group_; 

  uint8 shift = 0;
  for (uint32 i(0); (i < bytes_per_group_); ++i, ++aval, ++bval) {
    *aval = returned.teal_acc_vecval_[i/4].aval >> (8*shift);
    *bval = returned.teal_acc_vecval_[i/4].bval >> (8*shift);
    if (++shift == 4) {shift = 0; }
    //    local_log << teal_debug << "Memory write char " << i << " at " << hex << (uint32)aval << " aval is : " << (int)*aval << endm;
  }
}
#endif
  

