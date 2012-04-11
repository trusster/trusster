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
#include <fstream>
#include "stdio.h" //sprintf
#include <math.h>

using namespace teal;

const uint32 num_threads = 250; //273;
const uint32 num_iterations = 700; //1000; //600 for ModelTech, 700 for icarus

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
template <class data_t>
  data_t abs_f (data_t lhs) 
{
  return ((lhs < 0) ? -lhs : lhs);
}
		

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//  a feeble transactor
class simple_transactor {
public:
  simple_transactor (const std::string path) :
    path_ (path),
    op_code (path + ".op_code"),
    do_work (path + ".do_work"),
    work_done (path + ".work_done"),
    mutex_ ("Simple transactor: " + path),
    log_ (path)
  {

  }

  void execute (reg op_code1, uint32* complete_time)
  {
    //      log_ << teal_info << "about to execute" << endm;
    mutex_sentry guard (mutex_);
    //            log_ << teal_info << "about to execute got mutex " << do_work << endm;
    op_code = op_code1;
    do_work = 1;           //signal to verilog
    //        log_ << teal_info << "opcode set wait for work_done:" << work_done << endm;
    at (posedge (work_done)); //wait for ack
    *complete_time = vtime (); //assumes that the upper 32 are 0
    //      log_ << teal_info << " op done  " << " at " << *complete_time << endm;
  }

  const std::string path_;

private:
  vreg op_code;
  vreg do_work;
  vreg work_done;
  mutex mutex_;
  vout log_;
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
struct thread_context {
public:
  thread_context (const std::string& p, simple_transactor* st, uint32 my_id, bool* my_done, condition* c) : 
    path(p),
    simple_transactor_ (st),
    completed (c),
    done (my_done),
    id (my_id),
    log_ (p)
  {
    *done = false;
    for (uint32 j(0) ; j < 256; ++j) {
      histogram_8[j] = 0;
    }
  }


  ~thread_context () 
  {
    gaussian_check ();
    //       log_ << teal_info << " context dtor signalling done." << endm;
    if ((0) && (id < 30) && (id > 25)) {
      static pthread_mutex_t serial = PTHREAD_MUTEX_INITIALIZER;
      pthread_mutex_lock (&serial);
      char temp [256];
      sprintf (temp, "data_%d.txt", id);
      std::ofstream foo (temp);
      for (uint32 i(0); (i < 256); ++i) {
	//	std::cout << "histogram[" << i << "]: " << histogram_8[i] << std::endl;
	foo << histogram_8[i] << std::endl;
      }
      foo.close ();
      pthread_mutex_unlock (&serial);
    }
    reg my_total (0, 32);
    memory::lookup (simple_transactor_->path_)->from_memory (id, &my_total);
    log_ << ((my_total == num_iterations) ? teal_info : teal_error)
      << dec << " verilog total of  " << my_total << " compared to   "
      << num_iterations << endm;
    *done = true;
    completed->signal (); //notify parent
  }

  void gaussian_check () 
  {
    //check that distribution is normal
    //compute standard distribution and verify that rand is < 1 sigma
    uint64 mean(0);
    uint64 mean_of_squares (0);
    uint32 max_index (0);
    for (uint32 i(0); i < 256; ++i) {
      if (histogram_8[i]) max_index = i;
    }

    for (uint32 i(0); i < max_index; ++i) {
      mean += histogram_8[i];
      mean_of_squares += histogram_8[i] * histogram_8[i];
    }
    mean /= max_index;
    mean_of_squares /= max_index;
    double standard_deviation = sqrt (abs_f ((double) mean_of_squares - (mean*mean)));
    //std::cout << "mean " << mean << " mean of squares " << mean_of_squares << 
    //      " standard deviation: " << standard_deviation <<  std::endl;

    //    uint32 num_below_mean (0);
    uint32 num_within_one (0);
    uint32 num_within_two (0);
    for (uint32 i(0); i < max_index; ++i) {
      if (abs_f ((double)histogram_8[i] - (double)mean) <= standard_deviation) ++num_within_one;
      if (abs_f ((double)histogram_8[i] - (double)mean) <= (2 *standard_deviation)) ++num_within_two;
    }

    log_ << ((num_within_one >= (uint32)(.68 * max_index)) ? teal_info : teal_error)
      << dec << " of " << max_index << " bins "
      << num_within_one << " were within one standard deviation. Expected at least: " << (.68 * max_index) << endm;

    log_ << ((num_within_two >= (uint32)(.95 * max_index)) ? teal_info : teal_error)
      << " of " << max_index << " bins "
      << num_within_two << " were within two standard deviations. Expected at least: " << (.95 * max_index) << endm;
  }

  std::string path;
  simple_transactor* simple_transactor_;
  uint32 histogram_8[256];
  condition* completed;
  bool* done;
  uint32 id;
  vout log_;
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void* simple_thread (void* c) 
{
  thread_context* context = static_cast<thread_context*> (c);

  for (uint32 i(0); i < num_iterations; ++i) {
    uint32 time_taken;
    uint32 start_time = vtime ();
    context->simple_transactor_->execute (context->id, &time_taken);
    // context_->log_ << teal_info << " " << context->simple_transactor_->path_ << " start " << start_time << " end " << time_taken << endm;
    time_taken -= start_time;
    if (time_taken >= 256) {
      // context->log_ << teal_info << " Time taken > 256: " << time_taken << " Clipping." << endm;
      time_taken = 255;
    }
    ++(context->histogram_8[time_taken]);
    // context->log_ << teal_info << "Histogram at " << time_taken << " is " << dec << context->histogram_8[time_taken] << endm;
  }
  delete context;
  note_thread_completed ();
  return 0;
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
void verification_top ()
{
  vreg init_done ("top.init_done");
  at (posedge (init_done));

  vout log ("mutext test");

  file_vlog not_used = file_vlog (teal::dictionary::find ("out_file"), dictionary::find ("interactive", true));

  char temp[1024];
  simple_transactor* transactors[] = {
    new simple_transactor ("top.trans0"),
    new simple_transactor ("top.trans1"),
    new simple_transactor ("top.trans2"),
    new simple_transactor ("top.trans3"),
  };

  pthread_t threads[num_threads];
  bool thread_done[num_threads];
  condition completed ("an execute thread is done");

  for (uint32 i(0); i < num_threads; ++i) {
    uint32 x(0); RAND_RANGE (x, 0, 3);
    sprintf (temp, "simple thread %0d  on transactor %0d", i, x);
    
    threads[i] = start_thread (simple_thread, 
			     new thread_context ("top", transactors[x], i, &thread_done[i], &completed), temp);
  }

  //wait for all threads to complete their work
  bool all_done (false);
  while (! all_done) {
    completed.wait ();
            log << teal_info << "A thread completed. Scanning for all_done" << endm;
    uint32 j(0);
    for (; (j < num_threads) && (thread_done[j]); ++j) {
    }
    all_done = (j == num_threads);
            log << teal_info << " all done is : " << all_done << " j is " << j << endm;
  };

  if (vlog::get().how_many (vlog::error)) {
    log << teal_info << "Test Failed: Contained " << dec << vlog::get().how_many (vlog::error) << " errors." << endm;
  }
  else {
    log << teal_info << "Test Passed. "  << endm;
  }

  vreg test_done ("top.test_done");
  test_done = 1;

  //  note_task_completed ();
  finish ();
}

#include "../teal_hdl_connect.cpp"
