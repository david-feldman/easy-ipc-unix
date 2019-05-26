/*
 * * Project: Project 4
 * *
 * * Program: Shared Memory Library
 * * File Name: shared_mem.h 
 * *
 * * Description: Header file for shared memory library
 * *
 * * Programmer: David Feldman
 * * Organization: JHU Part-Time Programs in Engineering and Applied Science
 * * Host System: UNIX 4.3 BSD or Sun OS 4.x
 * * Language: C++
 * * Date Created: 04/21/2019
 * * Modifications:
 * * $Log$
 * */
#ifndef _SHMH
#define _SHMH

#define FTOK_PATH "/home/dfeldma9"

struct shm_segment {
	int key; 
	key_t sys_key;
	int ipd_id;
};

//not used in library but in homework programs that use
struct hw_data_struct {
	int is_valid;
	float x;
	float y;
	hw_data_struct(bool a, float b, float c) : is_valid((int) a), x(b), y(c) {}   
};

/*
This function has two arguments. The first argument serves as the key
for the shared memory segment. The second argument contains the
size (in bytes) of the shared memory segment to be allocated. The
return value for this function is a pointer to the shared memory area
which has been attached (and possibly created) by this function. If, for
some reason, this function cannot connect to the shared memory area
as requested, it shall return a NULL pointer. A program using this
library function must be able to use it to attach the maximum number
of shared memory segments to the calling process. (Note that Solaris
11 does not have a limit to the number of attachments, so you can use
the limit that Linux supports – use the limit you find on our Linux server
absaroka.apl.jhu.edu).
*/
void *connect_shm(int key, int size);

/*
This function detaches the shared memory segment attached to the
process via the argument addr. The associated shared memory
segment is not deleted from the system. This function will return OK
(0) on success, and ERROR (-1) otherwise.
*/
int detach_shm(void *addr);

/*
This function detaches all shared memory segments (attached to the
calling process by connect_shm( )) associated with the argument key
from the calling process. The shared memory segment is then
subsequently deleted from the system. This function will return OK (0)
on success, and ERROR (-1) otherwise.
*/
int destroy_shm(int key);

#endif // _SHMH
