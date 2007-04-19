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

#if defined (SIMULATOR)
#define register teal::reg
#define truss_reg_write8(addr,data) teal::memory::write (addr, data)
#define truss_reg_write32(addr,data) teal::memory::write (addr, data)
#define truss_reg_read8(addr, data) teal::memory::read (addr, data)
#define truss_reg_read32(addr, data) teal::memory::read (addr, data)
#else
#define register int
#define truss_reg_write8(addr,data) (*(unsigned char*)(addr)) = data
#define truss_reg_write32(addr,data) (*(int*)(addr)) = data
#define truss_reg_read8(addr, data) data = (*(unsigned char*)(addr))
#define truss_reg_read32(addr, data) data = (*(int*)(addr))
#endif


//given some register field definitions such as..
//#define field_<name>_max 16
//#define field_<name>_min 7

#if 0
  log_ << teal_debug << " after put_field " << #name << " data (" << name##_max << "," \
       << name##_min << ") = " << value << " result: " << data << teal::endm; \

#endif

#if defined SIMULATOR
#define truss_field_put(data, name, value) \
{\
  data(name##_max,name##_min) = value;\
}

#define truss_field_get(data, name) data(name##_max,name##_min)
#else
//WARNING- assumes a 32 bit register
#define truss_field_mask(maxf,minf) ((~0L >> (32-maxf)) << minf)
#define truss_field_put(data, name, value) data = (data & ~(field_mask(name##_max, name##_min)) | ((value & field_mask (name##_max, name##_min) << name##_min)
#define truss_field_get(data, name) ((data & field_mask(name##_min, name##_min)>> name##_min)
#endif
