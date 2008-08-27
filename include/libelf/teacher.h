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

#include <alphabet.h>

namespace libalf {

enum extended_bool {
	EB_TRUE,
	EB_FALSE,
	EB_UNKNOWN
};

template <class alphabet>
class teacher {

	virtual extended_bool membership_query(list< alphabet >) = 0;

	virtual void membership_query(BDD< alphabet >) = 0;

};

}; // end namespace libalf

