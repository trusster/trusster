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
#include <algorithm>

typedef sc_core::sc_process_handle real_thread_id;
#include <systemc>
#include <sc_join.h>

using namespace teal;

//sometimes its unsafe to use teal's logger
#define MARK std::cout << __FILE__ << " " << __LINE__ << std::endl;


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::uint64 teal::vtime ()
{
  if (!sc_core::sc_get_curr_process_handle ()) return 0;  //dumb ol systemc
  //  sc_core::sc_process_handle h = sc_core::sc_get_cur_process_handle (); 
  //  sc_core::sc_time t (h->time_stamp ());
  sc_core::sc_time t (sc_core::sc_time_stamp ());
  sc_dt::uint64 v (t.value());
  return (teal::uint64) v;
}

std::vector<real_thread_id> real_threads;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::finish () {
  static bool in_finish = false;
  if (in_finish) return;
  in_finish = true;

  //      SC_REPORT_FATAL (41062, "TEST FAILED");
  teal::vout local_vout ("finish");
  real_threads.clear();
  local_vout << teal_info << "Simulation end!" << teal::endm;
  //  exit(0);
  //The next two lines won't cause the deadman to finish if we run out of reservable resources
  sc_core::sc_stop();
  //SC_REPORT_ERROR (41062, "Teal: finish called");
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
std::string teal::current_thread_name ()
{
  //see also in sc_simcontext.h sc_get_curr_simcontext()
  sc_core::sc_process_b* h = sc_core::sc_get_curr_process_handle (); 
  return h->name ();
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//

struct utility {
  utility (teal::user_thread thread, void* user_data) :
    user_thread_(thread), user_data_ (user_data) {}

  void operator() () {
    user_thread_ (user_data_);
  }
  teal::user_thread user_thread_;
  void* user_data_;
};



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
#    include "sc_dynamic_processes.h"
thread_id teal::start_thread (user_thread thread, void* user_data, const std::string & name)
{
  //  thread_id id = sc_spawn <utility> (utility (thread, user_data), name.c_str());
  //  thread_id id = sc_core::sc_spawn (sc_bind (thread, user_data), name.c_str());
  real_thread_id id = sc_core::sc_spawn (sc_bind (thread, user_data), name.c_str());
  real_threads.push_back (id);
  //  local_vout << teal_info << "Thread " << name << " created. ID is " << hex << 
  //   thread_int (id) << " result " << result << endm;
  return real_threads.size();
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void teal::at (const sensitivity& s)
{
  if (! s.list.size()) {
  teal::vout local_vout ("at");
    local_vout << teal_error << "teal::at() called with no vregs! Ignored." << endm;
    return;
  }
  sc_core::sc_event_or_list& something_changed = (*s.list.begin())->get_event () | (*s.list.begin())->get_event ();
  for (std::vector<const vreg_match*>::const_iterator it (s.list.begin()+1); it != s.list.end(); ++it) {
    something_changed | (*it)->get_event ();
  }
    wait (something_changed);
}


//need to dig into the wonderful systemc macros and see what they do
void* teal::stop_thread (thread_id){
}
void* teal::join_thread (thread_id){
}
void teal::note_thread_completed (){
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
const sc_core::sc_event& teal::vreg_match::get_event () const {
  return bit_position_ ? the_vreg_[bit_position_]->value_changed_event() : the_vreg_.value_changed_event();
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
const sc_core::sc_event& teal::negedge::get_event () const {
  return bit_position_ ? the_vreg_[bit_position_]->negedge_event() : the_vreg_.negedge_event();
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
const sc_core::sc_event& teal::posedge::get_event () const {
  return bit_position_ ? the_vreg_[bit_position_]->posedge_event() : the_vreg_.posedge_event();
}
