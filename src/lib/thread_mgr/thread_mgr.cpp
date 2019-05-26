/*
* Project: Project 3
*
* Program: thread_mgr
* File Name: thread_mgr.cpp
*
* Description: Thread manager library, main part of module 7 assignment. 
*
* Programmer: David Feldman
* Organization: JHU Part-Time Programs in Engineering and Applied Science
* Host System: UNIX 4.3 BSD or Sun OS 4.x
* Language: C++
* Date Created: 03/30/2019
* Modifications:
* $Log$
*/
#include <iostream> /* including to print to stdout / read from stdin */
#include <sstream>  // for string streams 
#include <iomanip>  // for formatting output
#include <fcntl.h>
#include <pthread.h>
#include <algorithm>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cerrno>
#include <map>
#include "thread_mgr.h"
#include "log_mgr.h" 

using namespace std;

static const int THD_ERROR = -1;
static const int THD_OK = 0;
static const char *THREAD_STATE_STRINGS[] = { "ACTIVE", "EXITED", "KILLED" };
static map<pthread_t, thread*> AllThreads;
static pthread_mutex_t SigIntHandleMutex;
static pthread_mutex_t SigQuitHandleMutex;
static int Init = 0; //behaves like a boolean
static int ThrdCtr = 0;

//INTERNAL FUNCTIONS NOT TO BE USED OUTSIDE OF INTERNAL LIBRARY
void siginthandle(int value);
void sigquithandle(int value);
void lib_init();

/*
This library call executes the argument function as an independent thread within the process. The
library will create a name for the thread and maintain it within the library. The library shall also
create a unique integer handle (ThreadHandles) and return it upon successful execution. If the
function fails, it shall return THD_ERROR (-1).
*/
ThreadHandles th_execute(Funcptrs function) {
	//Default return is error
	pthread_t return_value = pthread_t(THD_ERROR);
	//Init if not yet done
	if (!Init) {
		lib_init();
		log_event(INFO, "Init Called");
	}
	//Create the thread
	thread *curr_thread = new thread();
	ThrdCtr++;
	char unique_str[20];
	sprintf(unique_str, "t%d", ThrdCtr);
	curr_thread->name = unique_str;
	//cout << unique_str << "\n";
	if (pthread_create(&curr_thread->id, NULL, function, (void*) (curr_thread))) {
		perror("pthread_create");
		log_event(WARNING, "Could not create thread");
	} else {
		curr_thread->state = ACTIVE;
		AllThreads[curr_thread->id] = curr_thread;
		return_value =  pthread_t(curr_thread->id);
	}
	log_event(INFO, "Thread Created");
	return return_value;
}

/*
This call blocks the calling thread until the thread associated with the argument handle terminates.
This call returns THD_ERROR if the argument is not a valid thread. Otherwise the thread returns
THD_OK (0). After the thread terminates, the thread library should purge the stored thread
information for the argument thread.
*/
int th_wait(ThreadHandles wait_thread) {
	if (!Init) {
		lib_init();
		log_event(INFO, "Init Called");
	}
	map<pthread_t, thread*>::iterator the_thread = AllThreads.find(wait_thread);
	//cant find it, throw error
	if (the_thread == AllThreads.end()) {
		log_event(WARNING, "Could not find thread to block based on");
		return THD_ERROR;
	}
	//block by joining, handle error if cannot
	if (pthread_join(wait_thread, NULL)) {
		perror("pthread_join");
		log_event(WARNING, "Could not join to block");
		return THD_ERROR;
	}
	// tha purge
	the_thread = AllThreads.find(wait_thread);
	if (the_thread != AllThreads.end()) {
		thread* temp = the_thread->second;
		AllThreads.erase(the_thread);
		delete temp;
	}
	log_event(INFO, "Sucessful Wait");
	return THD_OK;
}

/*
This function blocks until all threads in the library terminate. This function returns THD_ERROR if
the library is not managing any threads or upon any other error condition. Otherwise, the function
returns THD_OK after all threads terminate. The thread library should purge the stored thread
information for all threads upon successful execution of this call.
*/
int th_wait_all(void) {
	int size = AllThreads.size(); 
	if (size < 1) {
		log_event(WARNING, "No threads to block");
		return THD_ERROR;
	} else {
		log_event(INFO, "Waiting All...");
		int states[size];
		int ctr = 0; 
		map<pthread_t, thread*>::iterator it;
		for (it = AllThreads.begin(); it != AllThreads.end();) {
			if (pthread_join((it)->first, NULL)) {
					perror("pthread_join");
					log_event(WARNING, "pthread_join failed [thread: %lu]", it->first);
					return THD_ERROR;
				} else {
					if (AllThreads.find(it->first) != AllThreads.end()) {
						thread* temp = it->second;
						AllThreads.erase(it++);
						delete temp;
					}
				}
				//states[ctr++] = th_wait((it++)->first); 
			}
		
	}
	log_event(INFO, "Sucessful Wait All");
	return THD_OK;
}

