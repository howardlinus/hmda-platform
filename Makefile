# Makefile for HMDA Prepayment Library

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include
AR = ar
ARFLAGS = rcs

# Source files
UTILS_SRCS = $(wildcard src/utils/*.cpp)
CRE_SRCS = $(wildcard src/*.cpp)
ALL_SRCS = $(UTILS_SRCS) $(CRE_SRCS)

# Object files
UTILS_OBJS = $(UTILS_SRCS:.cpp=.o)
CRE_OBJS = $(CRE_SRCS:.cpp=.o)
ALL_OBJS = $(ALL_SRCS:.cpp=.o)

# Library
LIB = libprepayment.a

# Example
EXAMPLE_SRC = examples/run_arimax_infer.cpp
EXAMPLE_BIN = examples/run_arimax_infer

# Check for vendor dependencies
VENDOR_JSON = vendor/nlohmann/json.hpp
VENDOR_EIGEN = vendor/eigen/Eigen

# Default target
all: lib

# Library target
lib: $(LIB)

$(LIB): $(ALL_OBJS)
	@echo "Building static library: $(LIB)"
	$(AR) $(ARFLAGS) $@ $^
	@echo "✓ Library built successfully"

# Object file compilation
%.o: %.cpp
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Example target (requires vendor dependencies)
example: check-vendor-json $(LIB)
	@echo "Building example: $(EXAMPLE_BIN)"
	$(CXX) $(CXXFLAGS) -DHAVE_JSON -I./vendor $(EXAMPLE_SRC) -o $(EXAMPLE_BIN) -L. -lprepayment
	@echo "✓ Example built successfully"
	@echo ""
	@echo "Running example..."
	@cd . && $(EXAMPLE_BIN)

# Check for vendor dependencies
check-vendor-json:
	@if [ ! -f "$(VENDOR_JSON)" ]; then \
		echo "Error: nlohmann/json not found."; \
		echo "Please run: ./scripts/fetch_vendors.sh"; \
		exit 1; \
	fi

# Check for optional Eigen
check-eigen:
	@if [ -d "$(VENDOR_EIGEN)" ]; then \
		echo "✓ Eigen found - enabling matrix fractional power support"; \
	else \
		echo "⚠ Eigen not found - using fallback for matrix fractional power"; \
	fi

# Clean target
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(ALL_OBJS) $(LIB) $(EXAMPLE_BIN)
	@echo "✓ Clean complete"

# Help target
help:
	@echo "HMDA Prepayment Library - Build Targets"
	@echo "========================================"
	@echo ""
	@echo "  make all       - Build library (default)"
	@echo "  make lib       - Build static library (libprepayment.a)"
	@echo "  make example   - Build and run example (requires vendor deps)"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make help      - Show this help message"
	@echo ""
	@echo "Setup:"
	@echo "  ./scripts/fetch_vendors.sh  - Fetch vendor dependencies"
	@echo ""

.PHONY: all lib example clean help check-vendor-json check-eigen
