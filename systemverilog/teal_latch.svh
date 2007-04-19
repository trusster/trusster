`ifndef __teal_latch__
`define __teal_latch__


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class latch;
   local bit set_;
   local bit reset_value_;
   local event event_;
   local vout log_;

   function new (string name, bit reset_on_wait);
      log_ = new (name);
      set_ = 0;
      reset_value_ = !reset_on_wait;
   endfunction // new

   task signal ();
      set_ = 1;
      ->event_;
   endtask

   task clear ();
      set_ = 0;
   endtask

   function bit is_signaled (); return set_; endfunction

   task pause ();
//      $display ("pause reset val is %0d set value is %0d", reset_value_, set_);
      
      if (set_) begin
	 set_ = reset_value_;
	 return;
      end
//      wait (event_.triggered);  //will report again if we reset and wait at the same time.
      @(event_);  
      
      set_ = reset_value_;
   endtask // wait_for_set

   task reset_on_wait (bit r);
      reset_value_ = !r;
   endtask

endclass 
`endif
