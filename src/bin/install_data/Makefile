SRCS=install_data.cpp
OBJS=install_data.o
PROGRAM=install_data
PROJ_ROOT=../../../
LIBDIR=$(PROJ_ROOT)/lib
INSTALLDIR=$(PROJ_ROOT)/bin
INCLUDEDIR=$(PROJ_ROOT)/include
LIBS=${LIBDIR}/lib_log_mgr.a ${LIBDIR}/lib_thread_mgr.a ${LIBDIR}/lib_shmlib.a
LLIBS=-l_log_mgr -l_thread_mgr -l_shmlib

it : ${PROGRAM}

${PROGRAM}: ${OBJS} ${LIBS}
	g++  -pthread -Wall ${OBJS} -L${LIBDIR} ${LLIBS} -o ${PROGRAM}

.cpp.o:
	g++ -I${INCLUDEDIR} -c $<

depend dependlist:
	g++  -Wall -I ${INCLUDEDIR} -M ${SRCS} > dependlist

install: ${INSTALLDIR}/${PROGRAM}

${INSTALLDIR}/${PROGRAM}:${PROGRAM}
	cp ${PROGRAM} ${INSTALLDIR}/${PROGRAM}

clean:
	rm -f *.o ${PROGRAM} ${INSTALLDIR}/${PROGRAM} dependlist        

-include dependlist
