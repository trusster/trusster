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
function void shutdown::shutdown_now (string reason);
   //start a final_report phase (which can be called by the watchdog timer.)
   //at the start opf this phase, tell the regression object to start logging

   teal::vlog log = teal::vlog_get();

   begin
      static bit sentry = 0;
      if (0 && (sentry == 1)) begin
	 log.local_print ("Shutdown called while shutting down! Panic exit.");
	 $finish ();
      end
      sentry = 1;
   end

//   `truss_assert (testbench_ != null);
   if (testbench_ == null) begin
      log.local_print ("truss::shutdown_now() Testbench pointer is NULL! Skipping report()");
   end
   else begin
      testbench_.report (reason);
   end

//   `truss_assert (test_ != null);
   if (test_ == null) begin
	log.local_print ("truss::shutdown_now() Test pointer is NULL! Skipping report()");
   end
   else begin
	test_.report (reason);
   end

//   `truss_assert (watchdog_ != null);
   if (watchdog_ == null) begin
      log.local_print ("truss::shutdown_now() Watchdog pointer is NULL! Skipping report()");
   end
   else begin
      watchdog_.report (reason);
   end

   if (log.how_many (teal::vout_error) == 0) begin
      log_.info ({"Test ", test_.name(), " Passed."});
   end
   else begin
      string msg;
      if (test_ == null) begin
	    msg = $psprintf ("Test UNKNOWN Failed. Contained %0d errors.", log.how_many (teal::vout_error));
      end
      else begin
	   msg = $psprintf ("Test %s Failed. Contained %0d errors.", test_.name(), log.how_many (teal::vout_error));
      end
      log_.info (msg);
   end
   $finish ();
endfunction

