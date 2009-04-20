/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <list>
#include <iostream>

#include <jni.h>

#include <libalf/automaton_constructor.h>

using namespace std;
using namespace libalf;

jintArray basic_string2jintArray(JNIEnv *env, basic_string<int32_t> str)
{
	// Create new Java int array
	int strSize = str.size();
	jintArray arr = env->NewIntArray(strSize);

	// Copy array
	int intArray[strSize];
	int i=0;
	basic_string<int32_t>::iterator si;
	for(si = str.begin(); si != str.end(); si++) {
		intArray[i] = *si;
		i++;
	}

	// Fill Java array
	env->SetIntArrayRegion(arr, 0, strSize, (jint *)intArray);

	return arr;
}

jintArray list_int2jintArray(JNIEnv *env, list<int> l) {
	// Create new Java int array
	jintArray arr = env->NewIntArray(l.size());

	// Copy array
	int intArray[l.size()];
	int i=0;
	list<int>::iterator li;
	for(li = l.begin(); li != l.end(); li++) {
		intArray[i] = *li;
		i++;
	}

	// Fill Java array
	env->SetIntArrayRegion(arr, 0, l.size(), (jint *)intArray);

	return arr;
}

jobject create_transition(JNIEnv* env, int source, int label, int destination) {
	/*
	 *Create new Java LibALFTransition object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/jni/BasicTransition");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'BasicTransition'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "(III)V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'BasicTransition'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_transition = env->NewObject(jcls, jmid, source, label, destination);
	if(jmid == NULL) {
		cout << "Could not create new 'BasicTransition' object!\nReturning NULL\n";
		return NULL;
	}
	return java_transition;
}

jobject convertAutomaton(JNIEnv* env, basic_automaton_holder* automaton) {
	/*
	 *Create new Java LibALFAutomaton object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/jni/BasicAutomaton");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'BaiscAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "(ZII)V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_automaton = env->NewObject(jcls, jmid, automaton->is_dfa, automaton->state_count, automaton->alphabet_size);
	if(jmid == NULL) {
		cout << "Could not create new 'BasicAutomaton' object!\nReturning NULL\n";
		return NULL;
	}

	/*
	 * Copy given automaton, i.e. its initial and final states as well as its transitions.
	 *
	 * First, process the initial states.
	 */
	// Find the add initial states method
	jmid = env->GetMethodID(jcls, "addInitialState", "(I)V");
	if(jmid == 0) {
		cout << "Could not find addInitialState of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all initial states
	set<int>::iterator i;
	for(i = automaton->start.begin(); i != automaton->start.end(); i++)
		// Add state to Java object
		env->CallVoidMethod(java_automaton, jmid, *i);

	/*
	 * Now, process the final states
	 */
	// Find the add final states method
	jmid = env->GetMethodID(jcls, "addFinalState", "(I)V");
	if(jmid == 0) {
		cout << "Could not find addFinalState of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all initial states
	for(i = automaton->final.begin(); i != automaton->final.end(); i++)
		// Add state to Java object
		env->CallVoidMethod(java_automaton, jmid, *i);

	/*
	 * Finally, process the transitions
	 */
	// Find the add transition method
	jmid = env->GetMethodID(jcls, "addTransition", "(Lde/libalf/jni/BasicTransition;)V");
	if(jmid == 0) {
		cout << "Could not find addTransition of 'BasicFAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all transitions
	transition_set::iterator si;
	for(si = automaton->transitions.begin(); si != automaton->transitions.end(); si++) {
		// Add state to Java object
		jobject tr = create_transition(env, si->source, si->label, si->destination);
		env->CallVoidMethod(java_automaton, jmid, tr);
	}

	return java_automaton;
}