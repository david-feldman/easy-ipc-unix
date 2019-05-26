/*
 * * Project: Project 4
 * *
 * * Program: Shared Memory Library
 * * File Name: shared_mem.cpp
 * *
 * * Description: Shared Memory Library for Homework 4
 * *
 * * Programmer: David Feldman
 * * Organization: JHU Part-Time Programs in Engineering and Applied Science
 * * Host System: UNIX 4.3 BSD or Sun OS 4.x
 * * Language: C++
 * * Date Created: 04/21/2019
 * * Modifications:
 * * $Log$
 * */
#include <sys/time.h>
#include <cstring>
#include <signal.h> 
#include <cctype> 
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <csignal>
#include <stdio.h>
#include <fcntl.h>   
#include <unistd.h>
#include <map>
#include <sys/utsname.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "thread_mgr.h"
#include "log_mgr.h"
#include "shared_mem.h"

static const int ERROR = -1;
static const int OK = 0;
static const int MAX_SHM_SEGMENTS = 4096;
static int SegmentCtr = 0;
static std::map<int, shm_segment*> AllSegments;
static std::multimap<int, void *> AllAttachedAddr;

void *connect_shm(int key, int size) {
	
	std::map<int, shm_segment*>::iterator existing_segment_itr = AllSegments.find(key);
	//doesnt exist and needs to be created
	if (existing_segment_itr == AllSegments.end()) {
		if ( SegmentCtr == MAX_SHM_SEGMENTS ) {
			log_event(WARNING, "Maximum number of segments has already been created");
			return (void *) NULL;
		} 
		SegmentCtr++;
		log_event(INFO, "Creating new segment: %d", key);
		shm_segment *curr_segment = new shm_segment();
		key_t mykey;
		int shmid;
		if ((mykey = ftok (FTOK_PATH, key)) == -1) {
			perror("ftok");
			log_event(WARNING, "FTOK error");
			return (void *) NULL;
		}
		shmid = shmget (mykey, size, IPC_CREAT | 0666);
		curr_segment->key = key;
		curr_segment->sys_key = mykey;
		curr_segment->ipd_id = shmid;
		////std::count << "attached to: " << shmid << std::endl;
		AllSegments[curr_segment->key] = curr_segment;
		int * address = (int *) shmat(curr_segment->ipd_id, NULL, 0);
		if (* address == -1) {
			perror("shmat");
			log_event(WARNING, "shmat error");
			return (void *) NULL;
		} else {
			AllAttachedAddr.insert(std::make_pair(curr_segment->key, (void *) address));
			std::stringstream ss;
			ss << (void *) address;
			std::string add_string = ss.str();
			log_event(INFO, "Address %s returned attached to %d, %d segments now exist in total, %d attachements now exist in total", add_string.c_str(), curr_segment->ipd_id,  SegmentCtr, AllAttachedAddr.size());
			return (void *) address;
		}
 
	} else {
		int *  address = (int *) shmat(existing_segment_itr->second->ipd_id, NULL, 0);
		if (* address == -1) {
			perror("shmat");
			log_event(WARNING, "shmat error");
			return (void *) NULL;
			} else {
			AllAttachedAddr.insert(std::make_pair(existing_segment_itr->second->key, (void *) address)) ;
			return (void *) address;
			std::stringstream ss;
			ss << (void *) address;
			std::string add_string = ss.str();
			log_event(INFO, "Address %s returned attached to %d , %d segments now exist in total, %d attachements now exist in total", add_string.c_str(), existing_segment_itr->second->ipd_id,  SegmentCtr, AllAttachedAddr.size());
		}		
	}
	
}

int detach_shm(void *addr) {
	int ctr = 0;
	////std::count << "Size of Attachments before detach: " << AllAttachedAddr.size() << std::endl;
	for (std::multimap<int, void *>::iterator iter = AllAttachedAddr.begin(); iter != AllAttachedAddr.end();) {
	//	//std::count << "loop iteration!" << std::endl;
	//	//std::count << "checking " << iter->second << " against " << addr << std::endl; 
		std::stringstream ss_iter;
		std::stringstream ss_addr;
		ss_iter << (void *) iter->second;
		ss_addr << (void *) addr;
		std::string str_iter = ss_iter.str();
		std::string str_addr = ss_addr.str();
		if (!str_addr.compare(str_iter)) {
			ctr++;
			if (shmdt (addr) < 0) {
				perror("shmdt");
				log_event(WARNING, "shmdt error");
				return ERROR;
			}
			std::stringstream ss;
			ss << (void *) addr;
			std::string add_string = ss.str();
			log_event(INFO,"Detaching address %s, there are now %d active addresses in total", add_string.c_str(), AllAttachedAddr.size() -1); 
			AllAttachedAddr.erase(iter++);
		} else {
			iter++;	
		}
	}
	if (ctr == 0) {
		// could not find address to delete
		perror("did not locacte addr for detach_shm");
		log_event(WARNING, "detack_shm addr not located");
		return ERROR;
	}
	////std::count << "Size of Attachments after detach: " << AllAttachedAddr.size() << std::endl;
	return OK;
}

int destroy_shm(int key) {

	if (AllSegments.size() == 0) {
		log_event(WARNING,"No segments to destroy owned by process");
		return ERROR;
	}
	//first detach all 
	int ctr = 0;
	//std::count << 1 << std::endl;
	if (AllAttachedAddr.size() > 0) {
		for (std::multimap<int, void *>::iterator iter = AllAttachedAddr.begin(); iter != AllAttachedAddr.end();) {
			//std::count << 2 << std::endl;
			if (iter->first  ==  key) {
				//std::count << 3 << std::endl;
				ctr++;
				if (shmdt (iter->second) < 0) {
					//std::count << 4 << std::endl;
					perror("shmdt");
					log_event(WARNING, "shmdt error");
					return ERROR;
				}
				//std::count << 5 << std::endl;
				std::stringstream ss;
				ss << (void *) iter->second;
				std::string add_string = ss.str();
				log_event(INFO,"Detaching address %s, there are now %d active addresses in total", add_string.c_str(), AllAttachedAddr.size() -1);
				AllAttachedAddr.erase(iter++);
			} else {
				iter++;
			}
		}
	}
	//std::count << 6 << std::endl;
	//now we can destroy segment
	std::multimap<int, shm_segment*>::iterator the_segment = AllSegments.find(key);
	//std::count << 7 << std::endl;
	if (shmctl (the_segment->second->ipd_id, IPC_RMID, 0) == -1) {
		perror ("shmid:IPC_RMID");
		log_event(WARNING, "shmctl IPC_RMID fail %d", the_segment->second->ipd_id);
		return ERROR;
	} else {
		//std::count << 8 << std::endl;
		log_event(INFO,"Deleting segment with key of %d, now %d total segments", key, AllSegments.size() - 1 );
		SegmentCtr--;
		//std::count << 9 << std::endl;
		AllSegments.erase(key);
		//std::count << 10 << std::endl;
	}
	return OK;
}
