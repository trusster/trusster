

#redo this, it should only build one test (not an archive), whose name should be passed in from the run script
STATIC_LIB  = $(LIB)/tests.$(SIM).a
SRCS = \
	$(SRC)/block_uart.cpp 

INCL = -I../vip/uart -I../testbench/top -I../test_components -I../vip/wishbone -I../vip/uart_16550

include $(TRUSS_HOME)/inc/Makefile
