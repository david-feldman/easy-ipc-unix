/*
 * * Project: Project 4
 * *
 * * Program: random_fortune
 * * File Name: random_fortune.cpp
 * *
 * * Description: Random fortune program
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
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <csignal>
#include <stdio.h>
#include <fcntl.h>   
#include <unistd.h>
#include <sys/utsname.h>
#include "thread_mgr.h"
#include "log_mgr.h"

volatile std::sig_atomic_t ALARM_STATUS = false;
const int BUFFER_STR_LEN = 512;
const int BIG_BUFFER = 500000;
const std::string PIPE_PROGRAM_ABS  = "/home/jcn/unix_class/fortune_absaroka/fortune";
const std::string PIPE_PROGRAM_DEV  = "/home/jcn/unix_class/fortune/fortune"; 

void alarm_handler(int signal) {
	ALARM_STATUS = true;
	//std::cout << "signal" <<std::endl;
}

void to_upper_and_print(std::string lowercase) {
	char * char_result = new char [lowercase.length()+1];
	strcpy (char_result, lowercase.c_str());
	for (int i=0; i<strlen(char_result); i++)
		putchar(toupper(char_result[i]));
}

void my_sleep (int seconds) { 
	//std::cout << "sleeping for " << seconds << " seconds." << std::endl; 
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

int get_text_from_pipe(std::string pipe_program, std::string& result) {
	char buffer[BUFFER_STR_LEN];
	FILE* pipe = popen(pipe_program.c_str(), "r");
	if (!pipe) {
		perror("pipe_program failed to open.");
		return -1;
	}
	while (fgets(buffer, BUFFER_STR_LEN, pipe) != NULL) {
		result += buffer;
	}
	return pclose(pipe);
}

void * multiplex_from_stdin_and_look_for_q (void *ptr) {
	for (;;) {
		int result;
		char buf[BIG_BUFFER];
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(STDIN_FILENO, &readset);
		result = select(1, &readset, NULL, NULL, NULL);
		if (result > 0) {
			read(STDIN_FILENO, buf, sizeof(buf));
			if (buf[0] == 'q') {
				exit(0);
			} 
		}
	}
} 

int main() {
	thread *curr_thread = new thread();
	if (pthread_create(&curr_thread->id, NULL, multiplex_from_stdin_and_look_for_q, (void*) (curr_thread))) {
                perror("pthread_create");
                log_event(WARNING, "Could not create thread");
        }
	std::string pipe_program;
	struct utsname sysinfo;
	uname(&sysinfo);
	std::string absaroka = "absaroka"; 
	if (absaroka.compare(sysinfo.nodename) == 0) {
		pipe_program = PIPE_PROGRAM_ABS;
	} else {
		pipe_program = PIPE_PROGRAM_DEV;
	}
	srand(time(NULL));
	int ctr = 0;
	int rand_wait;
	while (true) {
		if (ctr != 0) {
			rand_wait = rand() % 8 + 1;
			my_sleep(rand_wait);	
		}
		std::string result;
		get_text_from_pipe(pipe_program,result);
		to_upper_and_print(result);
		ctr++;
	}
	return 0;
}
