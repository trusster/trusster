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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////// Synchronization /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This is the type of function that is used to start an
  //independent thread of execution. This thread runs until
  //it get to either an at(treg) or a condition or a mutex.
  //
  typedef void* (*user_thread) (void* user_data);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This function actually runs an instance of the
  //user_thread declared above.
  //
  pthread_t start_thread (user_thread, void* user_data, const std::string & name);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  std::string thread_name (pthread_t);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This function cancels the thread and wait for it to terminate.
  //
  void* stop_thread (pthread_t);
  void* join_thread (pthread_t);
  void note_thread_completed (); //for threads that end themselves
  void stop_all_threads ();

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //return the curent simultaion time. (TODO - allow a precision?)
  uint64 vtime ();

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //forward for internal use
  class thread_release; //internal record-keeping class
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class vreg_match {
  public:
    vreg_match (vreg& v, const std::string n, uint32 bit_position) :
      the_thread_release (0), the_vreg (v), name (n), 
      time_at_creation_ (vtime()),
      bit_position_ (bit_position)
   {
     //  aldec_2_0 diestd::cout << "**************** aaaaaaaaaaaaaaa  ***********" << (int)(&the_vreg) << std::endl;
     the_vreg.read_check ();
     //aldec_2_0 die  std::cout << "**************** bbbbbbbbbbbbbbb  ***********" << std::endl;
     old_value_ = (the_vreg)(bit_position);
      //      vout::get ().local_print_ ("vreg_match ctor of "+  name+ "\n");;
      //      if (! the_vreg.enabled()) {
      //	vout temp ("teal::vreg_match");
      //	temp << error << "Attempt to wait on unconnected signal " << the_vreg << endm;
      //      }
    }
    virtual ~vreg_match () {
      //      vout::get (note) << "vreg_match dtor of " << name << endl;
    }

    //does the signal match the change requested?
    //has time moved and has the bit changed? (since vcl will return on any bit in the vector)
    virtual bool match () const {  
      the_vreg.read_check ();
#if SIM==ivl
      //icarus does assignments one bit at a time, 
      //and calls us back each change, so the time == creation
      //when we do the bit test (for anything other than the 0 bit)
      bool matched (((the_vreg)(bit_position_)) != old_value_);
#else
      bool matched ((vtime() != time_at_creation_)
		    && (((the_vreg)(bit_position_)) != old_value_));
#endif
      //            vout temp ("teal::vreg_match");
      //            temp << note << "match " << matched << " old value " << old_value_ << " time " <<
      //      	time_at_creation_ << " now " << vtime() << " " << the_vreg << endm;
      time_at_creation_ = vtime ();  //update incase we have multiple changes in the new timeslice
      old_value_ = ((the_vreg)(bit_position_));
      return (matched);
    }

    mutable thread_release* the_thread_release; //set up by at()
    vreg& the_vreg;
    std::string name;
#if defined (vpi_2_0)
    mutable vpiHandle the_call_back;  //set up by at()
    vpiHandle get_handle () const {return the_vreg.handle_;}
#else
    handle get_handle () const {return the_vreg.handle_;}
#endif
    mutable uint64 time_at_creation_;
    const uint32 bit_position_;
    mutable reg::four_state old_value_;
  };

  vout& operator<< (vout& v, const vreg_match& s);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class change : public vreg_match {
  public:
    change (vreg& v) : vreg_match (v, " Change ", 0){};
    change (vreg& v, uint32 bit_position) : 
      vreg_match (v, " Change ", bit_position) {}
  };

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class posedge : public vreg_match {
  public:
    posedge (vreg& v) : vreg_match (v, " posedge ", 0) {};
    posedge (vreg& v, uint32 bit_position) : vreg_match (v, " posedge ", bit_position) {};
    bool match () const { return (vreg_match::match () && ((the_vreg)(bit_position_) == 1));}
  };

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class negedge : public vreg_match {
  public:
    negedge (vreg& v) : vreg_match (v, " negedge ", 0) {};
    negedge (vreg& v, uint32 bit_position) : vreg_match (v, " negedge ", bit_position) {};
    bool match () const {return (vreg_match::match () && ((the_vreg)(bit_position_) == 0));};
  };


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  // This class allows at (posedge(clk) || negedge(reset_n));
  //
  class sensitivity {
  public:
    sensitivity (const vreg_match& s) {
      list.push_back (&s);
      //      for (std::vector<const vreg_match*>::const_iterator it (list.begin());
      //	   it != list.end(); ++it) { vout::get(note) << "sensitivity ctor " << (**it) << teal::endl;}
    };

    ~sensitivity () {
      //      for (std::vector<const vreg_match*>::const_iterator it (list.begin());
      //	   it != list.end(); ++it) { vout::get(note) << "sensitivity dtor " << (**it) << teal::endl;}
    } //no delete because these are pointers to stack created entities.

    void push (const vreg_match& s) {
      list.push_back (&s);
      //      for (std::vector<const vreg_match*>::const_iterator it (list.begin());
      //	   it != list.end(); ++it) { vout::get(note) << "sensitivity push " << (**it) << teal::endl;}
    };
    std::vector<const vreg_match*> list;
  };

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  inline sensitivity operator|| (const vreg_match& lhs, const vreg_match& rhs) 
  {sensitivity returned (lhs); returned.push (rhs); return returned;}

  inline sensitivity operator|| (const vreg_match& lhs, sensitivity s) 
  {s.push (lhs); return s;}

  inline sensitivity operator|| (sensitivity s, const vreg_match& rhs)
  {s.push (rhs); return s;}

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This function suspends the calling thread until one
  //of the verilog signals change.
  //
  void at (const sensitivity&);

  /*previous*/ bool synch_chatty (bool new_value);

  void finish (); //exit the simulation

  void print_threads (const std::string& prefix);
