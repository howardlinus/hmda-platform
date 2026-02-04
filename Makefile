# Makefile for prepayment library and example

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I./include -I./vendor
AR = ar
ARFLAGS = rcs

# Source files
UTIL_SOURCES = src/utils/Benchmark.cpp src/utils/CPR.cpp src/utils/CSV.cpp \
               src/utils/TransitionMatrix.cpp src/utils/MatrixFractional.cpp
CRE_SOURCES = src/ARIMAX.cpp src/RegressionModel.cpp src/FeatureBuilder.cpp src/Mortgage.cpp

LIB_SOURCES = $(UTIL_SOURCES) $(CRE_SOURCES)
LIB_OBJECTS = $(LIB_SOURCES:.cpp=.o)

# Library
LIBRARY = libprepayment.a

# Example
EXAMPLE_SOURCE = examples/run_arimax_infer.cpp
EXAMPLE = examples/run_arimax_infer

# Default target
all: $(LIBRARY) $(EXAMPLE)

# Build library
$(LIBRARY): $(LIB_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Built static library: $(LIBRARY)"

# Build example
$(EXAMPLE): $(EXAMPLE_SOURCE) $(LIBRARY)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBRARY)
	@echo "Built example: $(EXAMPLE)"

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(LIB_OBJECTS) $(LIBRARY) $(EXAMPLE)
	@echo "Cleaned build artifacts"

# Phony targets
.PHONY: all clean
