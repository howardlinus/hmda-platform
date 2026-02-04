# Makefile for Prepayment Library and Example

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -I./include

# Check if Eigen is available
ifdef HAVE_EIGEN
  CXXFLAGS += -DHAVE_EIGEN -I./vendor/eigen
  $(info Building with Eigen support)
endif

# Linker flags
LDFLAGS = -lm

# Directories
SRC_DIR = src
UTILS_SRC_DIR = $(SRC_DIR)/utils
INCLUDE_DIR = include
BIN_DIR = bin
BUILD_DIR = build

# Library name
LIBRARY = libprepayment.a

# Source files
UTILS_SRCS = $(UTILS_SRC_DIR)/Benchmark.cpp \
             $(UTILS_SRC_DIR)/CPR.cpp \
             $(UTILS_SRC_DIR)/CSV.cpp \
             $(UTILS_SRC_DIR)/TransitionMatrix.cpp \
             $(UTILS_SRC_DIR)/MatrixFractional.cpp

CRE_SRCS = $(SRC_DIR)/ARIMAX.cpp \
           $(SRC_DIR)/RegressionModel.cpp \
           $(SRC_DIR)/FeatureBuilder.cpp \
           $(SRC_DIR)/Mortgage.cpp

LIB_SRCS = $(UTILS_SRCS) $(CRE_SRCS)

# Object files
UTILS_OBJS = $(patsubst $(UTILS_SRC_DIR)/%.cpp,$(BUILD_DIR)/utils/%.o,$(UTILS_SRCS))
CRE_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CRE_SRCS))
LIB_OBJS = $(UTILS_OBJS) $(CRE_OBJS)

# Example
EXAMPLE_SRC = examples/run_arimax_infer.cpp
EXAMPLE_BIN = $(BIN_DIR)/run_arimax_infer
EXAMPLE_CXXFLAGS = $(CXXFLAGS) -I./vendor/nlohmann

# Default target
.PHONY: all
all: lib example

# Build library
.PHONY: lib
lib: $(LIBRARY)

$(LIBRARY): $(LIB_OBJS)
	@echo "Creating static library $@"
	ar rcs $@ $^

# Build utils objects
$(BUILD_DIR)/utils/%.o: $(UTILS_SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)/utils
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build CRE objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build example
.PHONY: example
example: $(EXAMPLE_BIN)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIBRARY)
	@mkdir -p $(BIN_DIR)
	@echo "Building example $@"
	$(CXX) $(EXAMPLE_CXXFLAGS) $< -L. -lprepayment $(LDFLAGS) -o $@

# Run example
.PHONY: run
run: $(EXAMPLE_BIN)
	@echo "Running example..."
	$(EXAMPLE_BIN) test_data/logistic_model.json test_data/model_params.json test_data/exog_future.json test_data/transition_matrix.csv

# Clean
.PHONY: clean
clean:
	@echo "Cleaning build artifacts"
	rm -rf $(BUILD_DIR) $(LIBRARY) $(BIN_DIR)

# Help
.PHONY: help
help:
	@echo "Prepayment Library Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build library and example (default)"
	@echo "  lib          - Build static library (libprepayment.a)"
	@echo "  example      - Build example binary (bin/run_arimax_infer)"
	@echo "  run          - Run the example"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Options:"
	@echo "  HAVE_EIGEN=1 - Build with Eigen support for matrix operations"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build everything"
	@echo "  make HAVE_EIGEN=1       # Build with Eigen support"
	@echo "  make lib                # Build only the library"
	@echo "  make example            # Build only the example"
	@echo "  make run                # Build and run the example"
	@echo "  make clean              # Clean build artifacts"

# Declare phony targets
.PHONY: all lib example run clean help
