SRCS=thread_mgr.cpp
OBJS=thread_mgr.o
LIB=lib_thread_mgr.a
PROJ_ROOT=../../..
LIBDIR=$(PROJ_ROOT)/lib
INSTALLDIR=$(PROJ_ROOT)/bin
INCLUDEDIR=$(PROJ_ROOT)/include


.cpp.o:
	g++ -I ${INCLUDEDIR} -c $<

it: ${LIB}


${LIB}:${OBJS}
	ar rvs ${LIB} ${OBJS}

depend dependlist:
	g++ -I ${INCLUDEDIR} -M ${SRCS} > dependlist

install: ${LIBDIR}/${LIB}

${LIBDIR}/${LIB}: ${LIB}
	cp ${LIB}  ${LIBDIR}/${LIB}

clean:
	rm -f ${OBJS} ${LIB} ${INSTALLDIR}/${LIB} dependlist

include dependlist
