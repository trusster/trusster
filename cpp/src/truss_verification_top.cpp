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

#include "truss.h"


#ifndef TEST
#error "TEST undefined! cannot build a test!"
#endif

#ifndef SEED
#define SEED 1
#endif

#include TEST_H

using namespace truss;

namespace {
  truss::shutdown* a_shutdown;
};

namespace truss {

  void shutdown::shutdown_now (const std::string reason) {
    //start a final_report phase (which can be called by the watchdog timer.)
    //at the start opf this phase, tell the regression object to start logging
    static bool sentry = false;
    if (sentry) {
      teal::vlog::get().local_print ("Shutdown called while shutting down! Panic exit simulation.");
      teal::finish ();
    }
    sentry = true;

    {
      //   disable_at_sentry no_at = disable_at_sentry ();
      //at_disable (true);
      truss_assert (testbench_);
      testbench_->report (reason);

      truss_assert (test_);
      test_->report (reason);

      truss_assert (watchdog_);
      watchdog_->report (reason);
    }
    
    if (teal::vlog::get().how_many (teal::vlog::error) == 0) {
      log_ << teal_info << "Test " << test_->name << " Passed." << teal::endm;
    }
    else {
      log_ << teal_info << "Test " << test_->name << " Failed: Contained " << teal::dec << teal::vlog::get().how_many (teal::vlog::error) << " errors." << teal::endm;
    }
    teal::finish ();
  };
};



///////////////////////////////////////////////
///////////////////////////////////////////////
void verification_top ()
{
  std::string test_name = TEST_NAME;
    std::string file_name (teal::dictionary::find_on_command_line ("dictionary", test_name + ".cfg"));
    teal::dictionary::read (file_name);   //In code, use deafult to allow the read in values to stand

    a_shutdown = new shutdown ("Shutdown"); //ptrs set up a few lines down, after all built
    teal::file_vlog not_used = teal::file_vlog (teal::dictionary::find ("out_file"), teal::dictionary::find ("interactive", true));
    truss::error_limit_vlog not_used_2 = truss::error_limit_vlog (teal::dictionary::find ("error_limit", 10), a_shutdown);

    teal::vout log ("verification_top:"); 
    log.show_debug_level (99); 

    log << teal_info << "This is truss version \"" << truss_version << "\"" << teal::endm;

    std::string top = teal::dictionary::find_on_command_line ("truss_hdl_top", "top");
    if (top == "") top = "top";

    testbench* testbench_0 = new testbench (top);  
    watchdog*  watchdog_0 = new watchdog ("watchdog", top + ".watchdog",  a_shutdown); 
    TEST* test_0 = new TEST (testbench_0, watchdog_0, teal::dictionary::find_on_command_line ("test_name", TEST_NAME)); 

    a_shutdown->test_ = test_0;
    a_shutdown->testbench_ = testbench_0;
    a_shutdown->watchdog_ = watchdog_0;



    log << teal_debug << "Using dictionary file: " << file_name << teal::endm;

    teal::vrandom::init_with_seed (teal::dictionary::find ("seed", SEED));
    log << teal_info << "Using seed: " << teal::dec << teal::dictionary::find ("seed", SEED) << teal::endm;
 
    //run the watchdog through its dance to make sure it catches any hang conditions
    watchdog_0->time_zero_setup ();
    watchdog_0->out_of_reset (verification_component::cold);
    watchdog_0->write_to_hardware ();
    watchdog_0->start ();


    log << teal_debug << "About to randomize" << teal::endm;

    test_0->randomize ();          //first to allow to setup testbench
    testbench_0->randomize ();


    log << teal_debug << "About to time_zero" << teal::endm;
    {
      //   disable_at_sentry no_at = disable_at_sentry ();
      testbench_0->time_zero_setup ();
      test_0->time_zero_setup ();  //test last to be able to undo incorrect testbench setup
    }

    log << teal_debug << "About to out of reset" << teal::endm;

    testbench_0->out_of_reset (verification_component::cold);
    test_0->out_of_reset (verification_component::cold);



    log << teal_debug << "About to write to hardware" << teal::endm;

    testbench_0->write_to_hardware ();
    test_0->write_to_hardware ();

    log << teal_debug << "About to start" << teal::endm;

    testbench_0->start ();

    test_0->start ();

    log << teal_debug << "About to wait for completion." << teal::endm;

    testbench_0->wait_for_completion ();
    test_0->wait_for_completion ();

    log << teal_debug << "About to report" << teal::endm;
    a_shutdown->shutdown_now ("Final Report: ");
}
std::string truss::truss_version = "teal_1.50a";

#include "teal_hdl_connect.cpp"

