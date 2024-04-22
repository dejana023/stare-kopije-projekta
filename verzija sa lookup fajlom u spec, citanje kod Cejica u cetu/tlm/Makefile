# SystemC installation directory
SYSTEMC_HOME=/usr/local/systemc

# Loads library
LIBS = -lm 'pkg-config --libs opencv4' -lsystemc
GET = get

# Compiler to use
CC=g++

# Compiler flags
CFLAGS=-c -I$(SYSTEMC_HOME)/include -Wall -O0 -march=x86-64 -mmmx -msse -msse2 -msse3 `pkg-config --cflags opencv4`

# Linker flags
LDFLAGS=-L$(SYSTEMC_HOME)/lib-linux64 -lsystemc

# Source files
SOURCES=main.cpp memory.cpp cpu.cpp interconnect.cpp ip.cpp ../spec/fasthessian.cpp ../spec/image.cpp ../spec/imload.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE=main

# Default target
all: $(SOURCES) $(EXECUTABLE)

# Build target
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Compile target
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
