`timescale 1ns / 1ns

`ifdef ATHDL_SIM
`define COUNTER_WIDTH 64
`else
`ifdef MTI
`define COUNTER_WIDTH 64
`else
parameter COUNTER_WIDTH        = 64;        //Number of bits in time-out counter
`endif
`endif

module watchdog_implementation (timeout);
   output timeout;

   parameter TIMEOUT              = 10_000_000; //Default time-out is 10 ms
   parameter POST_TIMEOUT         = 100;       //Default post time-out count is 100 ns
   parameter POST_COUNTER_WIDTH   = 8;         //Number of bits in final time-out counter
   parameter CLK_PERIOD           = 1;         //Default clock period length
   
`ifdef ATHDL_SIM
   reg [`COUNTER_WIDTH-1:0]       counter;
`else
`ifdef MTI
   reg [`COUNTER_WIDTH-1:0]       counter;
`else
   reg [COUNTER_WIDTH-1:0]       counter;
`endif
`endif
   reg [POST_COUNTER_WIDTH-1:0] post_counter;
   reg 			         clk;
   reg                           timeout_reg;

   assign timeout = timeout_reg;

   initial begin
//      counter       = TIMEOUT;
      post_counter  = POST_TIMEOUT;
      clk           = 0;
      timeout_reg   = 0;
   end

   always #CLK_PERIOD clk = ~clk;

   // Time-out "event" - Signals truss that test has taken to long (default = 5 ms)
   always @(posedge clk) begin
      if (counter == 0) timeout_reg = 1;
//      else counter = counter -2; //use -2 to make TIMEOUT be in "ns".
   end

   // Post counter "event" - Shuts down simulation as a last resort if truss/teal don't notice the time-out event
   always @(posedge clk) begin
      if (timeout_reg == 1) begin
	 if (post_counter == 0) begin
	    $display("Error! [%t][%m] Time-out occured and final counter exired. System must be hung! Shutting down simulation\n", $time);
	    $finish;
	 end
	 else
	   post_counter = post_counter -2; //use -2 to make POST_TIMEOUT be in "ns".
      end
   end

endmodule   
   
