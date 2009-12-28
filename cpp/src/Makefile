
include $(TEAL_HOME)/Makefile


SRC = .

STATIC_LIB  = $(LIB)/truss.$(SIM).a
SRCS = \
           $(SRC)/truss_watchdog.cpp \
           $(SRC)/truss_verification_top.cpp \
	   $(PROJECT_HOME)/verification/tests/$(TEST).cpp

TRUSS_OBJ = ${addsuffix .o, ${basename ${SRCS}}}


OPT = -DTEST=$(TEST) -DTEST_H=\\\"$(TEST).h\\\"  -DTEST_NAME=\\\"$(TEST)\\\"

#for gcc compile us the following line
#OPT = -DTEST=$(TEST) -DTEST_H=\"$(TEST).h\"  -DTEST_NAME=\"$(TEST)\"


#include $(TRUSS_HOME)/inc/Makefile
#STD_OPT = -Wall $(INCL) $(ROOT_INCL) -I$(TEAL_HOME) -I$(TRUSS_HOME)/inc $(OPT) -DSIMULATOR

#The truss scripts takes care of building teal first
#build_truss:  $(TRUSS_OBJ)
build_truss:
	vlib $(PROJECT_HOME)/results/work
	sccom -scv  $(OPT) $(SRCS) -work $(PROJECT_HOME)/results/work  $(STD_OPT) $(ROOT_INCL) $(INCL)

	echo "Completed compiling" $(TRUSS_OBJ)

#	$(SYS_ARCH_CC) $(ARCH_DYNAMIC_LD) $(DYNAMIC_LIB_RULE) $(TRUSS_OBJ)	
#	vlib truss_$(TEST)
#	sccom -scv  $(OPT) $(SRCS) -work $(PROJECT_HOME)/results/work  $(STD_OPT) $(ROOT_INCL) $(INCL)
#	sccom -scv  $(OPT) $(SRCS) -work truss_$(TEST) $(STD_OPT) $(ROOT_INCL) $(INCL)

#build: clean



clean:
	rm -f $(TRUSS_OBJ)