/*
This function cancels the executing thread associated with the argument thread handle, and
updates the status of the thread appropriately. This function returns THD_ERROR if the argument
is not a valid thread handle. Note that this call is not required to asynchronously kill the thread; the
thread may be cancelled until the thread reaches its cancellation point, and cleaned up after the
application waits for the thread
*/
int th_kill(ThreadHandles kill_thread){
	if (!Init) {
		lib_init();
		log_event(WARNING, "No threads to block");
	}
	map<pthread_t, thread*>::iterator the_thread = AllThreads.find(kill_thread);
	//cant find it, throw error
	if (the_thread == AllThreads.end()) {
		log_event(WARNING, "could not find thread to kill");
		return THD_ERROR;
	}
	//kill thread if poss
	if (pthread_cancel(kill_thread)) {
		perror("issue with pthread_cancel");
		log_event(WARNING, "pthread_cancel failed [thread: %lu]", the_thread->first);
		return THD_ERROR;
	}

	the_thread->second->state = KILLED;
	log_event(INFO,"Thread killed [thread: %lu]",the_thread->first);
	return THD_OK;
}

/*
This function cancels all threads in the library. This function returns THD_ERROR if the library is
not managing any threads. Otherwise, the function returns THD_OK after all threads are cancelled.
*/
int th_kill_all(void) {
	log_event(INFO, "Killing all threads...");
	int size = AllThreads.size(); 
	if (size < 1) {
		log_event(WARNING, "No threads to kill");
		return THD_ERROR;
	} else {
		int states[size];
		int ctr = 0; 
		map<pthread_t, thread*>::iterator it;
		for (it = AllThreads.begin(); it != AllThreads.end();) {
			states[ctr++] = th_kill((it++)->first);
		}
		log_event(INFO, "Kill all complete");
		return *std::min_element(states, states + size);
	}
}

/*
This function should allow the thread that calls this function to clean up its information from the
library and exit. The thread information in the library should not be purged at this time; however,
proper status should be logged to the log file, and the internal status of the thread should be
updated. The thread information in the library shouldn’t be changed until another thread ‘waits’ for
the thread (using one of the ‘th_wait’ calls). This call does not return if executed successfully; thus the only possible return value for this
function is THD_ERROR.
*/
int th_exit(void) {

	if (!Init) {
		lib_init();
		log_event(INFO, "Init Called");
	}
	pthread_t id = pthread_self();
	map<pthread_t, thread*>::iterator the_thread = AllThreads.find(id);
	if (the_thread != AllThreads.end()) {
		the_thread->second->state = EXITED;
	}
	//exit 
	log_event(INFO, "Exiting");
	pthread_exit((void*) id);
	//should never reach this point 
	log_event(WARNING, "Reached Bad Exit");
	return THD_ERROR;
}

/*
Internal fucntion ton initialize mutex & deal with signal handling
*/
void lib_init() {
	//make sure this not run again
	Init++;
	// mutex inits for signal handling
	pthread_mutex_init(&SigIntHandleMutex, NULL);
	pthread_mutex_init(&SigQuitHandleMutex, NULL);
	// init signnal handler
	struct sigaction handler;
	handler.sa_handler = siginthandle;
	sigemptyset(&handler.sa_mask);
	handler.sa_flags = 0;
	if (sigaction(SIGINT, &handler, NULL)) {
		perror("siginthandle invoked");
	}
	handler.sa_handler = sigquithandle;
	sigemptyset(&handler.sa_mask);
	handler.sa_flags = 0;
	if (sigaction(SIGQUIT, &handler, NULL)) {
		perror("sigquithandle invoked");
	}
}


/*
 * Handle SIGINT
 * Adapted from course notes
 */
void siginthandle(int value) {
	log_event(INFO,"SIGINT HANDLED");
	pthread_mutex_lock(&SigIntHandleMutex);

	cout << "\nTHREADS:\n";
	map<pthread_t, thread*>::iterator it;
	for (it = AllThreads.begin(); it != AllThreads.end(); ++it) {
		cout << "name: " << it->second->name << " | " << "thread: " << it->first << " state: " << THREAD_STATE_STRINGS[it->second->state] << "\n" ;
	}
	pthread_mutex_unlock(&SigIntHandleMutex);
}

/*
 * Handle SIGQUIT
 * Adapted from course notes
 */
void sigquithandle(int value) {
	log_event(INFO,"SIGQUIT HANDLED");
	pthread_mutex_lock(&SigQuitHandleMutex);
	th_kill_all();
	// free up memory
	for (map<pthread_t, thread*>::iterator it = AllThreads.begin(); it != AllThreads.end(); ) {
		thread* temp = it->second;
		AllThreads.erase(it++);
		delete temp;
	}
	pthread_mutex_unlock(&SigQuitHandleMutex);
}
