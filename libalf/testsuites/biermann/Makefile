# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lm -lalf -lAMoRE++ -llangen

all: biermann_static biermann_regex

nodata:
	-rm -f *.dot *.jpg 2>&1

clean: nodata
	-rm -f *.o biermann_regex biermann_static 2>&1

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

biermann_static: biermann_static.cpp

biermann_regex: biermann_regex.cpp

