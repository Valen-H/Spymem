# Template Makefile
#	by:  V. H.

CC=gcc
RANLIB=ranlib
CFLAGS=-fPIE
ARFLAGS=rcs
targ=$(shell basename $$PWD)
DEPS=$(targ).h
LINK=-L. -I. -l:liblists.lib
TSTFLAGS=-ggdb -DDEBUG
TEST=test
BINR=bin.c
ifeq ($(OS), Windows_NT)
	LIB=\\usr\\lib
	END=dll
	PRE=lib
	ENDA=lib
	ENDO=exe
else
	LIB=$(PREFIX)/lib
	END=so
	PRE=lib
	ENDA=a
	ENDO=o
endif
INC=$(LIB)/../include
BIN=$(LIB)/../bin

default: all
all: $(targ).$(ENDO) $(PRE)$(targ).$(END) $(PRE)$(targ).$(ENDA) $(TEST).$(ENDO) lib$(targ).$(END).$(ENDA) fix install pack #$(targ)
.PHONY: clean install fix firstinst all default pack

$(targ).$(ENDO): $(targ).c $(DEPS)
	$(CC) $(CFLAGS) -c $(targ).c -o $@

$(PRE)$(targ).$(END): $(targ).$(ENDO)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LINKS)

$(PRE)$(targ).$(ENDA): $(targ).$(ENDO)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(TEST).$(ENDO): $(TEST).c $(targ).$(ENDO)
	$(CC) $(CFLAGS) $^ -o $@ $(TSTFLAGS) $(LINKS)

#$(targ): $(BINR) $(targ).$(ENDO)
#	$(CC) $(CFLAGS) $(BINR) -o $@ -L./ -l$(targ) $(LINKS)


lib$(targ).$(END).$(ENDA): $(PRE)$(targ).$(END)
	echo EXPORTS > $(targ).def
	nm $(PRE)$(targ).$(END) | grep ' T _' | sed 's/.* T _//' >> $(targ).def
	dlltool --def $(targ).def --dllname $(targ).$(END) --output-lib lib$(targ).$(END).$(ENDA)
	$(RANLIB) lib$(targ).$(ENDA)

install: $(PRE)$(targ).$(END)
	install -m 755 lib$(targ).$(END) $(LIB)
	install -m 755 $(targ).h $(INC)
	#install -m 755 $(targ) $(BIN)

clean: *
	rm *.o -f &
	rm *.exe -f &
	rm *.so -f &
	rm *.dll* -f &
	rm *.a -f &
	rm *.stackdump -f &
	rm *.bak -rf

fix: FORCE
	chmod 775 *

firstinst: FORCE
	$(shell export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd`)

pack: *
	$(shell TAR_MODE=755 tar cz -f ../$(targ).tgz ../$(targ) -p)

FORCE:
