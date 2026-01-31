# TinyRE Makefile

CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
CFLAGS  ?= -std=c99 -Wall -Wextra -O2 -I include
LDFLAGS ?=

SRC_DIR	= src
INC_DIR = include
LIB_NAME= libtre.a
OBJS    = $(SRC_DIR)/tre.o
TEST_SRC= $(SRC_DIR)/test_tre.c

all: $(LIB_NAME) test

# Build object from src/
$(SRC_DIR)/tre.o: $(SRC_DIR)/tre.c $(INC_DIR)/tre.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library
$(LIB_NAME): $(OBJS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

# Build test program (links against the static lib)
test: $(TEST_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) $(TEST_SRC) -L. -ltre -o test $(LDFLAGS)

# Run tests
check: test
	./test

# Clean builds
clean:
	rm -f $(OBJS) $(LIB_NAME) test core *.core

# Phony targets
.PHONY: all clean check test lib
