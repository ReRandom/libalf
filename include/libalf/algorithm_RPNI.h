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


/*
 * RPNI (regular positive, negative inference) is an offline learning algorithm,
 * described in e.g.
 *	* P. Graćıa and J. Oncina. Inferring regular languages in polynomial update time
 *	* D. Neider, Learning Automata for Streaming XML Documents
 */


#ifndef __libalf_algorithm_rpni_h__
# define __libalf_algorithm_rpni_h__

#include <string>
#include <list>
#include <map>
#include <queue>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class RPNI : public learning_algorithm<answer> {
	public:	// types
		class equivalence_relation {
			public: // types
				typedef typename knowledgebase<answer>::node node;
				typedef pair<node*, node*> nodeppair;
			public: // data
				knowledgebase<answer> * base;
				set<nodeppair> equivalences;
			protected:
				set<nodeppair> candidates;

			public: // member functions
				equivalence_relation(knowledgebase<answer> * my_knowledge)
				{{{
					this->base = my_knowledge;

					// add all trivial equivalences
					kIterator_lex_graded<answer> kit(base->get_rootptr());

					while(!kit.end()) {
						nodeppair p;
						p.first = &*kit;
						p.second = &*kit;
						equivalences.insert(p);
					}
				}}}
				equivalence_relation(equivalence_relation & copy_from)
				{ *this = copy_from; }
				~equivalence_relation()
				{ }

				equivalence_relation operator=(equivalence_relation & copy_from)
				{{{
					base = copy_from->base;
					equivalences = copy_from->equivalences;
					candidates = copy_from->candidates;
				}}}

				bool add_if_possible(node * a, node * b)
				{{{
					bool ok;

					ok = add(a, b);

					if(ok) {
						typename set<nodeppair>::iterator ci;
						for(ci = candidates.begin(); ci != candidates.end(); ci++)
							equivalences.insert(*ci);
					}

					candidates.clear();
					return ok;
				}}}

				set<node*> get_equivalence_class(node * n)
				{{{
					set<node*> ret;
					typename set<nodeppair>::iterator eqi;

					for(eqi = equivalences.begin(); eqi != equivalences.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);

					return ret;
				}}}

				bool are_equivalent(node * a, node * b)
				{{{
					pair<node*, node*> p;
					p.first = a;
					p.second = b;

					return equivalences.find(p) != equivalences.end();
				}}}
			protected:
				set<node*> get_equivalence_class_candidate(node * n)
				{{{
					set<node*> ret;
					typename set<nodeppair>::iterator eqi;

					for(eqi = equivalences.begin(); eqi != equivalences.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);
					for(eqi = candidates.begin(); eqi != candidates.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);

					return ret;
				}}}
				bool are_candidate_equivalent(node * a, node * b)
				{{{
					pair<node*, node*> p;
					p.first = a;
					p.second = b;

					if(equivalences.find(p) != equivalences.end())
						return true;
					return candidates.find(p) != candidates.end();
				}}}

				bool add(node * a, node * b)
				// FIXME: check if really all possible combinations are added and checked!
				{{{
					if(!are_candidate_equivalent(a, b)) {
						set<nodeppair> pending_equivalences;
						typename set<nodeppair>::iterator pi;


						set<node*> ca, cb;

						ca = get_equivalence_class_candidate(a);
						cb = get_equivalence_class_candidate(b);

						typename set<node*>::iterator cai, cbi;

						for(cai = ca.begin(); cai != ca.end(); cai++) {
							for(cbi = cb.begin(); cbi != cb.end(); cbi++) {
								nodeppair p;

								if((*cai)->is_answered() && (*cbi)->is_answered())
									if((*cai)->get_answer() != (*cbi)->get_answer())
										return false; // consistency failure. the requested equivalence is not possible with this sample set.

								p.first = *cai;
								p.second = *cbi;
								candidates.insert(p);

								p.first = *cbi;
								p.second = *cai;
								candidates.insert(p);

								for(int sigma = 0; sigma < p.first->max_child_count(); sigma++) {
									node *r, *s;
									r = (*cai)->find_child(sigma);
									s = (*cbi)->find_child(sigma);
									if(r != NULL && s != NULL) {
										nodeppair q;
										q.first = r;
										q.second = s;
										pending_equivalences.insert(q);
									}
								}
							}
						}

						for(pi = pending_equivalences.begin(); pi != pending_equivalences.end(); pi++)
							if(!add(pi->first, pi->second))
								return false;
					}

					return true;
				}}}
		};

	protected: // data

	public: // methods
		RPNI(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}
		virtual ~RPNI()
		{{{
			// nothing
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{ /* FIXME: maybe keep some stats from last run? */ }

		virtual bool sync_to_knowledgebase()
		{{{
			return true;
		}}}

		virtual bool supports_sync()
		{{{
			return true;
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			// we don't have any internal, persistent data
			ret += htonl(1);
			ret += htonl(learning_algorithm<answer>::ALG_RPNI);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(it == limit) return false;
			if(ntohl(*it) != 1)
				return false;

			it++; if(it == limit) return false;
			if(ntohl(*it) != learning_algorithm<answer>::ALG_RPNI)
				return false;

			it++;

			return true;
		}}}

		virtual void print(ostream &os)
		{{{
			os << tostring();
		}}}
		virtual string tostring()
		{{{
			// FIXME
			string s;
			return s;
		}}}

		// conjecture is always ready if there is a non-empty knowledgebase
		virtual bool conjecture_ready()
		{{{
			return ( (this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0) );
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "RPNI does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		virtual bool complete()
		{{{
			// we're offline.
			return true;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(bool & t_is_dfa, int & t_alphabet_size, int & t_state_count, set<int> & t_initial, set<int> & t_final, multimap<pair<int, int>, int> & t_transitions)
		{
			equivalence_relation eq(this->my_knowledge);

			merge_states(eq);

			// now construct automaton from that
			
			
			

			return true;
		}
		virtual void merge_states(equivalence_relation & eq)
		{{{
			kIterator_lex_graded<answer> lgo(this->my_knowledge->get_rootptr());
			int lgo_index = 0;

			lgo++;
			lgo_index++;

			while( ! lgo.end()) {

				kIterator_lex_graded<answer> lgo2(this->my_knowledge->get_rootptr());

				// check that current state is not equivalent to some smaller
				bool skip = false;
				while((&*lgo) != (&*lgo2)) {
					if(eq.are_equivalent(&*lgo, &*lgo2)){
						skip = true;
						break;
					}
					lgo2++;
				}

				if(!skip) {
					// check for all smaller states, if one can be joined with this one
					lgo2.set_root(this->my_knowledge->get_rootptr());
					while(&*lgo != &*lgo2) {
						// FIXME: we should only do this once for each equivalence class,
						// not for each node.

						if(eq.add_if_possible(&*lgo, &*lgo2))
							break;

						lgo2++;
					}
				}

				lgo++;
				lgo_index++;
			}
		}}}

};


}; // end namespace libalf

#endif // __libalf_algorithm_rpni_h__

