
module wishbone_driver_verilog (clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty);
   input		clk, rst;
   output	[31:0]	adr;
   input [31:0] 	din;
   output [31:0] 	dout;
   output		cyc, stb;
   output [3:0] 	sel;
   output		we;
   input		ack, err, rty;

   
   //interface to bfm
`ifdef kjljjkjljl
   
   reg 			do_work;
   reg			work_done;
   reg [1:0] 		op_code; 

   reg [31:0] 		address;
   wire [31:0] 		data_in;
   reg [31:0] 		data_out;
   reg [3:0] 		select;

   initial begin
      do_work = 0;
      work_done = 0;
      $display ("[verilog] %t %m do_work initialized", $time);
   end
`else
   
   wire 			do_work;
   wire			work_done;
   wire [1:0] 		op_code; 

   wire [31:0] 		address;
   wire [31:0] 		data_in;
   wire [31:0] 		data_out;
   wire [3:0] 		select;

   pulldown pd_1 (do_work);
   
`endif   
   reg 			work_doner;
   reg 			do_workr;
   //assign 		do_workr = do_work;
   reg [31:0]			data_outr;
   
   
   
   always @(posedge (do_work))
     begin
	work_doner = 0;
	//do_workr = 0;	       
//		 $display ("%t ns Verilog %m Received op_code %d", $time, op_code);


	case (op_code)
	  0: begin
	     wb_mast.wb_wr1 (address, select, data_in);
//	     $display ("[%t] [%m] Completed Write1: 0x%x , 0x%x (select 0b%b)", $time, address, data_in, select);
	  end
	  1: begin
	     wb_mast.wb_rd1 (address, select, data_outr);
//	     $display ("[%t] [%m] Completed Read1: 0x%x , 0x%x (select 0b%b)", $time, address, data_outr, select);
	  end
	  default: begin
	     $display ("[%t] [%m] Unknown Opcode: 0x%x", $time, op_code);
	  end
	endcase
	
	work_doner <= 1;  //non blocking so as not to miss the posedge.
     end


   //The real workhorse...
   wb_mast wb_mast (clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty); 
endmodule
