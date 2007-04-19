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
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////// memory  /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
package memory;

    class memory_bank {
    public:
      memory_bank (const string& path) : path_ (path), first_address (0), last_address (0) {};
      virtual ~memory_bank () {};

      virtual void from_memory (uint64 address, reg*) = 0;
      virtual void to_memory (uint64 address, const reg& value) = 0;

      bool contains (const string& path) const {return (path_.find (path) != string::npos);};
      bool contains (uint64 address) const {return ((address >= first_address) && (address <= last_address));};
      
      mutable string path_;
      uint64 first_address;
      uint64 last_address;
    };

    class memory_bank;

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    //The "normal" way to access memory...
    
    //setup a mapping between some global address space and some memory
    void add_map (const string& path, uint64 first_address, uint64 last_address);

    //now access it...
    void read (uint64 global_address, reg*);  //need the pointer top get the desired size, i.e 8 bit, 32 bit or burst
    void write (uint64 global_address, const reg& value);

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    class memory_bank;
    void add_memory_bank (/*owner*/ memory_bank*);

    /*cached*/ memory_bank lookup (uint64 address_in_range);
    /*cached*/ memory_bank lookup (const string& parial_path);
endpackage
