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

#include "co_knowledgebase.h"

co_knowledgebase::co_knowledgebase()
{{{
	o = new knowledgebase<extended_bool>;
}}};

co_knowledgebase::~co_knowledgebase()
{{{
	set<int>::iterator si;

	for(si = referring_learning_algorithms.begin(); si != referring_learning_algorithms.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	for(si = referring_iterators.begin(); si != referring_iterators.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_knowledgebase::handle_command(int command, basic_string<int32_t> & command_data)
{
	string s;
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;

	switch(command) {
		case KNOWLEDGEBASE_SERIALIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			serial = o->serialize();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_raw_blob(serial);
		case KNOWLEDGEBASE_DESERIALIZE:
			si = command_data.begin();
			if(!o->deserialize(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case KNOWLEDGEBASE_TO_DOTFILE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			s = o->generate_dotfile();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_string(s.c_str());
		case KNOWLEDGEBASE_TO_STRING:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			s = o->tostring();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_string(s.c_str());
		case KNOWLEDGEBASE_IS_ANSWERED:
		case KNOWLEDGEBASE_IS_EMPTY:
		case KNOWLEDGEBASE_GET_ALPHABET_SIZE:
		case KNOWLEDGEBASE_COUNT_QUERIES:
		case KNOWLEDGEBASE_COUNT_ANSWERS:
		case KNOWLEDGEBASE_COUNT_RESOLVED_QUERIES:
		case KNOWLEDGEBASE_GET_MEMORY_USAGE:
		case KNOWLEDGEBASE_RESOLVE_QUERY:
		case KNOWLEDGEBASE_RESOLVE_OR_ADD_QUERY:
		case KNOWLEDGEBASE_ADD_KNOWLEDGE:
		case KNOWLEDGEBASE_GET_QUERY_TREE:
		case KNOWLEDGEBASE_MERGE_TREE:
		case KNOWLEDGEBASE_BEGIN:
		case KNOWLEDGEBASE_END:
		case KNOWLEDGEBASE_QBEGIN:
		case KNOWLEDGEBASE_QEND:
		case KNOWLEDGEBASE_CLEAR:
		case KNOWLEDGEBASE_CLEAR_QUERIES:
		case KNOWLEDGEBASE_UNDO:
			return this->sv->send_errno(ERR_NOT_IMPLEMENTED);
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
};

void co_knowledgebase::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_knowledgebase::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

void co_knowledgebase::ref_knowledgebase_iterator(int oid)
{{{
	referring_iterators.insert(oid);
}}};

void co_knowledgebase::deref_knowledgebase_iterator(int oid)
{{{
	referring_iterators.erase(oid);
}}};
