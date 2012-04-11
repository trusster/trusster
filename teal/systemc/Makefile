#!/bin/sh
#Copyright (C) 2004 Mike Mintz

#This file is part of the Teal Library.  This library is free
#software; you can redistribute it and/or modify it under the
#terms of the GNU General Public License as published by the
#Free Software Foundation; either version 2, or (at your option)
#any later version.

#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.


#//foo.lib in sub dir OS/simulator or OS/company or set LD_LIBRRAY_PATH ? ?
#//seperate makefile.os.simulator and then "make -f Makefile.os.simulator install"?
#//how about dropping the lib in lib/ and maybe using a {LIB_PATH} where the default is ./lib ?



ROOT_INCL = -I$(SIMULATOR_HOME)/include 

SRC = .
OBJ = .
LIB = .
INCL = -I.

include $(TEAL_HOME)/Makefile.$(ARCH)

#SP = -I$(INCL) $(ROOT_INCL) -I/usr/local/riviera-2005.04/gcc/lib/gcc/i686-pc-linux-gnu/3.4.3/include
SP = $(INCL) $(ROOT_INCL)



ifeq ($(SIM),mti_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT)  -Dvpi_2_0 -DSIM=mti
else
ifeq ($(SIM),mti)
  CFLAGS =  -Wall $(OPT) $(D_OPT)  -DSIM=mti -I$(SIMULATOR_HOME)/include/systemc -I$(SIMULATOR_HOME)/include/scv 
else
ifeq ($(SIM),ivl)
  CFLAGS = -Wall  $(OPT) $(D_OPT) -Dvpi_2_0 -DSIM=ivl -Divl
else
ifeq ($(SIM),ncsim)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=ncsim -Dncsim -Dvpi_2_0 
else
ifeq ($(SIM),ncsim_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=ncsim -Dncsim -Dvpi_2_0
else
ifeq ($(SIM),vcs_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=vcs -Dvcs -Dvpi_2_0
else
ifeq ($(SIM),vcs)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=vcs -Dvcs 
else
ifeq ($(SIM),cver)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=cver -Dcver
else
ifeq ($(SIM),cver_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=cver -Dcver -Dvpi_2_0
else
ifeq ($(SIM),aldec)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=aldec -Daldec 
else
ifeq ($(SIM),aldec_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=aldec -Daldec -Dvpi_2_0
else
  CFLAGS = "Unknown Simulator type!"
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

STATIC_LIB  = $(LIB)/libteal.$(SIM).a
DYNAMIC_LIB = $(LIB)/libteal.$(SIM).$(ARCH_SHARED_SUFFIX)


#Note that hdl_connect.cpp must be included at your verification_top(), to ensure that it gets included in the final .so
#           $(SRC)/avv_spi.cpp \

TEAL_SRC = \
           $(SRC)/teal_vout.cpp \
           $(SRC)/teal_reg.cpp \
           $(SRC)/teal_vrandom.cpp \
           $(SRC)/teal_vreg.cpp \
           $(SRC)/teal_vreg_internal.cpp \
           $(SRC)/teal_dictionary.cpp \
           $(SRC)/teal_memory.cpp \
           $(SRC)/teal_synch.cpp 

TEAL_OBJ = ${addsuffix .o, ${basename ${TEAL_SRC}}}
#$(patsubst %.cpp, $(OBJ)/%.o, $(TEAL_SRC)) 
### MAIN TARGETS

build: test_environment build_$(SYS_PLATFORM)

test_environment:
	@if [ -z "$(SIMULATOR_HOME)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	The environment variable 'SIMULATOR_HOME' is set to the proper" ;\
		echo "	installation directory that has an include and lib subdirectories  e.g." ;\
		echo "	SIMULATOR_HOME = /tools/cadence3.3/tools/ or" ;\
		echo "	SIMULATOR_HOME = /usr/local/modeltech/mti/ or" ;\
		echo "	SIMULATOR_HOME = /usr/local/ivl/ or " ;\
		echo "	SIMULATOR_HOME = /tools/synopsys/vcs/vcs7.2 or ..." ;\
		echo "	" ;\
		exit 1;\
	fi
	@if [ -z "$(SIM)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	The environment variable 'SIM' is set to the proper" ;\
		echo "	simulator. e.g." ;\
		echo "	setenv SIM ncsim or" ;\
		echo "	setenv SIM mti or" ;\
		echo "	setenv SIM ivl or " ;\
		echo "	setenv SIM vcs7.2 or ..." ;\
		echo "	" ;\
		exit 1;\
	fi


build_unix: $(STATIC_LIB) $(DYNAMIC_LIB) 
	touch build

build_wint: $(DYNAMIC_LIB)
	touch build

clean:
	rm -f $(TEAL_OBJ)
	rm -f build
	rm -f *~

real_clean: clean
	rm -f $(STATIC_LIB)
	rm -f $(DYNAMIC_LIB)


.cpp.o:
	$(SYS_ARCH_CC) -c $(ARCH_RELOCATE_OPT) $(SRC)/$*.cpp -o $(OBJ)/$@ $(CFLAGS) $(SP)


$(STATIC_LIB): $(TEAL_OBJ)
	@rm -f $(STATIC_LIB)
	$(SYS_ARCH_AR) $(STATIC_LIB) $(TEAL_OBJ)

$(DYNAMIC_LIB): $(TEAL_OBJ)
	@rm -f $(DYNAMIC_LIB)
	$(SYS_ARCH_CC) $(ARCH_DYNAMIC_LD) $(DYNAMIC_LIB_RULE) $(TEAL_OBJ)
