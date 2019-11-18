CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g -w
EXEC = vim
SRCDIR = src
OBJECTS = ${addprefix ${SRCDIR}/, vmmain.o}
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}
