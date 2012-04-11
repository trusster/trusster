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

`ifndef __truss__
`define __truss__


`include "teal.svh"

`ifdef VCS
  `define virtual_interfaces_in_packages
 `endif

`ifdef ATHDL_SIM
 `define   virtual_interfaces_in_packages
`endif

`ifdef MTI
 `define   virtual_interfaces_in_packages
`endif

 
//If the simulator does not support interfaces delcared in packages, put them outside.
interface watchdog_interface (
   input reg hdl_timeout_,
`ifdef ATHDL_SIM
    input reg [`COUNTER_WIDTH-1:0] hdl_timeout_count_
`else
`ifdef MTI
   output reg [`COUNTER_WIDTH-1:0] hdl_timeout_count_
`else
   output reg [COUNTER_WIDTH-1:0] hdl_timeout_count_
`endif
`endif
  );
endinterface
   
package truss;

`define truss_assert(x) if (!(x)) log_.fatal (" assertion falure: x")

`include "truss_verification_component.svh"
`include "truss_test_base.svh"
`include "truss_shutdown.svh"
`include "truss_testbench_base.svh"
`include "truss_vout.svh"
`include "truss_checker.svh"
`include "truss_channel.svh"
`include "truss_test_component.svh"
`include "truss_watchdog.svh"
   
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   function string version (); return  "truss_1.62";   endfunction
endpackage

//interfaces are not allowed in a package, so the folowing cannot be inside of truss
//`include "truss_watchdog.svh"
`endif
