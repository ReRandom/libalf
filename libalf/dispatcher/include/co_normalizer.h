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

#ifndef __libalf_dispatcher_co_normalizer_h__
# define __libalf_dispatcher_co_normalizer_h__

#include <libalf/normalizer.h>

#include "client_object.h"

using namespace libalf;

class co_normalizer : public client_object {
	private:
		normalizer * o;

	public:
		// type MUST be valid.
		co_normalizer(enum libalf::normalizer::type type);

		virtual ~co_normalizer();

		virtual enum object_type get_type()
		{ return OBJ_NORMALIZER; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);
};

#endif // __libalf_dispatcher_co_normalizer_h__

