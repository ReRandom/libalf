/* $Id$
 * vim: fdm=marker
 *
 * This file is part of Finite Automata Tools (FAT).
 *
 * FAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FAT.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>

#include <libalf/algorithm_NLstar.h>
//#include <libalf/algorithm_ULstar.h>

#include "fat.h"

#include "amore_alf_glue.h"

//#define SANITY_CHECK_TRANSFORMATION

bool a2rfsa(finite_automaton *& automaton)
{{{
	knowledgebase<bool> base;
	NLstar_table<bool> tbl(&base, NULL, automaton->get_alphabet_size());
	finite_automaton * hypothesis = NULL;

	bool equal = false;

	while(!equal) {
		conjecture *cj;
		simple_moore_machine *ba;

		while( NULL == (cj = tbl.advance()) )
			amore_alf_glue::automaton_answer_knowledgebase(*automaton, base);
		ba = dynamic_cast<simple_moore_machine*>(cj);
		hypothesis = construct_amore_automaton(ba->is_deterministic, ba->input_alphabet_size, ba->state_count, ba->initial_states, ba->final_states, ba->transitions);
		delete cj;

		list<int> cex;
		if(amore_alf_glue::automaton_equivalence_query(*automaton, *hypothesis, cex)) {
			equal = true;
		} else {
			tbl.add_counterexample(cex);
			delete hypothesis;
		}
	}

	delete automaton;
	automaton = hypothesis;

	return true;
}}}

bool do_transformation(finite_automaton *& automaton, transformation trans)
{{{
	finite_automaton * tmp;

#ifdef SANITY_CHECK_TRANSFORMATION
	finite_automaton * cl;
	if(automaton)
		cl = automaton->clone();
#endif

	switch(trans) {
		case trans_none:
			break;
		case trans_mdfa:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			automaton->minimize();
			break;
		case trans_minimize:
			automaton->minimize();
			break;
		case trans_determinize:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			break;
		case trans_co_determinize:
			tmp = automaton->co_determinize();
			delete automaton;
			automaton = tmp;
			break;
		case trans_reverse:
			tmp = automaton->reverse_language();
			delete automaton;
			automaton = tmp;
			break;
		case trans_rfsa:
			if(!a2rfsa(automaton))
				return false;
			break;
		default:
			return false;
	}

#ifdef SANITY_CHECK_TRANSFORMATION
	if(automaton) {
		finite_automaton * difference;
		list<int> w;
		bool empty;

		difference = automaton->lang_symmetric_difference(*cl);
		w = difference->get_sample_word(empty);
		if(!empty) {
			cerr << "transformation-error! mismatching word " << word2string(w) << "\n";
		} else {
			cerr << "transformation ok.\n";
		}

		delete difference;
		delete cl;
	}
#endif

	return true;;
}}}

