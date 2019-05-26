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
#include <fstream>
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
char ** ArgVDump;
std::ifstream Infile;
int RunSeconds = 30;
hw_data_struct * DataRa; 
ThreadHandles InstallThread;
ThreadHandles MonitorThread;

void * install_data(void * ptr);

void signal_handler(int signum) {
	if (signum == SIGHUP) {
		log_event(INFO,"SIGHUP caught, restarting install");
		th_kill(InstallThread);
		InstallThread = th_execute(install_data);
	}
	if (signum == SIGINT) {
		log_event(INFO,"SIGINT caught, terminating");
		exit(0);
	}
	if (signum == SIGTERM) {
		log_event(INFO,"SIGTERM caught, terminating");
		exit(0);
	}
}

void my_sleep (int seconds) {
	log_event(INFO, "Waiting for %d seconds", seconds);
	sleep(seconds);
}



void * install_data(void * ptr) {
	std::string line;
	hw_data_struct *temp = new hw_data_struct(false, 0, 0);
	for (int i = 0; i < 20; i++) {
		DataRa[i] = *temp;
	}
	while (std::getline(Infile, line)) {
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
				DataRa[index] = *temp;
				log_event(INFO,"Inserted: %f, %f", x_value, y_value);
			} else if (time_increment == 0) {
				hw_data_struct *temp = new hw_data_struct(true,x_value,y_value);
				DataRa[index] = *temp;
				log_event(INFO,"Inserted: %f, %f", x_value, y_value);
			} else {
				my_sleep(abs(time_increment));
				DataRa[index].is_valid = 0;
				log_event(INFO,"Waited: %d seconds and then set object at index %d to invalid state", abs(time_increment), index);
			}
		}

	}
	Infile.close();
} 

void * monitor_data(void * ptr) {
	for (int i = 0; i < RunSeconds; i++) {
		if (i != 0){
			my_sleep(1);
		}
		int active_count = 0;
		double x_sum = 0;
		double y_sum = 0;
		for (int j = 0; j < 20; j++) {
			hw_data_struct temp =  DataRa[j];
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
}

int main (int argc, char* argv[]) {
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	ArgVDump = argv; 
	if(argc != 3 and argc != 2) {
		log_event(FATAL,"Incorrect number of arguments provided by user");
		std::cout << "Usage: " << argv[0] << " [MANDATORY INPUT FILE TO READ DATA FROM] [OPTIONAL NUMBER OF SECONDS TO MONITOR DATA]" << std::endl;
		return ERROR;
	} else if (argc == 2) {
		Infile.open(ArgVDump[1]);
		if (Infile.fail()) {
			std::cout << "Invalid file name supplied." << std::endl;
			log_event(FATAL,"Invalid file name supplied.");
			return ERROR;
		}
	} else {
		int temp = atoi(argv[2]);
		if (temp == 0) {
			log_event(WARNING,"Invalid arg provided, defaulting to 30 seconds");
		} else {
			RunSeconds = temp;
		}
	}
	DataRa = (hw_data_struct *) malloc (sizeof(hw_data_struct)*20);
	InstallThread = th_execute(install_data);
	MonitorThread = th_execute(monitor_data);
	th_wait_all();
 	free(DataRa);
	return EXIT_SUCCESS;
}

