
module wishbone_driver (clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty);
   input		clk, rst;
   output	[31:0]	adr;
   input [31:0] 	din;
   output [31:0] 	dout;
   output		cyc, stb;
   output [3:0] 	sel;
   output		we;
   input		ack, err, rty;

   
   //interface to bfm
   reg 			do_work;
   reg 			work_done;
   reg [1:0] 		op_code; 

   reg [31:0] 		address;
   reg [31:0] 		data;
   reg [3:0] 		select;
   
   
   initial do_work = 0;
   
   always @(posedge (do_work))
     begin
	#1;
	work_done = 0;	
	do_work = 0;	       
	 $display ("%t ns Verilog %m Received op_code %d", $time, op_code);


	case (op_code)
	  0: begin
	     $display ("[%t] [%m] before Completed write: 0x%x , 0x%x (select 0b%b)", $time, address, data, select);
	     wb_mast.wb_wr1 (address, select, data);
	     $display ("[%t] [%m] Completed Write1: 0x%x , 0x%x (select 0b%b)", $time, address, data, select);
	  end
	  1: begin
	data = 32'h0;
	     $display ("[%t] [%m] before Completed Read1: 0x%x , 0x%x (select 0b%b)", $time, address, data, select);
	     wb_mast.wb_rd1 (address, select, data);
	     $display ("[%t] [%m] Completed Read1: 0x%x , 0x%x (select 0b%b)", $time, address, data, select);
	  end
	  default: begin
	     $display ("[%t] [%m] Unknown Opcode: 0x%x", $time, op_code);
	  end
	endcase
	
	work_done <= 1;  //non blocking so as not to miss the posedge.
     end

   always @(do_work)
     begin
	$display ("%t ns Verilog %m do work chenged. is now  %d", $time, do_work);
     end

   //The real workhorse...
   wb_mast wb_mast (clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty); 
	
endmodule
