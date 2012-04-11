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
  typedef size_t  thread_id;
  thread_id start_thread (user_thread, void* user_data, const std::string & name);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  std::string thread_name (thread_id);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //This function cancels the thread and wait for it to terminate.
  //
  void* stop_thread (thread_id);
  void* join_thread (thread_id);
  void note_thread_completed (); //for threads that end themselves
  void stop_all_threads ();

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //return the curent simultaion time. (TODO - allow a precision?)
  uint64 vtime ();

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //forward for internal use
//  class thread_release; //internal record-keeping class
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class vreg_match {
  public:
    vreg_match (vreg& v, const std::string n, uint32 bit_position) :
      the_vreg_ (v), name_ (n), bit_position_ (bit_position)  {}
    virtual ~vreg_match () {}

    virtual const sc_core::sc_event& get_event () const;
    
  protected:
    vreg& the_vreg_;
    std::string name_;
    const uint32 bit_position_;
  };

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
    virtual const sc_core::sc_event& get_event () const;
  };

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  class negedge : public vreg_match {
  public:
    negedge (vreg& v) : vreg_match (v, " negedge ", 0) {};
    negedge (vreg& v, uint32 bit_position) : vreg_match (v, " negedge ", bit_position) {};
    virtual const sc_core::sc_event& get_event () const;
  };


  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  // This class allows at (posedge(clk) || negedge(reset_n));
  //
  class sensitivity {
  public:
    sensitivity (const vreg_match& s) {list.push_back (&s); };
    ~sensitivity () { } 

    void push (const vreg_match& s) { list.push_back (&s);}
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

  void at (const sensitivity&);

  //To help in debugging...
  /*previous*/ bool synch_chatty (bool new_value);

  void finish (); //exit the simulation

  void print_threads (const std::string& prefix);

  std::string current_thread_name ();
