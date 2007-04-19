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
#ifndef __truss_test_component__
#define __truss_test_component__



namespace truss {

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Run the phases off of the start method
  //basic exercisor, usually a fixed length
  class test_component : protected verification_component, protected virtual thread {
  public:
    test_component (const std::string& n) : thread (n), verification_component (n), name (thread::name_),  completed_ (true) {}

    virtual void time_zero_setup () = 0;
    virtual void out_of_reset (reset) = 0;
    virtual void randomize () = 0;
    virtual void write_to_hardware () = 0;

    void start () {thread::start ();}; 
    void stop () {thread::stop ();};  

    void wait_for_completion () {wait_for_completion_(); completed_ = true;}

    void report (const std::string prefix) const {
      if (completed_) {	log_ << teal_debug << prefix << " Completed as expected." << teal::endm; }
      else {log_ << teal_error << prefix << " Did not complete!" << teal::endm; }
    }
    const std::string& name;

  protected:
    virtual void  start_ () {
      log_ << teal_debug << "start_() for test_component  begin" << teal::endm;
      completed_ = false;
      start_components_ ();
      run_component_traffic_ ();
      log_ << teal_debug << "start_() for test_component  end" << teal::endm;
    }

    virtual void run_component_traffic_ () {randomize (); generate ();}
    virtual void start_components_ () = 0;
    //generate some data 
    virtual void generate () = 0;
    virtual void wait_for_completion_ () = 0;

    bool completed_;

  private:
    test_component (const test_component&);  
    test_component& operator= (const test_component&);
  };


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class irritator : public virtual test_component {
  public:
    irritator (const std::string& n) : thread (n), test_component (n), generate_ (false){}  
    virtual ~irritator () {}

    void stop_generation () {generate_ = false;}

  protected:
    virtual void  start_ () {
      log_ << teal_debug << "start_() for irritator begin" << teal::endm;
      generate_ = true;
      test_component::start_ ();
      log_ << teal_debug << "start_() for irritator end" << teal::endm;
    }

    virtual void run_traffic_ () {
      log_ << teal_debug << "run_trafic_() for irritator begin" << teal::endm;
      while (continue_generation ()) {
	test_component::run_component_traffic_ ();
	inter_generate_gap ();
      }
      log_ << teal_debug << "run_trafic_() for irritator end" << teal::endm;
    }


  protected:
    virtual bool continue_generation () {return generate_;}

    //subclass to do things like manage an amount in_flight, random delay
    //CAUTION: It's generally a good idea to have this method consume time. Otherwise, an infinate loop can occur.
    //At least one of the following three methods MUST consume time: 
    //                      continue_generation(), run_traffic() [i.e randomize(); generate();], or inter_generate_gap()
    virtual void inter_generate_gap () = 0;

    bool generate_;
  };


};
#endif
