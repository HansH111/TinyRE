# TinyRE Makefile
# Minimalist Regular Expression Engine in C

CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
LDFLAGS ?=
AR ?= ar
RANLIB ?= ranlib

# Directories
SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

# Source files
TRE_SRC = $(SRC_DIR)/tre.c
TRE_OBJ = $(BUILD_DIR)/tre.o
TEST_SRC = $(SRC_DIR)/test_tre.c
TEST_OBJ = $(BUILD_DIR)/test_tre.o
ERROR_SRC = $(SRC_DIR)/test_error.c
ERROR_OBJ = $(BUILD_DIR)/test_error.o

# Targets
LIB = $(BUILD_DIR)/libtre.a
TEST_TARGET = $(BUILD_DIR)/test_tre
ERROR_TARGET = $(BUILD_DIR)/test_error

# Default target
all: $(LIB) $(TEST_TARGET) $(ERROR_TARGET)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build TinyRE library
$(LIB): $(TRE_OBJ) | $(BUILD_DIR)
	$(AR) rcs $@ $^
	$(RANLIB) $@

# Compile TinyRE object file
$(TRE_OBJ): $(TRE_SRC) $(INC_DIR)/tre.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executable
$(TEST_TARGET): $(TEST_OBJ) $(LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile test object file
$(TEST_OBJ): $(TEST_SRC) $(INC_DIR)/tre.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Build test executable
$(ERROR_TARGET): $(ERROR_OBJ) $(LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile test object file
$(ERROR_OBJ): $(ERROR_SRC) $(INC_DIR)/tre.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Test target
test: $(TEST_TARGET)
	@echo "Running TinyRE tests..."
	./$(TEST_TARGET)
	@echo "Running TinyRE errors..."
	./$(ERROR_TARGET)

# Install headers and library
install: $(LIB)
	@mkdir -p $(INC_DIR)
	cp $(INC_DIR)/tre.h $(INC_DIR)/tre.h
	cp $(LIB) $(BUILD_DIR)/libtre.a
	@echo "TinyRE installed to $(BUILD_DIR)/"

# Clean all builds (objects, executables, libraries)
clean:
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(BUILD_DIR)/test_tre $(BUILD_DIR)/test_error
	@echo "All builds removed (*.o, *.a, executables)"
	@echo "Builds cleaned"

# Very clean - also remove backup files
distclean: clean
	@rm -f Makefile.old

# Show help
help:
	@echo "TinyRE - Minimalist Regular Expression Engine"
	@echo ""
	@echo "Targets:"
	@echo "  all            - Build library and test binary (default)"
	@echo "  $(LIB)         - Build TinyRE static library"
	@echo "  $(TEST_TARGET) - Build test executable"
	@echo "  test           - Build and run tests"
	@echo "  install        - Install headers and library"
	@echo "  clean          - Remove all build artifacts (*.o, *.a, executables)"
	@echo "  distclean      - Remove all generated files"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC            : C compiler ($(CC))"
	@echo "  CFLAGS        : Compiler flags ($(CFLAGS))"
	@echo "  LDFLAGS       : Linker flags ($(LDFLAGS))"
	@echo "  BUILD_DIR     : Build directory ($(BUILD_DIR))"
	@echo ""
	@echo "Examples:"
	@echo "  make                          # Build everything"
	@echo "  make CC=clang                 # Use Clang compiler"
	@echo "  make CFLAGS='-O3 -march=native' # Optimize for this machine"
	@echo "  make test                     # Build and run tests"
	@echo "  make clean                   # Remove all build artifacts"

# Phony targets
.PHONY: all test install clean distclean help

# Dependencies
$(TRE_OBJ): $(wildcard $(SRC_DIR)/*.h)
$(TEST_OBJ): $(wildcard $(INC_DIR)/*.h)
