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
//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////// Reg //////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  class reg;
  class vout;

  //teal_acc_vec_val is just some sugar ontop of the vpi one.
  //doesn't add any data or virtual methods, so its interchangeable with base class.

  //////////////////////////////////////////
  //////////////////////////////////////////
  class teal_acc_vecval : public s_acc_vecval {
  public:
    explicit teal_acc_vecval () {aval = ~0; bval = ~0;}; //start at X, doesn't know length so client must clean,
    //cannot add a param for length, since is new'd in an array  
    void operator= (uint32 val) {aval = val; bval = val;}; //val is most often 0
    teal_acc_vecval (const teal_acc_vecval& rhs) {aval = rhs.aval; bval = rhs.bval;};
    teal_acc_vecval (uint32 val) {aval = val; bval = val;}; //val is most often 0

    vout& operator<< (vout& c) const;
    friend vout& operator<< (vout&, const teal_acc_vecval&);
  };


  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  //helper class to implement register slice assignment.
  class reg_slice {
  public:
    void operator= (const reg&);              //to provide reg_a(5:3) = reg_b;
    void operator= (uint64);                  //to provide reg_a(5:3) = 2;
    //printer???
  private:
    friend class reg;
    explicit reg_slice (uint32 u, uint32 l, reg& r);
    explicit reg_slice (const reg&); //no impl
    uint32 upper_;
    uint32 lower_;
    reg& reg_;
    std::ostream& operator<< (std::ostream& c) const;
    friend std::ostream& operator<< (std::ostream&, const reg_slice& rhs);
    vout& operator<< (vout& c) const;
    friend vout& operator<< (vout&, const reg_slice&);
  };

  extern teal::vout& operator<< (vout& c, const reg& rhs);

  //////////////////////////////////////////
  //////////////////////////////////////////
  class reg {
  public:
    //constructors/destructor
    explicit reg ();                          //one bit register
    reg (const reg_slice &);
    reg (uint64 value, uint64 bit_size = 64);
    reg (const reg &);
    reg& operator= (const reg &);

    typedef enum four_state {zero = 0, one, X, Z}; //NOTE: zero to allow simple expressions
    reg (four_state);
    //what about from string???

    virtual ~reg ();

    //accessors
    reg_slice operator() (uint32 u, uint32 l);
    reg operator() (uint32 u, uint32 l) const;

    std::string format_hex_string () const;
    std::string format_binary_string () const;
    std::string format_decimal_string () const; //must be 64 bits or smaller

    uint64 to_int () const;
    char to_int (uint32 b) const;    //extract one bit (WARNING: Drops X/Z, see operator(uint32))

    //logic
    friend four_state triple_equal (const reg& lhs, const reg& rhs);
    friend bool operator== (const reg& lhs, const reg& rhs);
    friend four_state operator< (const reg& lhs, const reg& rhs); //Should be bool?
    friend four_state reduce_xor (const reg&);

    four_state operator() (uint32 b) const;
    
    //math
    reg& operator+= (const reg& rhs) {*this = *this + rhs; return *this;}
    reg& operator-= (const reg& rhs) {*this = *this - rhs; return *this;}
    //    reg& operator>> (unsigned rhs) { return (*this = *this >> rhs);}
    reg& operator<< (unsigned rhs) {*this = *this << rhs; return *this;}
    reg& operator&= (const reg& rhs) {*this = *this & rhs; return *this;}
    reg& operator|= (const reg& rhs) {*this = *this | rhs; return *this;}
    //what about modulus , xor, unary not?

    
    void get_nibble_ (uint32 i, uint32 j, uint8* a, uint8* b) const;

    std::ostream& operator<< (std::ostream& c) const;
    virtual vout& operator<< (vout& c) const;

    virtual void write_through () const {}; //see vreg.h
    virtual void read_check () const {}; //see vreg.h

    //utility
    //return the reg that represents the biggest valid value 
    static reg max_value (uint32 bit_length);
    
    uint32 word_length () const {return word_length_;}//Should be ref, just do cpy/assign ctor
    uint32 bit_length () const {return bit_length_;}  //Should be ref, just do cpy/assign ctor

    teal_acc_vecval vecval (uint32 i) const 
      {return (i < word_length_ ? teal_acc_vecval_[i] : 0);}

  protected:
    void resize (uint32 bit_length);

    //WARNING, order of parameters is used in ctors
    mutable uint32 bit_length_;
    mutable uint32 word_length_;
    mutable teal_acc_vecval* teal_acc_vecval_;

    friend class reg_slice;
    friend class ::regular_memory_bank;
    static uint32 words_ (uint32 bits);
    static uint32 mask_bit_ (uint32 bit);
    static uint32 which_word_ (uint32 bit);


    //symmetric math ops
    friend reg operator+ (const reg & lhs, const reg& rhs);
    friend reg operator- (const reg & lhs, const reg& rhs);
    friend reg operator& (const reg & lhs, const reg& rhs);
    friend reg operator| (const reg & lhs, const reg& rhs);
    friend reg operator~ (const reg & lhs);

    friend reg operator>> (const reg & lhs, const uint32 rhs);    
    friend reg operator<< (const reg & lhs, const uint32 rhs);    

    friend std::ostream& operator<< (std::ostream&, const reg& rhs);
    friend teal::vout& teal::operator<< (vout& c, const reg& rhs);
  };
 
  //In case the compiler does not do a friend as a declaration...
  reg operator+ (const reg & lhs, const reg& rhs);
  reg operator- (const reg & lhs, const reg& rhs);
  reg operator& (const reg & lhs, const reg& rhs);
  reg operator| (const reg & lhs, const reg& rhs);
  reg operator~ (const reg & lhs);

  reg operator>> (const reg & lhs, const uint32 rhs);    
  reg operator<< (const reg & lhs, const uint32 rhs);    

  std::ostream& operator<< (std::ostream&, const reg& rhs);
  vout& operator<< (vout& c, const reg& rhs);

  inline bool operator!= (const reg& lhs, const reg& rhs) { return !(rhs == lhs);}
  vout& operator<< (vout&, reg::four_state);

  //the reduction operators (c++ does not do unary &,!,^
  //could make a dummy object, but that seems too complcated
  reg::four_state reduce_xor (const reg&);

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////// Vreg /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  class signal_match;
  class vout;

  class vreg : public reg {
  public:
    vreg (const std::string& path_and_name);
    virtual ~vreg ();
#if defined (vpi_2_0)
    explicit    vreg (vpiHandle); //generally for memory.cpp, but I dislike redundant code in differnt cpps
#endif

    static bool present (const std::string& path_and_name);

    void name (const std::string& path_and_name);
    std::string name () const {return path_and_name_;}

    bool enabled () const {return enabled_;}

    vreg& operator= (const reg&);

    /*override*/ void read_check () const; 
    /*override */ void write_through () const;

    //called by the pli callback to change the global current state value.
    static void invalidate_all_vregs ();
    
    vreg& operator= (const vreg&);

    //for verilog regs that want to take place in the current simulation time...
    void immediate_write (bool val) {
#if defined (vpi_2_0)
      //wires are immediate by definition;
      write_policy_ = val ? vpiNoDelay : vpiInertialDelay;
#else
      write_policy_ = val ? accNoDelay : accInertialDelay;
#endif
    }

    //for verilog wires, used to release the wire(s)
    void release () {
#if defined (vpi_2_0)
      write_policy_ = vpiReleaseFlag;
#else
      assert ((write_policy_ == accForceFlag) || (write_policy_ == accReleaseFlag));
      write_policy_ = accReleaseFlag;
#endif
      invalidate_all_vregs (); //really just me, but probably not a big performance hit
      write_through (); //flush out the release
      read_check ();    //get the floating value

      //restore
#if defined (vpi_2_0)
      write_policy_ = vpiForceFlag;
#else
      assert ((write_policy_ == accForceFlag) || (write_policy_ == accReleaseFlag));
      write_policy_ = accForceFlag;
#endif
    }

    vreg (const vreg&); 

  protected:
    std::string path_and_name_;
    virtual vout& operator<< (vout& c) const;
    virtual void connect_ ();
    virtual void disconnect_ ();

  private:

#if defined (vpi_2_0)
    vpiHandle handle_;
#else
    handle handle_;
#endif
    mutable uint32 state_;
    static uint32 master_state_;
    bool enabled_;
    int32 write_policy_;

    friend class vreg_match;
    friend vout& operator<< (vout&, const vreg&);
  };
  vout& operator<< (vout&, const vreg&);
