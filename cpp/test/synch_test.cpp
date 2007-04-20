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

#if 0
./run -c -$SIM -clean -t synch_test
#endif

#include "teal.h"

using namespace teal;

struct thread_context {
  thread_context (const std::string& p) : path(p), log (p) {}
  std::string path;
  vout log;
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* add_one (void* c) 
{
  thread_context* context = static_cast<thread_context*> (c);
  vreg my_signal (context->path + ".top_signal");
  vreg my_variable (context->path + ".add_one_var");

  while (1) {
    at (posedge (my_signal));
    my_variable = my_variable + 1;
    context->log << teal_info << " " << my_variable.name() << " is now: " << my_variable.format_hex_string () << endm;
  }
  return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* toggle_bits (void* c) 
{
  thread_context* context = static_cast <thread_context*> (c);
  vreg clk (context->path+ ".clk");
  vreg my_variable (context->path + ".toggle_var");

  while (1) {
    at (negedge (clk));
    my_variable(0,0) = ! my_variable (0);
    context->log << teal_info << " " << my_variable.name() << " is now: " << my_variable.format_hex_string () << endm;
  }
  return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
namespace monitor {
  pthread_t id;

  void* monitor_thread (void* c) {
    thread_context* context = static_cast<thread_context*> (c);
    vreg var1 (context->path + ".add_one_var");
    vreg var2 (context->path + ".toggle_var");
    reg last_var1 = var1;
    bool last_var2 (0);
    vout log (context->log);

    while (1) {
      at (change (var1) ||  change (var2));

      if ((vtime () % 7) == 0) { //time for a var1 change
	log << ((var1 == (last_var1 + 1)) ? teal_info : teal_error)
	  << " change expected: " << var1.name () << " is " << var1.format_hex_string () 
	  << " last was " << last_var1.format_hex_string () << endm;
	context->log << ((var2 == last_var2) ? teal_info : teal_error)
	  << " change not expected: " << var2.name () << " is: " << var2.format_hex_string () 
	  << " last was " << last_var2 << endm;
	last_var1 = var1;
      }
      else {
	context->log << ((var1 == last_var1) ? teal_info : teal_error)
	  << " change not expected: " << var1.name () << " is " << var1.format_hex_string () 
	  << " last was " << last_var1.format_hex_string () << endm;
	log << ((var2 == !last_var2) ? teal_info : teal_error)
	  << " change expected: " << var2.name () << " is: " << var2.format_hex_string () 
	  << " last was " << last_var2 << endm;
	last_var2 = var2(0);
      }
    }
    log << teal_info << "monitor::returning?" << endm;
    return 0;
  }

  void start (const std::string& p) {
    //warning: must be "new'd" because the thread may run after this function (and the context will be dtor'd)
    thread_context* foo = new thread_context (p);
    monitor::id = teal::start_thread (monitor_thread, foo, "monitor_task");    
  }

  void stop () {
    stop_thread (monitor::id);
  }
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void verification_top ()
{
  vout log ("synch test");


  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  vreg init_done ("top.init_done");
  at (posedge (init_done));

  thread_context context_1 ("top");
  start_thread (add_one, &context_1, "add_one");
  thread_context context_2 ("top");
  start_thread (toggle_bits, &context_2, "toggle_bits");
  monitor::start ("top");

  //wait for done vreg 
  vreg simple_test_done ("top.simple_test_done");
  at (posedge (simple_test_done));
  vreg test_done ("top.test_done");
  test_done = 1;

  log << teal_info << endm;
  //kill all tasks
#if 0
  stop_thread (add_one_id);
  stop_thread (toggle_bits_id);
#else
  stop_all_threads ();
  log << teal_info << endm;
#endif
  monitor::stop ();
  log << teal_info << endm;
  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }
  finish ();
}

#include "../teal_hdl_connect.cpp"
