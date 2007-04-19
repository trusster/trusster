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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
`ifdef virtual_interfaces_in_packages
 function watchdog::new (string name, virtual watchdog_interface pins, shutdown s);
`else
    function watchdog::new (string name, shutdown s);
`endif
  super.new (name);
   shutdown_ = s;
`ifdef virtual_interfaces_in_packages
   watchdog_interface_ = pins;
`endif
   timeout_occurred_  = 0;
   hdl_timeout_occurred_ = 0;
endfunction // watchdog

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function void watchdog::report (string prefix);
   if (timeout_occurred_) begin
      log_.error ((hdl_timeout_occurred_ ? "HDL timeout occurred." : "Test-based shutdown occurred"));
   end
   else begin
      log_.debug (" Watchdog not triggerred. Normal test completion.");
   end
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task watchdog::start_ ();
   //set the the timeout value
   int value = teal::dictionary_find_integer ({name_, "_timeout"}, 10000000);
   teal::vout x = new (name_);
   string msg;
   msg = $psprintf ("  Using Timeout of %0d", value);
   x.info (msg);

`ifdef virtual_interfaces_in_packages
   watchdog_interface_.hdl_timeout_count_ = value;
  @ (posedge (watchdog_interface_.hdl_timeout_));
`else
   real_interfaces.watchdog_interface_0.hdl_timeout_count_ <= value;
  @ (posedge (real_interfaces.watchdog_interface_0.hdl_timeout_));
`endif   
  hdl_timeout_occurred_ = 1;
  shutdown_now ("HDL Timeout!!!");
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task watchdog::shutdown_now (string prefix);
   timeout_occurred_ = 1;   
  shutdown_.shutdown_now (prefix);
endtask


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
task watchdog::wait_for_completion ();
`ifdef virtual_interfaces_in_packages   
   @ (posedge (watchdog_interface_.hdl_timeout_));
`else
   @ (posedge (real_interfaces.watchdog_interface_0.hdl_timeout_));
`endif
endtask
