# Tool macros
CC ?= gcc
CXX ?= g++

# Settings
NAME = app
BUILD_PATH = ./build

# Figure out which OS we're using
ifeq ($(OS), Windows_NT)
	UNAME := Windows
else
	UNAME := $(shell uname 2>/dev/null || echo Unknown)
endif
$(info OS: $(UNAME))

# Location of main.cpp (must use C++ compiler for main) and submission
CXXSOURCES = source/main.cpp source/submission.cpp

# Search path for header files (lib/ directory)
CFLAGS += -Ilib/fast-cpp-csv-parser
CFLAGS += -Ilib/imu-emulator
CFLAGS += -Ilib/time-emulator
CFLAGS += -Ilib/print-emulator

# C and C++ Compiler flags
CFLAGS += -Wall						# Include all warnings
CFLAGS += -g						# Generate GDB debugger information
CFLAGS += -Wno-strict-aliasing		# Disable warnings about strict aliasing
CFLAGS += -Os						# Optimize for size
CFLAGS += -DNDEBUG					# Disable assert() macro

# C++ only compiler flags
CXXFLAGS += -std=c++14				# Use C++14 standard

# Linker flags
LDFLAGS += -lm 						# Link to math.h
LDFLAGS += -lstdc++					# Link to stdc++.h
LDFLAGS += -lpthread				# Link to pthread.h

# Include C source code for required libraries
CSOURCES +=		

# Include C++ source code for required libraries
CXXSOURCES +=	$(wildcard lib/imu-emulator/*.c*) \
				$(wildcard lib/time-emulator/*.c*) \
				$(wildcard lib/print-emulator/*.c*)

# Generate names for the output object files (*.o)
COBJECTS := $(patsubst %.c,%.o,$(CSOURCES))
CXXOBJECTS := $(patsubst %.cpp,%.o,$(CXXSOURCES))
CCOBJECTS := $(patsubst %.cc,%.o,$(CCSOURCES))

# Default rule
.PHONY: all
all: app

# Compile library source code into object files
$(COBJECTS) : %.o : %.c
$(CXXOBJECTS) : %.o : %.cpp
$(CCOBJECTS) : %.o : %.cc
%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
%.o: %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $^ -o $@
%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $^ -o $@

# Build target (must use C++ compiler)
.PHONY: app
app: $(COBJECTS) $(CXXOBJECTS) $(CCOBJECTS)
ifeq ($(UNAME), Windows)
	if not exist build mkdir build
else
	mkdir -p $(BUILD_PATH)
endif
	$(CXX) $(COBJECTS) $(CXXOBJECTS) $(CCOBJECTS) -o $(BUILD_PATH)/$(NAME) $(LDFLAGS)

# Remove compiled object files
.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del /Q $(subst /,\,$(patsubst %.c,%.o,$(CSOURCES))) >nul 2>&1 || exit 0
	del /Q $(subst /,\,$(patsubst %.cpp,%.o,$(CXXSOURCES))) >nul 2>&1 || exit 0
	del /Q $(subst /,\,$(patsubst %.cc,%.o,$(CCSOURCES))) >nul 2>&1 || exit 0
else
	rm -f $(COBJECTS)
	rm -f $(CCOBJECTS)
	rm -f $(CXXOBJECTS)
endif