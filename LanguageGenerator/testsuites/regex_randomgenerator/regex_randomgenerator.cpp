/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFArandomgenerator: testsuite for DFA random generator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include <LanguageGenerator/regex_randomgenerator.h>

using namespace LanguageGenerator;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	int alphabet_size;
	int num_op;

	if(argc != 3) {
		cout << "please give <alphabet size> and <operand count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	num_op = atoi(argv[2]);

	string regex;
	regex_randomgenerator rrg;

	float p_sigma[alphabet_size];
	float peps, pcon, puni, pstar;
	for(int i = 0; i < alphabet_size; i++)
		p_sigma[i] = 0.2;
	peps = 0;
	pcon = alphabet_size;
	puni = alphabet_size;
	pstar = alphabet_size;

	regex = rrg.generate(num_op, alphabet_size, p_sigma, peps, pcon, puni, pstar);

	bool success;
	nondeterministic_finite_automaton automaton(alphabet_size, regex.c_str(), success);

	if(!success) {
		cout << "failed to create automaton from regex!\n";
		return 1;
	}

	finite_automaton * dfa;

	dfa = automaton.determinize();
	dfa->minimize();

	printf("NFA has %3d, mDFA has %3d states. RegEx: %s\n", automaton.get_state_count(), dfa->get_state_count(), regex.c_str());

	delete dfa;

	return 0;
}

