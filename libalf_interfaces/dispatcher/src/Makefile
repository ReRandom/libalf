# $Id$
# libalf dispatcher Makefile

.PHONY: all clean install uninstall

include ../../config.mk

VERSIONTAG:=$(shell cat "../../version" 2>/dev/null)
ifeq (${VERSIONTAG},)
GITHASH := $(shell git --no-pager rev-parse --short HEAD)
DIRTYFLAG := $(shell git --no-pager status -s | wc -c | sed -e 's/^[1-9][0-9]*$$/-DIRTY/' -e 's/^0$$//')
VERSION := git-${GITHASH}${DIRTYFLAG}
LIBVERSIONTAG := .git
else
# release branch
VERSION := v${VERSIONTAG}
LIBVERSIONTAG := .${VERSIONTAG}
endif

CPPFLAGS+=-Wall -I ../include/ -I${INCLUDEDIR}
#CPPFLAGS+=-Wextra
CPPFLAGS+=-DVERSION="\"${VERSION}\"" -DVERBOSE_DEBUG
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-L${LIBDIR} -L../../../libalf/src -lz3

OBJECTS=main.o serversocket.o servant.o co_learning_algorithm.o co_knowledgebase.o co_knowledgebase_iterator.o co_logger.o co_normalizer.o

all:	dispatcher

dispatcher: ${OBJECTS}
	${CXX} ${OBJECTS} ${LDFLAGS} -lalf -o dispatcher

dispatcher-static: ${OBJECTS}
	make -C ../../../libalf/src libalf.a
	${CXX} ${OBJECTS} ${LDFLAGS} -static -lalf -o dispatcher

clean:
	-rm -f dispatcher *.o 2>&1

install: dispatcher
	@echo
	@echo installing dispatcher to ${DESTDIR}${BINDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${BINDIR}
	install -v -m 755 dispatcher ${DESTDIR}${BINDIR}

uninstall:
	@echo
	@echo removing dispatcher from ${DESTDIR}${BINDIR} ...
	@echo
	rm -f ${DESTDIR}${BINDIR}/dispatcher

