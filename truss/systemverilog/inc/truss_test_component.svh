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
`ifndef __truss_test_component__
`define __truss_test_component__

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class test_component  extends verification_component;
    protected bit completed_;

   function new (string n);
      super.new (n);
      completed_ = 1;
   endfunction // new
   
    virtual task start (); fork  start_ (); join_none endtask

   task wait_for_completion (); wait_for_completion_(); completed_ = 1; endtask

   function void report (string prefix);
       if (completed_) begin log_.debug ({prefix, " Completed as expected."}); end
       else begin log_.error ({prefix, " Did not complete!"});  end
    endfunction

//Protected interface
    virtual protected task start_ ();
       log_.debug ("start_() for test_component  begin");
       completed_ = 0;
       start_components_ ();
       run_component_traffic_ ();
       log_.debug ("start_() for test_component  end");
    endtask // start_
   

   virtual task run_component_traffic_ (); randomize2 (); generate2 (); endtask
   
  `PURE virtual protected task start_components_ ();
   //generate some data 
  `PURE  protected virtual task generate2 ();
  `PURE virtual protected task wait_for_completion_ ();
 endclass // test_component


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class irritator  extends test_component;
    local bit generate_;

   function new (string n);
      super.new (n);
      generate_ = 0;
   endfunction // new

   task stop_generation (); generate_ = 0; endtask

   protected virtual task  start_ ();
       log_.debug ("start_() for irritator begin");
       generate_ = 1;
       super.start_ ();
       log_.debug ("start_() for irritator end");
    endtask 
      

   virtual protected task run_component_traffic_ ();
       log_.debug ("run_trafic_() for irritator begin");
       while (continue_generation ()) begin
	  super.run_component_traffic_ ();
	  inter_generate_gap ();
       end
       log_.debug ("run_trafic_() for irritator begin");
    endtask 
      


   virtual protected function bit continue_generation (); return generate_; endfunction

    //subclass to do things like manage an amount in_flight, random delay
    //CAUTION: It's generally a good idea to have this method consume time. Otherwise, an infinate loop can occur.
    //At least one of the following three methods MUST consume time: 
    //                      continue_generation(), run_traffic() [i.e randomize2(); generate();], or inter_generate_gap()
    `PURE protected virtual task inter_generate_gap ();
    endclass 
`endif
