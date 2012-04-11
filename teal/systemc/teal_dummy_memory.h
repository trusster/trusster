#ifndef __teal_dummy_memory__
#define __teal_dummy_memory__

//have to stub out a memory object? Use this one!


#if !defined(SJM)
#define SJM 0
#endif

namespace teal {

  namespace memory {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::vector<teal::uint32> words;  //maybe uint64?

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef enum {read_access, write_access} mem_access;  

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //should a block really have an access or should that be in a subclass??
    class block {
    public:
      explicit block (teal::uint64 a, mem_access acc = write_access) : address (a), access (acc) {}
      explicit block (teal::uint64 a, teal::uint32 d, mem_access acc = write_access) 
	: address (a), access (acc) {data.push_back (d); }
      explicit block (teal::uint64 a, teal::uint64 d, mem_access acc = write_access) 
	: address (a), access (acc) {data.push_back (d); data.push_back (d >> 32);}
      explicit block (teal::uint64 a, words w, mem_access acc = write_access) : data(w), address (a), access (acc) {}
    
      words data;
      teal::uint64 address;
      mem_access access;

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      teal::vout& operator<< (teal::vout& o) const {
	o << teal::hex << "Block at: " << address << " " << ((access == write_access) ? " (write)" : "(read)") 
	  << "size is " << (int)data.size() << (data.size() ? " Data is: " : " No data ");
	for (words::const_iterator it(data.begin()); it != data.end(); ++it) {
	  o << " " << *it << ",";
	}
	return o;
      }

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      bool is_next (const memory::block& b) {	return ((address + (4 * data.size())) == b.address);      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      block& operator+= (const block& rhs);

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      block& operator+= (teal::uint32 rhs) {	data.push_back (rhs);	return *this;      }


      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      size_t size () {return data.size();}
    };


    //teal memory should be reworked into a block, and read should have access size (byte, word, dword, humungadunga???)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //WARNING You must new() this object, as its ctor will pass a pointer to teal::memory, which will cache it!!!
    class dummy_bank : public teal::memory::memory_bank {
    public:
      dummy_bank (const std::string& path, uint32 start, uint32 end) : memory_bank (path) {
	teal::memory::add_memory_bank (this);
	teal::memory::add_map (path, start, end);
      };

      void from_memory (uint64 address, reg* x) {      *x = mem_[address];    
        teal::vout log ("memory_acc");
        if (address != mem_[address]) log << teal_debug << teal::level(3) << teal::hex << "Read: Address:" << first_address << "+" << address << ", Value: " << *x << teal::endm;
      }

      void to_memory (uint64 address, const reg& value) {     
	mem_[address] = value.to_int ();    
        teal::vout log ("memory_acc");
        if (address != value) //skip silly stuff
          log << teal_debug << teal::level(3) << teal::hex << "Write: Address:" << first_address << "+" << address << ",Value: " << value << teal::endm;

      }

      void to_memory_blocked (uint64 address, const reg& value) {     
        if (SJM) assert(0);
	mem_[address] = value.to_int ();    
        teal::vout log ("memory_acc");
        if (address != value) //skip silly stuff
          log << teal_debug << teal::level(3) << teal::hex << "Blocking write: Address:" << first_address << "+" << address << ",Value: " << value << teal::endm;

      }

      std::map<uint32,uint32> mem_;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    class dummy_bank_unregistered : public teal::memory::memory_bank {
    public:
      dummy_bank_unregistered (const std::string& path) : memory_bank (path) {
      };

      void from_memory (uint64 address, reg* x) {      *x = mem_[address];    
        teal::vout log ("memory_acc");
        if (address != mem_[address]) log << teal_debug << teal::level(3) << teal::hex << "Read: Address:" << first_address << "+" << address << ", Value: " << *x << teal::endm;
      }

      void to_memory (uint64 address, const reg& value) {     
	mem_[address] = value.to_int ();    
        teal::vout log ("memory_acc");
        if (address != value) //skip silly stuff
          log << teal_debug << teal::level(3) << teal::hex << "Write: Address:" << first_address << "+" << address << ",Value: " << value << teal::endm;

      }

      void to_memory_blocked (uint64 address, const reg& value) {     
        if (SJM) assert(0);
	mem_[address] = value.to_int ();    
        teal::vout log ("memory_acc");
        if (address != value) //skip silly stuff
          log << teal_debug << teal::level(3) << teal::hex << "Blocking write: Address:" << first_address << "+" << address << ",Value: " << value << teal::endm;

      }
      std::map<uint32,uint32> mem_;
    };
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool operator== (const teal::memory::block& lhs, const teal::memory::block& rhs)
{
  //not sure whether to compare access or not?, since writes would be checked by reads
  return ((rhs.data == lhs.data) and 
	  (rhs.address == lhs.address));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline teal::vout& operator<< (teal::vout& o, const teal::memory::block& lhs) {  return lhs.operator<<(o);}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline teal::memory::block& teal::memory::block::operator+= (const teal::memory::block& rhs) {
  if ((address + (4*data.size())) != rhs.address) {
    teal::vout x ("memory::block::operator+");
    x << teal_error << " Addresses do not align! " << address  
      << " + " << (int)(4*data.size()) << " != " << rhs.address << teal::endm;
  }
  copy (rhs.data.begin(), rhs.data.end(), std::back_inserter(data));
  return *this;
}

 
#endif
