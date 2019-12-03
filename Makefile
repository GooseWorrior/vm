CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g -w
EXEC = vm
SRCDIR = src
OBJECTS = ${addprefix ${SRCDIR}/, Model.o vmmain.o controller/Controller.o controller/Keyboard.o editorComponent/Cursor.o editorComponent/StatusLine.o editorComponent/EditorComponent.o editorComponent/VM.o view/View.o view/SyntaxView.o view/PlainView.o}
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -lncurses -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}
