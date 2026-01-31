# TinyRE Makefile
# Builds static library libtre.a + test program

CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
CFLAGS  ?= -std=c99 -Wall -Wextra -O2 -I include
LDFLAGS ?=

SRC_DIR    = src
LIB_NAME   = libtre.a
OBJS       = $(SRC_DIR)/tre.o
TEST_SRC   = $(SRC_DIR)/test_tre.c

all: $(LIB_NAME) test

# Build object from src/
$(SRC_DIR)/tre.o: $(SRC_DIR)/tre.c include/tre.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tre.c -o $@

# Build static library (this is the 'make lib' target)
$(LIB_NAME): $(OBJS)
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@ || true

lib: $(LIB_NAME)

# Build test program (links against the static lib)
test: $(TEST_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) $(TEST_SRC) -L. -ltre -o test $(LDFLAGS)

# Run tests
check: test
	./test

# Clean build artifacts
clean:
	rm -f $(OBJS) $(LIB_NAME) test core *.core

# Phony targets
.PHONY: all clean check test lib


