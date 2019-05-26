/*
 * * Project: Project 4
 * *
 * * Program: Install Data
 * * File Name: install_data.cpp
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
char ** ArgVDump;

using namespace std;

int install_data();

void alarm_handler(int signal) {
	ALARM_STATUS = true;
	//std::cout << "signal" <<std::endl;
}

void signal_handler(int signum) {
	if (signum == SIGHUP) {
		log_event(INFO,"SIGHUP caught, restarting install");
		destroy_shm(SEGMENT_KEY);
		exit(install_data());
	} 
	if (signum == SIGINT) {
		log_event(INFO,"SIGINT caught, terminating");
		destroy_shm(SEGMENT_KEY);
		exit(0);
	} 
	if (signum == SIGTERM) {
		log_event(INFO,"SIGTERM caught, terminating");
		destroy_shm(SEGMENT_KEY);
		exit(0);
	}
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

int install_data() {
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	string line;
	ifstream infile;
	infile.open(ArgVDump[1]);
	if (infile.fail()) {
		cout << "Invalid file name supplied." << endl;
		log_event(FATAL,"Invalid file name supplied.");
		return ERROR;
	}
	hw_data_struct * shm_array = (hw_data_struct *) connect_shm(SEGMENT_KEY,SEGMENT_SIZE);
	hw_data_struct *temp = new hw_data_struct(false, 0, 0);
	for (int i = 0; i < 20; i++) {
		shm_array[i] = *temp;
	}
	while (std::getline(infile, line)) {
		std::istringstream iss(line);
		int index, time_increment;
		float x_value, y_value;
		if (!(iss >> index >> x_value >> y_value >> time_increment) or index > 19 ) {
			log_event(WARNING,"Invalid line: %s",line.c_str());
			break;
		} else {
			if (time_increment > 0) {
				my_sleep(abs(time_increment));
				hw_data_struct *temp = new hw_data_struct(true,x_value,y_value);
				shm_array[index] = *temp;
				log_event(INFO,"Inserted: %f, %f", x_value, y_value);
			} else if (time_increment == 0) {
				hw_data_struct *temp = new hw_data_struct(true,x_value,y_value);
				shm_array[index] = *temp;
				log_event(INFO,"Inserted: %f, %f", x_value, y_value);
			} else {
				my_sleep(abs(time_increment));
				shm_array[index].is_valid = 0;
				log_event(INFO,"Waited: %d seconds and then set object at index %d to invalid state", abs(time_increment), index);
			}
		}

	}

	infile.close();
	if (destroy_shm(SEGMENT_KEY) != 0)
		return ERROR;
	return OK;
}  

int main (int argc, char* argv[]) {
	if(argc != 2) {
		log_event(FATAL,"No args provided by user.");
		cout << "You need to supply one argument to this program." << endl;
		return ERROR;
	}
	ArgVDump = argv; 
	return install_data(); 	
}
