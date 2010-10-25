PROGTARGET  :=vim3d
CC          := g++

# source and headers
SRCFILES    := $(wildcard *.cpp) $(wildcard primitives/*.cpp)
HEADERS     := $(wildcard *.h)

# object files
OBJDIR      := obj
OBJFILES    := $(SRCFILES:%.cpp=$(OBJDIR)/%.o)

# are we on a macintosh?
UNAME       := $(shell uname -s)
ifeq ($(UNAME), Darwin)
CFLAGS      := -framework GLUT -framework OpenGL -lpthread
else
CFLAGS      := -lGL -lGLU -lglut -lpthread
endif

$(PROGTARGET): $(OBJFILES)
	$(CC) $^ $(CFLAGS) -o $(PROGTARGET)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	$(CC) -Wall -c -o $@ $<

db: $(OBJFILES)
	$(CC) $^ -ggdb3 $(CFLAGS) -o $(PROGTARGET)
	gdb -x gdbCommands

debug: 
	$(CC) *.cpp primitives/*.cpp -Wall -ggdb3 $(CFLAGS) -o $(PROGTARGET)

// make search FUNC=normalize
search:
	grep ${FUNC} *.cpp
	grep ${FUNC} *.h

lines:
	wc *.cpp *.h

clean:
	@rm obj/*.o obj/primitives/*.o
