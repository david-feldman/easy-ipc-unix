/*
* Project: Project 2
*
* Program: conv_to_fahr
* File Name: conv_to_fahr.cpp
*
* Purpose: simple program that prompts the user for a number of 
* Centigrade degrees and prints out the corresponding number of Fahrenheit degrees
*
* Synopsis (Usage and Parameters):
* Main method for centigrade -> conversion 
* Usage: following execution of compiled program, user will be prompted to enter degrees 
* in centegrade into their command prompt. the program will convert this to the fahrenheight scale and output results. 
*
* Programmer: David Feldman
* Organization: JHU Part-Time Programs in Engineering and Applied Science
* Host System: UNIX 4.3 BSD or Sun OS 4.x
* Language: C++
* Date Created: 02/18/2019
* Modifications:
* $Log$
*/
#ifndef _LOGMGRH
#define _LOGMGRH

#define DEFAULT_LOGFILE_LOCATION (char *)"logfile"

typedef enum {INFO, WARNING, FATAL} Levels;

int log_event (Levels l, const char *fmt, ...);

const char* get_levels_name(Levels l);

int set_logfile (const char *logfile_name);

void close_logfile (void);

#endif //_LOGMGRH