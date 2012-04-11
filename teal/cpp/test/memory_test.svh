`ifndef __memory_test__
`define __memory_test__

`ifdef MTI
interface memory_1 (
   wire       [122:0]  bank0  [0 : 312]
);
endinterface

interface memory_2 (
   wire       [17:0]  a_top_bank  [0 : 1717]
);
endinterface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_1 extends teal::memory_bank;
  virtual memory_1 memory_bank_1_;
   function new (virtual memory_1 mb);      super.new ("top.a_sub_module.bank0");   memory_bank_1_ = mb; endfunction 
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
//      value = top.a_sub_module.bank0[address];
value = memory_bank_1_.a_sub_module.bank0[address];
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
//      top.a_sub_module.bank0[address] <= value;
      memory_bank_1_.a_sub_module.bank0[address] <= value;
   endtask
endclass

`ifdef kjljkjlk
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_2 extends teal::memory_bank;
  virtual memory_2 memory_bank_2_;
   function new (virtual memory_2 mb);      super.new ("top.a_top_bank");   memory_bank_2_ = mb; endfunction
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
//      value = top.a_top_bank[address];
      value = memory_bank_2_.a_top_bank[address];
//      $display ("top bank Read 0x%0x (%0d) to 0x%0x", address, address, value);
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
//      top.a_top_bank[address] = value;
      memory_bank_2_.a_top_bank[address] = value;
//      $display ("top bank write 0x%0x (%0d) to 0x%0x", address, address, value);
   endtask
endclass

module inteface_creators;
function teal::memory_bank new_mb1 (); 
  memory_bank_1 yy =new (interfaces_memory.mb1); 
  return (yy); 
endfunction

function teal::memory_bank new_mb2 (); 
  memory_bank_2 yy =new (interfaces_memory.mb2); 
  return (yy); 
endfunction

endmodule
`endif

module interfaces_memory;
   memory_1 mb1 (top.a_sub_module.bank0);
//   memory_2 mb2 (top.a_top_bank);
endmodule

`else

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_1 extends teal::memory_bank;
   function new ();      super.new ("top.a_sub_module.bank0");   endfunction 
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
      value = top.a_sub_module.bank0[address];
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
      top.a_sub_module.bank0[address] <= value;
   endtask
endclass

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class memory_bank_2 extends teal::memory_bank;
   function new ();      super.new ("top.a_top_bank");   endfunction
   
   virtual task from_memory (bit [63:0] address, output bit [teal::MAX_DATA - 1:0] value, input int size);
      value = top.a_top_bank[address];
//      $display ("top bank Read 0x%0x (%0d) to 0x%0x", address, address, value);
   endtask

   virtual task to_memory (bit [63:0] address, input bit [teal::MAX_DATA - 1:0]  value, input int size);
      top.a_top_bank[address] = value;
//      $display ("top bank write 0x%0x (%0d) to 0x%0x", address, address, value);
   endtask
endclass

module inteface_creators;
function teal::memory_bank new_mb1 (); 
  memory_bank_1 yy =new (); 
  return (yy); 
endfunction

function teal::memory_bank new_mb2 (); 
  memory_bank_2 yy =new (); 
  return (yy); 
endfunction
`endif




`endif
