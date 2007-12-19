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
`ifndef __watchdog__
`define __watchdog__

`include "truss.svh"
`include "teal.svh"

typedef class shutdown;
typedef class verification_component;
      

//used in the .v as well


//put the wires in the interface to not change watchdog.v
//MUST copy to /testbench/$config/interfaces_dut.svh
`ifdef virtual_interfaces_in_packages
   `ifdef virtual_interface_declarations_in_interface
interface watchdog_interface (
   wire hdl_timeout_,
   wire [COUNTER_WIDTH-1:0] hdl_timeout_count_;
  );
endinterface // watchdog_interface
   `endif
`endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class watchdog extends verification_component;
`ifdef virtual_interfaces_in_packages   
   virtual watchdog_interface  watchdog_interface_;
   
    extern function new (string name, virtual watchdog_interface pins, shutdown s);
`else
    extern function new (string name, shutdown s);
`endif       

    virtual task time_zero_setup (); endtask
    virtual task out_of_reset (reset r); endtask
    virtual function void randomize2 (); endfunction
    virtual task write_to_hardware (); endtask
    virtual task start (); fork start_ (); join_none endtask
    extern virtual task wait_for_completion ();

    extern virtual function void report (string prefix);

    //for remote or other watchdog mechanisms to create a timeout
    extern task shutdown_now (string prefix);

    extern local task start_ ();
    local shutdown shutdown_;
    local bit timeout_occurred_;
    local bit hdl_timeout_occurred_;
//    local virtual watchdog_interface watchdog_interface_;
    endclass // truss_watchdog

`include "truss_watchdog.sv"
`endif
