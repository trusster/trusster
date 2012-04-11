Mike Mintz 05/10/2004

This file contains descriptions of the files contained in the teal test 
directory.

-------- MAKEFILE -----------------
First define ARCH as Linux (other OS not supported)
Then, define VERILOG_INST_DIR as the path to your simulator (sn include 
  directory is assumed).


To run a test (that uses a simulator, there are a few that do not),
use ./run -c -<SIM> -t <test_name> 
where <SIM> is either -mti, -mti_2_0, or ivl and
<test_name> is either (for now) memory_test, mutex_test, 
  semaphore_test, or run_loop_test

To run a test list use, ./run -c -<SIM> -l <some_file> See test_list for 
an example.

NOTE: Apple Valley Verification has a c-shell version of run with more parameters. 
      Send e-mail to support@applevalleyverification.com for a copy.

----------------- TEST FILES --------------
In general, each test has a .cpp and a .v component. 

-------- trandom_test.cpp, vout_test.cpp   --------------
-------- reg_test.cpp, dictionary_test.cpp  --------------
These test are real simple exercisers to make sure the basic functionality 
works. They do not use a simulator, and are "straight" c++. This means
they are also the bas set for diagnostics. (Have to add a diag memory test.)

They are also good places to see some of Teal's basic, non simulator 
dependent capabilities.

To Do: make these tests use a run_diag script, and a libteal.diag.a/so

-------------- vreg_test.cpp and .v --------------
This simple little test shows how to make a verilog register and "peek" 
and "poke" it.  This is the base level that uses a simulator.

-------------- memory_test.cpp and .v --------------
This test shows how to hook up a bank of memory. It also shows the front 
and back door use of the memory interface.

To Do: rename to tmemory.h

-------------- mutex_test.cpp and .v --------------
This test shows how many threads can access a shared common hardware resource.
In testing, one may have seveal daemon threads, all trying to beat on a bus.
This is most often the case when the threads are trying to act like a cpu
using its main bus. In this case, the software is responsible for avoiding
having two or more accesses to the bus at the same time. In the Teal simulator
world, this is accomplised by a mutex.

The test shows 100 (or more) threads trying to set their op_code onto the 
hardware transactor. They use a mutex to guarentee that only one gets in 
at a time. The test ends when all threads have been given access to the
bus for a fixed number of times.


-------------- synch_test.cpp and .v --------------
This cute little test shows how a user_main() can use threads to create
independent monitors and injectors.

There are two injectors, add_one and toggle_bits, that, well, add one to 
a vreg and toggle the bit state of a vreg.  Both of them have a "wait"
condiion, that is fipped by the verilog.

The monitor thread waits until either one changes, and checks to make 
sure only the expected one chaged. The synch_test.v has cleaverly use a 
different delay so that the monitor can figure this out.

-------------- synch_test.cpp and .v --------------
Based on sempaphore_test, this test has the same three therads. But, instead
of using a verilog signal in the toggle_bits() thread, it uses a semaphore
from the add_one() thread. This shows how threads can communicate events
to other threads. 



-------------- run_loop_test.cpp and .v --------------
This test shows how a run_loop can be used to create a transactor and 
a checker. The transactor responds to teh verilog asking for data. The verilog
buffers the data, When the buffer is full, the verilog signals to another 
thread to check the data.
 

----------------------------------------------------------

