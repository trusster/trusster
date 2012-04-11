


LIB = .

STATIC_LIB  = vip.$(SIM).a
LIBS = \
	alu/$(LIB)/alu.$(SIM).a 


all: $(STATIC_LIB)

#NEED TO PASS THE make args down???
$(LIBS) :
	cd alu; make

#GERRY Help create an archive of archives?
$(STATIC_LIB): $(LIBS)
	ar cr $(STATIC_LIB) alu/*.o 

clean:
	-cd alu; make clean; cd ..
	rm -f $(LIBS)
	rm -f $(STATIC_LIB)
