#David Feldman
#Organization: JHU Part-Time Programs in Engineering and Applied Science
#Host System: UNIX 4.3 BSD or Sun OS 4.x
#Language: Makefile
#Date Created: 02/18/2019
# Adapted from the example provided in class lecture notes

SUBS = log_mgr thread_mgr shmlib

it install clean depend:
	-@ X=`pwd`; \
        for i in $(SUBS); \
        do echo '<<<' $$i '>>>'; cd $$X/$$i; make $@; done
