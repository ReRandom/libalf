#include "libalf/algorithm_deterministic_inferring_csp_minisat.h"
#include "libalf/algorithm_counterexample_learning.h"
#include "libalf/knowledgebase.h"
#include "libalf/alphabet.h"
#include <list>

using namespace libalf;

int main(int argc, char**argv)
{
	knowledgebase<bool> base;
	std::list<int> w = word(2,0,1);
	base.add_knowledge(w, true);

	//data fields
	int alphabet_size = 2;
	deterministic_inferring_csp_MiniSat<bool> infer_alg(&base, NULL, alphabet_size);
	
	knowledgebase<bool> cel_base;

	counterexample_learning alg(&cel_base, NULL, alphabet_size, &infer_alg);

	//serialize
	std::cout << "serializing...";
	std::basic_string<int32_t> serial = alg.serialize();
	std::cout << "done." << std::endl;

	//deserialize
	std::cout << "deserializing...";
	counterexample_learning alg2(NULL, NULL, 0, NULL);
	serial_stretch ss(serial);	
	if(alg2.deserialize(ss))
		std::cout << "done.";
	else
		std::cout << "failed.";
	std::cout << std::endl << std::endl;

	
	std::cout << std::endl;
	

	return 0;
}

