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
`ifndef __verification_component__
`define __verification_component__


typedef enum {cold, warm} reset;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
virtual class verification_component;
  protected string name_;
  protected teal::vout log_;

   function new (string n); name_ = n; log_ = new (n); endfunction

   //engage pull ups,downs, and things like sense pins
  `PURE virtual task time_zero_setup ();

  //take your part of the DUT out of reset
  //may NOT be called in a loop/rerun/multiple test scenerio
  `PURE virtual task out_of_reset (reset r);

  `PURE virtual function void randomize2 ();

  //after this call, the component is ready for traffic 
  //(why not start of BFM calls this? , because generally usefull method, subcomponents)
  `PURE virtual task write_to_hardware ();

  //on start(), do what makes sense for the object type.
  //for big picture verifictaion components, engage your configuration, start monitors and BFMs
  //do NOT asume that the resisters are at reset, unless you know that for sure
  `PURE virtual task start ();

  //stop_all_threads, get back to the state just before the start call
//not clear that the scope disable works in all sv varients yet
//  `PURE virtual task stop ();

  //or not supposed to be in the generic thing? But needed in a checker
  //called after stop (automatically, or by owner?)
  `PURE virtual task wait_for_completion (); 

  //can mean different things, but generally report your state.
  `PURE virtual function void report (string prefix);

  function string name(); name = name_; endfunction

endclass
`endif
