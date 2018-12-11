# Template Makefile
#	by:  V. H.

CC=gcc
RANLIB=ranlib
CFLAGS=-fPIE
ARFLAGS=rcs
targ=$(shell basename $$PWD)
DEPS=$(targ).h
TSTFLAGS=-ggdb -DEBUG
TEST=test
BINR=bin.c
ifeq ($(OS), Windows_NT)
	LIB=/usr/lib
	END=dll.a
	PRE=cyg
else
	LIB=$(PREFIX)/lib
	END=so
	PRE=lib
endif
INC=$(LIB)/../include
BIN=$(LIB)/../bin
LINKS=-L$(LIB) -I$(INC) -llists

default: all
all: $(targ).o $(PRE)$(targ).$(END) $(PRE)$(targ).a $(TEST).o fix install pack #$(targ)
.PHONY: clean install fix firstinst all default pack

$(targ).o: $(targ).c $(DEPS)
	$(CC) $(CFLAGS) -c $(targ).c -o $@

$(PRE)$(targ).$(END): $(targ).o
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LINKS)

$(PRE)$(targ).a: $(targ).o
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(TEST).o: $(TEST).c $(targ).o
	$(CC) $(CFLAGS) $^ -o $@ $(TSTFLAGS) $(LINKS)

#$(targ): $(BINR) $(targ).o
#	$(CC) $(CFLAGS) $(BINR) -o $@ -L./ -l$(targ) $(LINKS)


install: $(PRE)$(targ).$(END)
	install -m 755 $(PRE)$(targ).$(END) $(LIB)
	install -m 755 $(targ).h $(INC)
	#install -m 755 $(targ) $(BIN)

clean: *
	rm *.o &
	rm *.so &
	rm *.a &
	rm *.exe &
	rm *.dll.*

fix: FORCE
	chmod 775 *

firstinst: FORCE
	$(shell export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd`)

pack: *
	$(shell TAR_MODE=755 tar cz -f ../$(targ).tgz ../$(targ) -p)

FORCE:
