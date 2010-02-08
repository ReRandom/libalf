/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libmvca_mvca_h__
# define __libmvca_mvca_h__

#include <list>
#include <set>
#include <string>
#include <map>

#include <libmVCA/pushdown.h>
#include <libmVCA/transition_function.h>

namespace libmVCA {

using namespace std;

const char * libmVCA_version();

// NOTE: this implementation DOES NOT SUPPORT epsilon transitions.



class mVCA_run {
	public:
		list<int> prefix;
		int state;
		int m;

		mVCA_run()
		{ state = 0; m = 0; };

		mVCA_run(int state, int m)
		{ this->state = state; this->m = m; };
};

// for mVCA_run:
// NOTE: all comparison-functions don't care about the prefix! this way,
// we can easily make a breadth-first search and remember visited
// m/state tuples in a set<mVCA_run> this is used in mVCA::shortest_run
bool operator<(const mVCA_run first, const mVCA_run second);
bool operator==(const mVCA_run first, const mVCA_run second);
bool operator>(const mVCA_run first, const mVCA_run second);




// interface and common functions for nondeterministic_mVCA and deterministic_mVCA.
class mVCA {
	public: // types
		enum mVCA_derivate {
			DERIVATE_NONE = 0,

			DERIVATE_DETERMINISTIC = 1,
			DERIVATE_NONDETERMINISTIC = 2,

			DERIVATE_LAST_INVALID = 3
		};
	protected: // data
		unsigned int state_count;
		pushdown_alphabet alphabet;
		int initial_state;
		set<int> final_states;

		int m_bound; // there exist m_bound+1 transition_functions
//		transition_function :: implemented by deriving classes

		friend mVCA * construct_mVCA(unsigned int state_count, int alphabet_size, set<int> & up, set<int> & stay, set<int> & down, int initial_state, set<int> & final_states, int m_bound, map<int, map<int, map<int, set<int> > > > & transitions);

	public: // methods
		mVCA();

		// ALPHABET
		// set alphabet (will be copied, will erase all other information about former automaton)
		void set_alphabet(pushdown_alphabet & alphabet);
		void unset_alphabet();
		pushdown_alphabet get_alphabet();
		int get_alphabet_size();

		// STATES
		int get_state_count();

		// INITIAL/FINAL STATES
		int get_initial_state();
		set<int> get_initial_states();
		set<int> get_final_states();
		bool set_initial_state(unsigned int state);
		bool set_final_state(const set<int> & states);

		bool contains_initial_states(const set<int> & states);
		bool contains_final_states(const set<int> & states);

		// TRANSITIONS
		// XXX: successor/predecessor/transition maps(?)

		set<int> transition(int from, int & m, int label);
		virtual set<int> transition(const set<int> & from, int & m, int label) = 0; // depends on transition function
		virtual bool endo_transition(set<int> & states, int & m, int label) = 0; // depends on transition function

		set<int> run(const set<int> & from, int & m, list<int>::iterator word, list<int>::iterator word_limit);
		// get shortest run from a state in <from> to a state in <to>.
		// the run has to result in a configuration where the current state is a state in <to> and m is <to_m>.
		list<int> shortest_run(const set<int> & from, int m, const set<int> & to, int to_m, bool &reachable);
		// test if word is contained in language of automaton
		bool contains(list<int> & word);
		bool contains(list<int>::iterator word, list<int>::iterator word_limit);
		// obtain shortest word in language resp. test if language is empty,
		list<int> get_sample_word(bool & is_empty);
		bool is_empty();


		///-----------------------------------
		
		bool lang_subset_of(mVCA & other);
		//bool lang_disjoint_to(mVCA & other);
		//bool lang_complement();

		//mVCA * determinize();
		
		///-----------------------------------

		// obtain id of unique derived class
		virtual enum mVCA_derivate get_derivate_id() = 0;

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	string length (not in bytes but in int32_t; excluding this length field)
		//	derivate id (enum mVCA_derivate)
		//	state_count
		//	<serialized alphabet>
		//	initial_state
		//	number of final states
		//	final_states[]
		//	m_bound
		//	<serialized derivate-specific data>
		// </serialized automaton>
		basic_string<int> serialize();
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		string generate_dotfile();
	protected:
		virtual basic_string<int32_t> serialize_derivate() = 0;
		virtual bool deserialize_derivate(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, int & progress) = 0;
		virtual string get_transition_dotfile() = 0;
};



mVCA * construct_mVCA(	unsigned int state_count,
			int alphabet_size, set<int> & up, set<int> & stay, set<int> & down,
			int initial_state,
			set<int> & final_states,
			int m_bound,
			map<int, map<int, map<int, set<int> > > > & transitions
			// transitions: m -> state -> sigma -> states
		);

mVCA * deserialize_mVCA(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);



}; // end of namespace libmVCA.

#endif // __libmvca_mvca_h__
