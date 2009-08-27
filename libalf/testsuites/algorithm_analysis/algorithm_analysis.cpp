/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>
#include <libalf/algorithm_NLstar.h>
#include <libalf/knowledgebase.h>

#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/regex_randomgenerator.h>

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include "local_amore_alf_glue.h"

using namespace std;
using namespace libalf;
using namespace amore;
using namespace liblangen;

int main(int argc, char**argv)
{
	int num_testcases, min_msize, max_msize, min_asize, max_asize, model_size_step;

	if(argc != 7) {
		cout << "please give the following parameters:\n"
			"\t1: number of testcase per construction method, alphabet size and model size\n"
			"\t2: minimal alphabet size\n"
			"\t3: maximal alphabet size\n"
			"\t4: minimal model size\n"
			"\t5: maximal model size\n"
			"\t6: model size steps\n";
		return 1;
	}

	num_testcases = atoi(argv[1]);
	min_asize = atoi(argv[2]);
	max_asize = atoi(argv[3]);
	min_msize = atoi(argv[4]);
	max_msize = atoi(argv[5]);
	model_size_step = atoi(argv[6]);

	dfa_randomgenerator dfa_rg;
	nfa_randomgenerator nfa_rg;
	regex_randomgenerator regex_rg;

	int method, model_size, alphabet_size, testcase_index;

	int model_index = 0;
	int max_model_index = num_testcases * (max_asize - min_asize + 1) * (max_msize - min_msize + 1) * 3 /* 3 different kinds of model generation */ - 1;

	ostream_logger log(&cout, LOGGER_DEBUG);
	knowledgebase<bool> base;
	char logline[1024];
	ofstream statfile;
	statfile.open("statistics");
	statfile << "# model_index alphabet_size method model_size mDFA_size RFSA_size - L*-membership L*-uniq_membership L*-equivalence - L*col-membership L*col-uniq_membership L*col-equivalence - NL*-membership NL*-uniq_membership NL*-equivalence\n";

	for(alphabet_size = min_asize; alphabet_size <= max_asize; ++alphabet_size) {
		for(method = 0; method <= 2; method++) {
			for(model_size = min_msize; model_size <= max_msize; model_size += model_size_step) {
				for(testcase_index = 0; testcase_index < num_testcases; ++testcase_index) {
					// construct automaton according to method
					finite_automaton * model;

					bool f_is_dfa;
					int f_alphabet_size, f_state_count;
					std::set<int> f_initial, f_final;
					multimap<pair<int,int>, int> f_transitions;

model_too_big:

					// {{{ construct model
					if(method != 2) {
						if(method == 0) {
							// 0 == DFA
							if(!dfa_rg.generate(alphabet_size, model_size,
									f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
								cout << "failed to generate random DFA!\n";
								return 1;
							}
							dfa_rg.discard_tables(); // we need space
						} else {
							// 1 == NFA
							if(!nfa_rg.generate(alphabet_size, model_size, 2, 0.5, 0.5,
									f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
								cout << "failed to generate random NFA!\n";
								return 1;
							}

						}
						model = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
						if(!model) {
							cout << "failed to construct automaton from generated data!\n";
							return 1;
						}
					} else {
						// 2 == RegEx
						std::string regex;
						bool success;
						regex = regex_rg.generate(alphabet_size, model_size, 0.556, 0.278, 0.166);
						model = new nondeterministic_finite_automaton(alphabet_size, regex.c_str(), success);
						if(!success) {
							cout << "failed to construct NFA from regex!\n";
							return 1;
						}
					}
					/// }}}

					int stat_size_model = model->get_state_count();

					// save model to file, then determinize and minimize model so we're faster {{{
					char modelfilename[128];
					ofstream modelfile;

					snprintf(modelfilename, 128, "model_%08d.dot", model_index);

					modelfile.open(modelfilename);
					modelfile << model->generate_dotfile();
					modelfile.close();

					finite_automaton * tmp;
					tmp = model;
					model = tmp->determinize();
					delete tmp;
					model->minimize();
					// }}}

					int stat_size_mDFA = model->get_state_count();

					if(stat_size_mDFA > max_msize * 3) {
						delete model;
						goto model_too_big;
					}

					log(LOGGER_INFO, "completed %5.1f%% [model %d/%d size %d] (current alphabet size %d, method %d, model size %d)   \r",
							(float)model_index / max_model_index * 100, model_index, max_model_index, stat_size_mDFA , alphabet_size, method, model_size);

					// learn model with different algorithms
					learning_algorithm<bool> * alg;
					statistics stats[3];

					int stat_size_RFSA;

					for(int learner = 0; learner <= 2; learner++) {
						base.clear();
						switch (learner) {
							case 0: alg = new angluin_simple_table<bool>(&base, &log, alphabet_size); break;
							case 1: alg = new angluin_col_table<bool>(&base, &log, alphabet_size); break;
							case 2: alg = new NLstar_table<bool>(&base, &log, alphabet_size); break;
						}

						finite_automaton * hypothesis;
						bool equal = false;
						int iteration = 0;
						while(!equal) {
							while(! alg->advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions) )
								stats[learner].queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*model, base);

							hypothesis = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
							if(!hypothesis) {
								cout << "failed to construct hypothesis!\n";
								return 1;
							}

							list<int> counterexample;
							stats[learner].queries.equivalence++;
							stats[learner].queries.membership = base.count_resolved_queries();
							log(LOGGER_DEBUG, "eq query. \r");
							if(amore_alf_glue::automaton_equivalence_query(*model, *hypothesis, counterexample)) {
							log(LOGGER_DEBUG, "completed \r");
								equal = true;
								if(learner == 2)
									stat_size_RFSA = hypothesis->get_state_count();
							} else {
								alg->add_counterexample(counterexample);
							}

							delete hypothesis;
							iteration++;
						}

						delete alg;
					}

					// save stats:
					//		model_index alphabet_size method model_size mDFA_size RFSA_size
					// (L* stats)
					//		- membership uniq_membership equivalence
					// (L*_col stats)
					//		- membership uniq_membership equivalence
					// (NL* stats)
					//		- membership uniq_membership equivalence
					snprintf(logline, 1024, "%d %d %d %d %d %d - %d %d %d - %d %d %d - %d %d %d\n",
							model_index, alphabet_size, method, stat_size_model, stat_size_mDFA, stat_size_RFSA,
							stats[0].queries.membership, stats[0].queries.uniq_membership, stats[0].queries.equivalence,
							stats[1].queries.membership, stats[1].queries.uniq_membership, stats[1].queries.equivalence,
							stats[2].queries.membership, stats[2].queries.uniq_membership, stats[1].queries.equivalence
							);

					statfile << logline;

					delete model;
					model_index++;
				}
			}
		}
	}

	log(LOGGER_INFO, "done. thanks for your (non)attention :-)\n");

	statfile.close();

	return 0;
}
