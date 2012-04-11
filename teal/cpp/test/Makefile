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


include ../Makefile.$(ARCH)
SRC = .
OBJ = .
LIB = .
INCL = .

ifeq ($(SIM),mti_2_0)
  WORK_LIB        = worklib
else
  ifeq ($(SIM),mti)
      WORK_LIB        = worklib
  else
    ifeq ($(SIM),ncsim)
      WORK_LIB        = INCA_libs/ncvlog_lib worklib
    else
      WORK_LIB = 
    endif
  endif
endif


TEST_TARGETS    = compile elaborate simulate verify

VLOG_FILES      = tc_mux.v
DESIGN_TOPS     = $(WORK_LIB).top

SNAPSHOT_NAME   = $(WORK_LIB).ncvlog_lib:ncvlog_lib
COMP_OPTIONS    = -MESSAGES -REDUCE_MESSAGES -NOCOPYRIGHT -LOGFILE ncvlog.log
ELAB_OPTIONS    = -MESSAGES -REDUCE_MESSAGES -NOCOPYRIGHT -ACCWARN -LIBNAME ncvlog_lib -LOGFILE ncelab.log
SIM_OPTIONS     = -MESSAGES -REDUCE_MESSAGES -NOCOPYRIGHT -ACCWARN -RUN -LOGFILE ncsim.log

ROOT_INCL = -I$(SIMULATOR_HOME)/include -I$(SIMULATOR_HOME)/pli_incs -I$(SIMULATOR_HOME)/pli/interface
TEAL_INST = ..

#WARNING: Need to have TEAL first, since "memory.h" is a Linux standard include
# and ivl, by default, installes in /usr/include
SP =  -I$(TEAL_INST) $(ROOT_INCL)

#EXPORT_LIB_PATH = setenv LD_LIBRARY_PATH "./:../:$(DEFAULT_LIBPATH);/usr/local/lib"; export LD_LIBRARY_PATH

build: test_environment $(TEST).$(SIM).so

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
	@if [ -z "$(TEST)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	the make or environment variable 'TEST' is set to the test" ;\
		echo "	you want to run. e.g." ;\
		echo "	setenv TEST memory_test or" ;\
		echo "	setenv semaphore_test vcs7.2 or ..." ;\
		echo "	" ;\
		exit 1;\
	fi
	@if [ -z "$(SIM)" ] ; then \
		echo "	" ;\
		echo "	Before running this example Please make sure that" ;\
		echo "	the make or environment variable 'SIM' is set to the proper" ;\
		echo "	simulator. e.g." ;\
		echo "	setenv SIM ncsim or" ;\
		echo "	setenv SIM mti or" ;\
		echo "	setenv SIM ivl or " ;\
		echo "	setenv SIM vcs7.2 or ..." ;\
		echo "	" ;\
		exit 1;\
	fi

memory_test : memory_test.so
	-mkdir ncvlog_lib
	ncxlmode \
	  -l memory_test.log \
	  +loadpli1=../memory_test.so:vlog_startup_routines \
	memory_test.v	

ifeq ($(SIM),mti_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT)  -Dvpi_2_0 -DSIM=mti
else
ifeq ($(SIM),mti)
  CFLAGS = -Wall $(OPT) $(D_OPT)  -DSIM=mti
else
ifeq ($(SIM),ivl)
  CFLAGS = -Wall $(OPT) $(D_OPT) -Dvpi_2_0 -DSIM=ivl -Divl
else
ifeq ($(SIM),ncsim)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=ncsim -Dncsim -Dvpi_2_0 
else
ifeq ($(SIM),vcs)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=vcs -Dvcs
else
ifeq ($(SIM),vcs_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=vcs -Dvcs -Dvpi_2_0
else
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=$SIM -Dvpi_2_0
endif
ifeq ($(SIM),cver)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=cver -Dcver
else
ifeq ($(SIM),cver_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=cver -Dcver -Dvpi_2_0
else
ifeq ($(SIM),aldec_2_0)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=aldec -Daldec -Dvpi_2_0 -I/home/mmintz/riviera-2005.08/gcc/include
else
ifeq ($(SIM),aldec)
  CFLAGS = -Wall $(OPT) $(D_OPT) -DSIM=aldec -Daldec
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

#	ncvlog memory_test.v
.cpp.o:
	$(SYS_ARCH_CC) -c $(ARCH_RELOCATE_OPT) $(SRC)/$*.cpp -o $(OBJ)/$@ $(CFLAGS) $(SP)
$(TEST).so: $(TEST).o
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(CFLAGS) $(SP) -L..-L/usr/local/lib  ../libteal.a -lstdc++ -lpthread  -shared


$(TEST).$(SIM).so: $(TEST).o ../libteal.$(SIM).a
ifeq ($(SIM),mti)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L..  -L/home/mmintz/riviera-2005.08/gcc/lib  -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),mti_2_0)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),ivl)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(CFLAGS) $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared -lveriuser -lvpi
	iverilog -m./$@ -o $(TEST) -s top $(TEST).v
endif
ifeq ($(SIM),ncsim)
#	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o libpli.so $(SP)  -L.. -L/usr/local/lib  -L. ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o libvpi.so $(SP)  -L.. -L/usr/local/lib  -L. ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),vcs)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),vcs_2_0)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),cver)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),cver_2_0)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L.. -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),aldec)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) $(TEST).o -o $@ $(SP)  -L/usr/local/lib  ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif
ifeq ($(SIM),aldec_2_0)
	$(SYS_ARCH_CC) $(ARCH_RELOCATE_OPT) /home/mmintz/riviera-2005.08/gcc/lib/libc.a  $(TEST).o -o $@ $(SP)   ../libteal.$(SIM).a -lstdc++ -lpthread  -shared $(CFLAGS)
endif

################# Target section

automatic: clean test clean

test:  setup compile_cpp compile elaborate simulate verify

setup:
	-$(MKDIR) $(WORK_LIB)

#CFLAGS = -Wall $(OPT) $(D_OPT)
SRC = .

memory_test : memory_test.so memory_test.v

compile_cpp: memory_test.so


compile:
	-$(VLOGCOMPILER) $(COMP_OPTIONS) $(VLOG_FILES)

elaborate:
	-$(EXPORT_LIB_PATH); $(ELABORATOR) $(ELAB_OPTIONS) $(DESIGN_TOPS) -SNAPSHOT $(SNAPSHOT_NAME)

simulate:
	-$(EXPORT_LIB_PATH); $(SIMULATOR) $(SIM_OPTIONS) $(SNAPSHOT_NAME)

verify: 
	-$(DIFF) ncsim.log test.au > ncsim.df
	-$(DIFF) toggle_count.nets toggle_count.nets.au > toggle_count.nets.df
	-$(DIFF) toggle_count.ports toggle_count.ports.au > toggle_count.ports.df

realclean: clean

clean:
	-$(RM) $(WORK_LIB) *.o *.so *~ *test *_results.txt


#ncxlmode +LOADPLI=./memory_test.so memory_test.v
