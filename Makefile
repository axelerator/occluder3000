TARGET    = pmrrt

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDES = $(SOURCES:.cpp=.h)

CC = g++
LD = g++

ifeq ($(shell uname -m),x86_64)
	ARCH = athlon64
endif

ifeq ($(shell uname -m),i686)
	ARCH = pentium3
endif

OPTIMIZATION = -O2 -march=$(ARCH) -falign-functions=4 -ffast-math -fforce-addr -fprefetch-loop-arrays -funroll-loops -fomit-frame-pointer

CPPFLAGS = -I./
ifeq ($(ENVIR),DEBUG)
	CFLAGS = -ggdb -Wall -Wextra -Wno-unused-parameter -ansi -pedantic
else
CFLAGS = $(OPTIMIZATION) -Wall -Wextra -Werror -Wno-unused-parameter -ansi -pedantic
endif
#CFLAGS = -ggdb -Wall -Wextra -Werror -Wno-unused-parameter -ansi -pedantic


# Linker flags
LDFLAGS_LINUX    = -L/usr/X11R6/lib 
LDFLAGS_LINUX64  = -L/usr/X11R6/lib64
LDFLAGS_COMMON   = 
LDFLAGS          = $(LDFLAGS_COMMON) $(LDFLAGS_$(OS))

# Linker libraries
XLIBS            = -lXmu -lXi -lX11
GLLIBS           = -lGLEW -lGLU -lGL -lSDL
#LDLIBS           = $(GLLIBS) $(XLIBS) -lm -lboost_regex -lboost_thread -lSDL_ttf
LDLIBS           = $(GLLIBS) -lm -lboost_regex -lboost_thread -lSDL_ttf
ifeq ($(findstring Linux,$(shell uname -s)),Linux)
  OS = LINUX
  ifeq ($(shell uname -m),x86_64)
    OS = LINUX64
  endif
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $*.o $*.cpp

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
