

all :
	cd uart; make
	cd wishbone; make
	cd uart_16550; make
	ar cr $(STATIC_LIB) uart/*.o wishbone/*.o uart_16550/*.o

LIB = .

STATIC_LIB  = vip.$(SIM).a
LIBS = \
	uart/$(LIB)/uart.$(SIM).a \
	uart_16550/$(LIB)/uart_16550.$(SIM).a \
	wishbone/$(LIB)/wishbone.$(SIM).a \

all: $(STATIC_LIB)

#NEED TO PASS THE make args down???
$(LIBS) :
	cd uart; make
	cd wishbone; make
	cd uart_16550; make

#GERRY Help create an archive of archives?
$(STATIC_LIB): $(LIBS)
#	ar cr $(STATIC_LIB) $(LIBS)
#	cp  uart/$(LIB)/uart.$(SIM).a $(STATIC_LIB)
	ar cr $(STATIC_LIB) uart/*.o wishbone/*.o uart_16550/*.o

clean:
	-cd uart; make clean; cd ..
	-cd wishbone; make clean; cd ..
	-cd uart_16550; make clean; cd ..
	rm -f $(LIBS)
