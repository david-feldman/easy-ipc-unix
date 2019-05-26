#!/usr/bin/python

'''
* Project: Project 3
*
* Program: release python script
* File Name: release.py
*
* Purpose: package releases of homework 4
*
* Synopsis (Usage and Parameters):
* use either a -b or -s flag (arguement)  when running, and follow instructions to create releases
* Programmer: David Feldman
* Organization: JHU Part-Time Programs in Engineering and Applied Science
* Host System: UNIX 4.3 BSD or Sun OS 4.x
* Language: Python 2.X
* Date Created: 04/21/2019
* Modifications:
* $Log$
'''


import tarfile
import argparse
import sys
import os

parser = argparse.ArgumentParser()
parser.add_argument('-b', action='store_true')
parser.add_argument('-s', action='store_true')
args = parser.parse_args()

if not len(sys.argv) > 1 or (args.b == False and args.s == False):
	print("USAGE: Pass `-b` as an arguement for binary release, pass `-s` as an arguement for source release.\n       No other flags currently supported.")
if args.b == True:
	question_not_answered = True
	proceed = False
	while question_not_answered:
		print "You have requested a binary release via the `-b` flag. Please confirm [Y/N]"  
		response = sys.stdin.readline().rstrip()
		if response == 'N':
			question_not_answered = False
		if response == 'Y':
			question_not_answered = False
			proceed = True 
	if proceed  == True:
		print "Please enter a hostname for binary release."
		hostname = sys.stdin.readline().rstrip()
		os.system("make clean > /dev/null 2> /dev/null ")
		os.system("make install > /dev/null 2> /dev/null")
		os.system("mkdir tmp > /dev/null 2> /dev/null")
		os.system("cp -r bin tmp/  > /dev/null 2> /dev/null")
		output_tar_name = "homework4_" + hostname + ".tar" 
		with tarfile.open(output_tar_name, "w") as tar:
			tar.add("tmp","homework4_" + hostname)
		os.system("rm -rf tmp > /dev/null 2> /dev/null")
		print output_tar_name + " sucessfully generated."

if args.s == True:	 
	question_not_answered = True
        proceed = False
        while question_not_answered:
                print "You have requested a source release via the `-s` flag. Please confirm [Y/N]"
                response = sys.stdin.readline().rstrip()
                if response == 'N':
                        question_not_answered = False
                if response == 'Y':
                        question_not_answered = False
                        proceed = True
        if proceed == True:
		os.system("make clean > /dev/null 2> /dev/null ")
		os.system("mkdir tmp > /dev/null 2> /dev/null")
		os.system("cp -r ./ tmp/ > /dev/null 2> /dev/null")
		os.system("rm -rf tmp/*.tar tmp/tmp > /dev/null 2> /dev/null")
		
		output_tar_name = "homework4" + ".tar"	
		with tarfile.open(output_tar_name, "w") as tar:
                        tar.add("tmp","homework4")
		os.system("rm -rf tmp > /dev/null 2> /dev/null")
		print output_tar_name + " sucessfully generated."

