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
CFLAGS += -Ilib/ei-cpp-sdk
CFLAGS += -Ilib/fast-cpp-csv-parser
CFLAGS += -Ilib/imu-emulator
CFLAGS += -Ilib/time-emulator

# C and C++ Compiler flags
CFLAGS += -Wall						# Include all warnings
CFLAGS += -g						# Generate GDB debugger information
CFLAGS += -Wno-strict-aliasing		# Disable warnings about strict aliasing
CFLAGS += -Os						# Optimize for size
CFLAGS += -DNDEBUG					# Disable assert() macro
CFLAGS += -DEI_CLASSIFIER_ENABLE_DETECTION_POSTPROCESS_OP	# Add TFLite_Detection_PostProcess operation

# C++ only compiler flags
CXXFLAGS += -std=c++14				# Use C++14 standard

# Linker flags
LDFLAGS += -lm 						# Link to math.h
LDFLAGS += -lstdc++					# Link to stdc++.h
LDFLAGS += -lpthread				# Link to pthread.h

# Include C source code for required libraries
CSOURCES += $(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions/*.c) \
			$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions/*.c)

# Include C++ source code for required libraries
CXXSOURCES += 	$(wildcard lib/ei-cpp-sdk/tflite-model/*.cpp) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/dsp/kissfft/*.cpp) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/dsp/dct/*.cpp) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/dsp/memory.cpp) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/porting/posix/*.c*) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/porting/mingw32/*.c*)
CXXSOURCES +=	

# Use TensorFlow Lite for Microcontrollers (TFLM)
CFLAGS += -DTF_LITE_DISABLE_X86_NEON=1
CSOURCES +=	lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/c/common.c
CCSOURCES +=	$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/kernels/*.cc) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/micro/*.cc) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/tensorflow/lite/core/api/*.cc)

# Include CMSIS-NN if compiling for an Arm target that supports it
ifeq (${CMSIS_NN}, 1)

	# Include CMSIS-NN and CMSIS-DSP header files
	CFLAGS += -Iedge-impulse-sdk/CMSIS/NN/Include/
	CFLAGS += -Iedge-impulse-sdk/CMSIS/DSP/PrivateInclude/

	# C and C++ compiler flags for CMSIS-NN and CMSIS-DSP
	CFLAGS += -Wno-unknown-attributes 					# Disable warnings about unknown attributes
	CFLAGS += -DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1	# Use CMSIS-NN functions in the SDK
	CFLAGS += -D__ARM_FEATURE_DSP=1 					# Enable CMSIS-DSP optimized features
	CFLAGS += -D__GNUC_PYTHON__=1						# Enable CMSIS-DSP intrisics (non-C features)

	# Include C source code for required CMSIS libraries
	CSOURCES += $(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions/*.c) \
				$(wildcard lib/ei-cpp-sdk/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions/*.c)
endif

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