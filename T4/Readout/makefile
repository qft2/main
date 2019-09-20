# Set this to the path where liblab.so has been installed
LIBPATH=/usr/lib
# Set this to the path where the headers have been installed
INCPATH=/usr/include/liblab

PROGS=readout

OBJS=

GPP=/usr/bin/c++
CFLAGS=-Wall -c -I$(INCPATH) -std=gnu++11
LDFLAGS=-Wall -L$(LIBPATH) -llab -lusb-1.0 -lboost_system -lpthread -lCAENComm -std=gnu++11


.PHONY:	all clean

all:	$(PROGS)
	@echo "done."
	
$(PROGS):%:	%.o $(OBJS)
	@echo -n ">>> Linking   "
	$(GPP) $< $(OBJS) -o $@ $(LDFLAGS)

%.o:	%.cpp | %.h
	@echo -n ">>> Compiling "
	$(GPP) $(CFLAGS) $<

%.o:    %.cpp
	@echo -n ">>> Compiling "
	$(GPP) $(CFLAGS) $<

clean:
	@for prog in $(PROGS); do ( rm -f $${prog} $$prog.o ); done
	@for obj in $(OBJS); do ( rm -f $$obj ); done
