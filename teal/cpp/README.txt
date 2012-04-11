Mike Mintz 04/30/2004

This file contains descriptions of the files contained in the teal directory.

-------- MAKEFILE -----------------
First define ARCH as Linux (other OS not supported)
Then, define SIMULATOR_HOME as the path to your simulator (sn include 
  directory is assumed).

To build for modsim, use make SIM=mti or make SIM=mti_v_2_0 for the vpi 2.0 interface.
For Icarus, use make SIM=ivl

The build will create a libteal.$(SIM).a

Then go to the test subdirectory to run some tests.

------- FILES THAT FORM THE BASE --------
teal.h - every header is now in one file

reg.cpp : The files for arbitrary length 4-value registers. Its intent is
  to be a mirror of the reg concept in Verilog. There are probably some more 
  operations to be added, but the basics are there. A multiply and 
  divide are not present because they are implemented in different ways 
  depending on the desired precision. The concept of lower index in the reg,
  e.g. reg (33,4), is also not there because it complicates the understanding 
  of reg.cpp. See test/reg_test.cpp for an example.

vout.cpp: The basic IO system. It's intent is to model c++'s cout, but in
  a minimal subset. I could have just hooked into the streams world, but that 
  brings in a fair amount of complexity. Efficiency is also a concern.
  See test/vout_test.cpp for an example.

-------- FILES THAT ARE USED TO HOOK TO VERILOG'S VPI/PLI -----------

vreg.cpp: Here's where we start to talk verification! Given a string,
  this object hooks into Verilog and provides the get and put capability. 
  By cleverly coding reg (above), vreg can hook all operations so that, 
  when an assignment is done, like "vreg = 5;", the appropriate VPI/PLI
  routine is done. Also, any use of vreg has hooked into reg such that the
  value is fetched from the verilog world. Also, by using c++'s standard string, 
  you and make the vreg location be relative. I usually make a transactor 
  (i.e group of vregs and some logic) as taking in a root path. Then by 
  constructing the vregs I need with (path + ".signal_name"), the transactor
  can be put in different places in a dut or reused. See test/vreg_test.cpp and
  many others in test for examples.

synch.cpp One of the differences between Teal and simply
  using verilog callbacks is the illusion of the C world being in control. By 
  you implementing a "user_main ()", the C code is in control until it calls
  the at() function. The user_main() usually starts a number of transactors 
  and then a main_loop(). This main_loop can be a repeat until a certain time,
  repeat until an event, or even a tcl interpreter. 

  The at() function is an attempt to mirror the Verilog @() expression. Where
  in Verilog, it would be "always @ (posedge(foo), negedge (bar), clk)", in Teal
  it's "at (posedge (foo) || negedge (bar) || change (clk));". This is what 
  stops the C code and lets the simulator get back in control. The always part 
  is done by run_thread. Maybe I should rename that to always ? 
  See test/synch_test.cpp for an example.

  The run_thread () function is designed to mirror the always and fork block.
  It's the way transactors/main loop is started. Run_thread() starts a new 
  thread of execution (i.e runs the passed in function.).It runs until an at()
  is reached,

  As soon as one has threads, one can have contention. In Verlog this is the 
  multiple drivers issue, in Vera/Specman it's the lock concept. This happens 
  when two or more threads access the same vreg/piece of hardware/bus. If you
  don't have an arbitration scheme (as in ARM AHB with seperate request lines,
  or ethernet with collecion detection) you must ensure that this does not 
  happen.  The mutex concept does this. Suppose you've written three transactors
  that all need to look like the ARM master. Since there is only one arm master,
  the code must coordinate on putting addresses/data on the bus. This is where
  a mutex is used. See the test/mutext_test.cpp for an example.

memory.cpp: Given that most hardware has or uses memory, 
  these files provide back door access. Of course, you make decide to implement
  a memory range as a bus transaction, and then you would not use this code. However,
  if you want to init the memory (for faster sims) or check that something 
  got into memory, you might prefer the speed of using the backdoor access.

  To use the memory system, you must add an 
  "initial $teal_memory_note (reg_bank)" to your verilog memory model. I 
  decided not to have the C++ hold the memory because (1) Verilog is good at 
  the handshaking protocol and (2) you probably already have a verilog model.
  Note that this assumes you do not have gigabytes of memory. If so, I would 
  implement the actual memory as a c++ standard map.
      
  The memory system is really just a simple lookup to find the right memory_bank
  and then passes the read/write operation to the memory_bank. The memory_bank 
  is where the actual reading/writing occurs. This is to allow multiple banks
  of memory to act as a single bank. See test/memory_test.cpp for an example.


------ FILES THAT ARE GOOD THINGS FOR A VERIFICATION SYSTEM --------

dictionary.cpp:  This procides a simple way to have scripting 
  variables. By creating a text file with "some_switch 1" or 
  "percent_bad_traffic 75", one can write multiple tests or constrain the
  random number generation.  I've used this in the past to control on or off
  of features for particular chip varients and to change probability of 
  bad things happenning. I've also used to to control vcd dumping and length
  of a simulation. See test/dictionay_test.cpp for an example.

trandom.cpp: This is the random number package. By initing the random
  to a master seed, all instances of the random object will follow from that
  seed. Now, each random object instance is given it's own seed to hash with
  the master seed. That allows an instance to be independent from other 
  instances, but still related to the master seed. See test/trandom_test.cpp 
  for an example.

  There is a related issue of what causes an random object instance to change
  it's stream of random numbers. Of course, there's the changing of the master 
  seed, but it will also change when its seed is changed. So don't do that.
  One way is to always pass in a fixed string or integer to the class. But
  that gets tedious. Because the random macros use the ansii __FILE__ and 
  __LINE__, it's important to not change the placement of the macro once it's
  been coded. I use the technique of putting all the RAND_XXX calls at the 
  top of the source file, using functions to get a random number. (Should I
  have an example?)

----------- MISCELLANEOUS ----------
test directory:
Some feeble testing for Teal. Really a good area for examples.See the 
README.txt file there.

