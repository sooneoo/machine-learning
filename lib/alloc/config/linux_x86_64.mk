LIB_ROOT ?= ..
CC = gcc
AR = ar
CFLAGS += -std=c11 -O2 -Wall -Wextra -Werror -pedantic -D_DEFAULT_SOURCE -Iinclude -I$(LIB_ROOT)/unit/include
OBJ := $(CACHE)/stack.o $(CACHE)/arena.o $(CACHE)/pages_linux_x86_64.o
LIB_OUTPUT := $(RELEASE)/liballoc.a
TEST_OUTPUT := $(RELEASE)/alloc_test
TEST_OBJ := $(CACHE)/test_main.o $(CACHE)/test_alloc.o $(CACHE)/unit.o

build: $(LIB_OUTPUT)

$(LIB_OUTPUT): $(OBJ) | env
	$(AR) rcs $@ $(OBJ)

$(TEST_OUTPUT): $(TEST_OBJ) $(LIB_OUTPUT) | env
	$(CC) $(TEST_OBJ) $(LIB_OUTPUT) -o $@

$(CACHE)/stack.o: src/stack.c include/alloc/stack.h include/alloc/alloc.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/arena.o: src/arena.c include/alloc/arena.h include/alloc/pages.h include/alloc/alloc.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/pages_linux_x86_64.o: src/pages_linux_x86_64.c include/alloc/pages.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_main.o: tests/main.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_alloc.o: tests/test_alloc.c $(LIB_ROOT)/unit/include/unit/unit.h include/alloc/alloc.h include/alloc/arena.h include/alloc/stack.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/unit.o: $(LIB_ROOT)/unit/src/unit.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@
