# $Id$

.PHONY: all clean install uninstall

all:
	+make -C libAMoRE
	+make -C libAMoRE++
	+make -C libmVCA
	+make -C liblangen
	+make -C libalf
	+make -C finite-automata-tool

clean:
	-rm *.tar.bz2
	-rm -Rf release
	+make -C libAMoRE clean
	+make -C libAMoRE++ clean
	+make -C libmVCA clean
	+make -C liblangen clean
	+make -C libalf clean
	+make -C finite-automata-tool clean

install:
	+make -C libAMoRE install
	+make -C libAMoRE++ install
	+make -C libmVCA install
	+make -C liblangen install
	+make -C libalf install
	+make -C finite-automata-tool install

uninstall:
	+make -C libAMoRE uninstall
	+make -C libAMoRE++ uninstall
	+make -C libmVCA uninstall
	+make -C liblangen uninstall
	+make -C libalf uninstall
	+make -C finite-automata-tool uninstall

