CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g -w
EXEC = vim
SRCDIR = src
OBJECTS = ${addprefix ${SRCDIR}/, vmmain.o controller/Controller.o controller/Keyboard.o editorComponent/CommandLine.o editorComponent/Cursor.o editorComponent/StatusLine.o editorComponent/Text.o editorComponent/VM.o view/View.o view/PlainView.o model/Model.o}
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}
