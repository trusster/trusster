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
`ifndef __uart_channel__
 `define __uart_channel__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class uart_channel;
   local uart_block storage_[$];
   local teal::latch put_condition_;
   local teal::latch get_condition_;
   local bit[63:0] depth_;
   local semaphore mutex_;
   local uart_channel listners_[$];  //template on uart_block once classes can be templated
   local teal::vout log_;

   function new  (string n, bit[63:0] d = 64'hFFFF_FFFF_FFFF_FFFF);
      put_condition_ = new ({n,  "_put_channel_condition"}, 1);
      get_condition_ = new ({n, "_get_channel_condition"}, 1);
      depth_ = d;
      mutex_ = new (1);
      log_ = new (n);
   endfunction // new


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task put (uart_block d);
      bit [63:0] count_; count(count_);
      if (count_ >= depth_) begin
	 get_condition_.pause ();
      end
      mutex_.get ();
      begin
	 string msg;
	 msg = $psprintf ("put() : data is %s", d.sreport ()); 
	 log_.debug (msg);
      end
      storage_.push_back (d);
      mutex_.put ();
      put_condition_.signal ();

      //now for the attached channels
      for (integer i = 0; i < listners_.size(); ++i) begin
	 listners_[i].put (d);
      end
   endtask // put
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task get (output uart_block returned);
      begin
	 string msg;
	 bit [63:0] count_;
	 int foo;
	 count(count_);
	 msg = $psprintf ("get() : count  is %0d", count_); 
	 log_.debug (msg);
      end

      begin
	 bit [63:0] count_; count(count_);      
      while  (!count_) begin
	 put_condition_.pause ();
	 begin
	    string msg;
	    bit [63:0] count_; 
	    int foo;
	    count(count_);	    
	    msg = $psprintf ("get() : after wait. count is %0d", count_); 
	    log_.debug (msg);
	 end
	 count(count_);
      end
	 end
      
      mutex_.get ();
      returned = storage_.pop_front();
      mutex_.put ();
      begin
	 string msg;
	 msg = $psprintf ("get() : data is %s", returned.sreport ());
	 log_.debug (msg);
      end
      
      get_condition_.signal ();
   endtask

   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   task count (output  bit[63:0] returned);
      mutex_.get ();
      returned = storage_.size();
      mutex_.put ();
   endtask

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   function string name (); return log_.name ();  endfunction

   function void add_listner (uart_channel new_one);
      listners_.push_back (new_one);
   endfunction
endclass


`endif
