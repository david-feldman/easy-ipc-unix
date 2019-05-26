/*
 * * Project: Project 4
 * *
 * * Program: Monitor Shm
 * * File Name: monitor_shm.cpp
 * *
 * * Description: Test the Shared Memory Library for Homework 4
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
#include <fstream>
#include "thread_mgr.h"
#include "log_mgr.h"
#include "shared_mem.h"

volatile std::sig_atomic_t ALARM_STATUS = false;
static const int SEGMENT_SIZE = 512; //we really only need 160 i believe but, hey, 512 bytes is quite cheap, so safety first
static const int SEGMENT_KEY = 414; // this is a random int that happens to be our class number :)
static const int ERROR = -1;
static const int OK = 0;

void alarm_handler(int signal) {
        ALARM_STATUS = true;
}

void my_sleep (int seconds) {
        struct itimerval it_val;  /* for setting itimer */
        log_event(INFO, "Waiting for %d seconds", seconds);
        if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
                perror("Unable to catch SIGALRM");
                exit(1);
        }
        it_val.it_value.tv_sec = seconds;
        it_val.it_value.tv_usec = seconds % 1000000;
        it_val.it_interval = it_val.it_value;
        if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
        }
        while (ALARM_STATUS == false) {
        }
        ALARM_STATUS = false;
}


int main (int argc, char* argv[]) {
	
	int run_seconds = 30;
	if (argc == 2) {
		int temp = atoi(argv[1]);
		if (temp == 0) {
			log_event(WARNING,"Invalid arg provided, defaulting to 30 seconds");
		} else {
		run_seconds = temp;
		}
	} else {
		log_event(WARNING,"No arg provided, defaulting to 30 second runtime");
	} 
	hw_data_struct * shm_array = (hw_data_struct *) connect_shm(SEGMENT_KEY,SEGMENT_SIZE);
	//std::cout << sizeof(shm_array[20]) << std::endl;	
	for (int i = 0; i < run_seconds; i++) {
		if (i != 0){
			my_sleep(1);
		}
		int active_count = 0;
		double x_sum = 0;
		double y_sum = 0;
		for (int j = 0; j < 20; j++) {
			hw_data_struct temp =  shm_array[j];
			active_count += temp.is_valid;
			x_sum += temp.x;
			y_sum += temp.y; 
		}
		if (active_count > 0) {
			std::cout << "seconds elapsed: " << i << ", x avg: " << x_sum/active_count << ", y avg: " << y_sum/active_count << ", active segments: " << active_count <<std::endl;
		} else {
		std::cout << "seconds elapsed: " << i <<  ", no segments acticve!" <<std::endl;
		}
	}
	detach_shm((void *) shm_array);
}  
