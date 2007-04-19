#!/bin/sh
#Copyright (C) 2004 Mike Mintz

#This file is part of the White Toolbox Library.  

ROOT_INCL = -I$(SIMULATOR_HOME)/include -I$(SIMULATOR_HOME)/pli_incs -I$(SIMULATOR_HOME)/pli/interface

#default directory structure
SRC = .
OBJ = .
LIB = .
#INCL = 


#add you own options by:
#OPT

#You must define the following
#STATIC_LIB  = $(LIB)/common.$(SIM).a
#SRCS = \
#           $(SRC)/avv_uart.cpp 


STD_OPT = -Wall $(INCL) $(ROOT_INCL) -I$(TEAL_HOME) -I$(TRUSS_HOME)/inc $(OPT) -DSIMULATOR

ifeq ($(SIM),mti_2_0)
  CFLAGS =  -Dvpi_2_0 -DSIM=mti
else
ifeq ($(SIM),mti)
  CFLAGS =  -DSIM=mti
else
ifeq ($(SIM),ivl)
  CFLAGS =  -Dvpi_2_0 -DSIM=ivl -Divl
else
ifeq ($(SIM),ncsim)
  CFLAGS =  -DSIM=ncsim -Dncsim
else
ifeq ($(SIM),vcs)
  CFLAGS =  -DSIM=vcs -Dvcs
else
ifeq ($(SIM),vcs_2_0)
  CFLAGS =  -DSIM=vcs -Dvcs -Dvpi_2_0
else
ifeq ($(SIM),cver)
  CFLAGS =  -DSIM=cver -Dcver
else
ifeq ($(SIM),cver_2_0)
  CFLAGS =  -DSIM=cver -Dcver -Dvpi_2_0
else
ifeq ($(SIM),aldec)
  CFLAGS =  -DSIM=aldec -Daldec 
else
ifeq ($(SIM),aldec_2_0)
  CFLAGS =  -DSIM=aldec -Daldec -Dvpi_2_0
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

OBJS = ${addsuffix .o, ${basename ${SRCS}}}

build: test_environment build_

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
	@if [ -z "$(TEAL_HOME)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	The environment variable 'TEAL_HOME' is set to the proper" ;\
		echo "	installation directory that has an include and lib subdirectories  e.g." ;\
		echo "	TEAL_HOME = /tools/teal_1_0" ;\
		echo "	" ;\
		exit 1;\
	fi
	@if [ -z "$(TRUSS_HOME)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	The environment variable 'TRUSS_HOME' is set to the proper" ;\
		echo "	installation directory that has an inc subdirectories  e.g." ;\
		echo "	TRUSS_HOME = /tools/white_toolbox_1_1" ;\
		echo "	" ;\
		exit 1;\
	fi


build_: $(STATIC_LIB)
	touch build

clean:
	rm -f $(OBJ)/$(OBJS)
	rm -f build
	rm -f *~
	rm -f $(STATIC_LIB)


.cpp.o:
	gcc -c -Wall -fPIC $(SRC)/$*.cpp -o $(OBJ)/$@ $(CFLAGS) $(STD_OPT)


$(STATIC_LIB): $(OBJ)/$(OBJS)
	@rm -f $(STATIC_LIB)
	ar cr $(STATIC_LIB) $(OBJS)

