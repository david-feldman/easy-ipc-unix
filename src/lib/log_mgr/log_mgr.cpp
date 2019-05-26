/*
* Project: Project 2
*
* Program: log_mgr
* File Name: log_mgr.cpp
*
* Purpose: Package for managing error and event logging 
*
* Synopsis (Usage and Parameters):
* See .h file for available functions
*
* Programmer: David Feldman
* Organization: JHU Part-Time Programs in Engineering and Applied Science
* Host System: UNIX 4.3 BSD or Sun OS 4.x
* Language: C++
* Date Created: 03/04/2019
* Modifications:
* $Log$
*/

#include <iostream> /* including to print to stdout / read from stdin */
#include <stdio.h> // c file io
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include <pthread.h>
#include <cerrno>
#include <sys/stat.h>
#include <sstream>  // for string streams 
#include <iomanip>  // for formatting output
#include <ctime> //for datetime
#include <stdarg.h>// to deal with variadic fcn
#include "log_mgr.h" 
#include <sys/file.h>
#include <sys/fcntl.h>
#include <unistd.h>

using namespace std;

static const int ERROR = -1;
static const int OK = 0;

const int BUFFER_STR_LEN = 512;
char ModifiedLogFile[BUFFER_STR_LEN];
int Fd = -1;  

int log_event (Levels l, const char *fmt, ...) {

	char buffer[BUFFER_STR_LEN];
	char * logfile;

	//set open logifle if necessary
	if (!(strlen(ModifiedLogFile) > 0) and Fd < 0) {
		set_logfile(DEFAULT_LOGFILE_LOCATION);
	} else if (Fd < 0) {
		set_logfile(ModifiedLogFile);
	}

	//always seek to end
	lseek (Fd, 0, SEEK_END);

	time_t _tm =time(NULL);
	struct tm * curtime = localtime(&_tm);
	string output(asctime(curtime));
	output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());
	sprintf (buffer, "%s:%s:", output.c_str(), get_levels_name(l));
	va_list args;
	va_start(args, fmt);
	vsnprintf (buffer + strlen(buffer), BUFFER_STR_LEN - 1 - strlen(buffer),fmt, args);
	va_end (args);
	sprintf(buffer + strlen(buffer), "\n");
	write (Fd, buffer, strlen(buffer));
	return OK; 
}

int set_logfile (const char *logfile_name) {
	//close old before opening new
	if (Fd >= 0) {
		close_logfile();
	}
	// open Fd
    if ((Fd = open (logfile_name, O_CREAT | O_WRONLY | O_APPEND, 0600)) < 0) {
    		perror("open file descriptor");
			return ERROR; 
    }
    
	sprintf(ModifiedLogFile, "%s", logfile_name);
	return OK; 
}

void close_logfile (void) {
	if (Fd >= 0) {
		close(Fd);
		Fd = -1;
	} else {
		perror("close_logfile");
	}
}

const char* get_levels_name(Levels l) {

	if (l == INFO) {
		return("INFO");
	} else if (l == WARNING) {
		return("WARNING");
	} else if (l == FATAL) {
		return("FATAL");
	} else {
		return("Invalid logging level.");
	}
} 

//int set_logfile (const char *logfile_name);

//void close_logfile (void);
