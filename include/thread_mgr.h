/*
* Project: Project 3
*
* Program: thread_mgr
* File Name: thread_mgr.cpp
*
* Description: Header file for thread manager library, part of module 7 assignment. 
*
* Programmer: David Feldman
* Organization: JHU Part-Time Programs in Engineering and Applied Science
* Host System: UNIX 4.3 BSD or Sun OS 4.x
* Language: C++
* Date Created: 03/30/2019
* Modifications:
* $Log$
*/
#ifndef _THREADMGRH
#define _THREADMGRH

/*
state for each thread
*/
typedef enum {
	ACTIVE, EXITED, KILLED
} ThreadStates;


/*
Basic info for each thread to be kept in this struct
*/
struct thread {
	pthread_t id;
	std::string name;
	ThreadStates state;
};

typedef pthread_t ThreadHandles;
typedef void *Funcptrs(void *);

//ASSIGNMENT REQUIRED FUNCTIONS
/*
 * This library call executes the argument function as an independent thread within the process. The
 * library will create a name for the thread and maintain it within the library. The library shall also
 * create a unique integer handle (ThreadHandles) and return it upon successful execution. If the
 * function fails, it shall return THD_ERROR (-1).
 * */
ThreadHandles th_execute(Funcptrs function);
/*
 * This call blocks the calling thread until the thread associated with the argument handle terminates.
 * This call returns THD_ERROR if the argument is not a valid thread. Otherwise the thread returns
 * THD_OK (0). After the thread terminates, the thread library should purge the stored thread
 * information for the argument thread.
 * */
int th_wait(ThreadHandles wait_thread);
/*
 * This function blocks until all threads in the library terminate. This function returns THD_ERROR if
 * the library is not managing any threads or upon any other error condition. Otherwise, the function
 * returns THD_OK after all threads terminate. The thread library should purge the stored thread
 * information for all threads upon successful execution of this call.
 * */
int th_wait_all(void);
/*
 * This function cancels the executing thread associated with the argument thread handle, and
 * updates the status of the thread appropriately. This function returns THD_ERROR if the argument
 * is not a valid thread handle. Note that this call is not required to asynchronously kill the thread; the
 * thread may be cancelled until the thread reaches its cancellation point, and cleaned up after the
 * application waits for the thread
 * */
int th_kill(ThreadHandles);
/*
 * This function cancels all threads in the library. This function returns THD_ERROR if the library is
 * not managing any threads. Otherwise, the function returns THD_OK after all threads are cancelled.
 * */
int th_kill_all (void);
/*
 * This function should allow the thread that calls this function to clean up its information from the
 * library and exit. The thread information in the library should not be purged at this time; however,
 * proper status should be logged to the log file, and the internal status of the thread should be
 * updated. The thread information in the library shouldn’t be changed until another thread ‘waits’ for
 * the thread (using one of the ‘th_wait’ calls). This call does not return if executed successfully; thus the only possible return value for this
 * function is THD_ERROR.
 * */
int th_exit(void);


#endif //_THREADMGRH
