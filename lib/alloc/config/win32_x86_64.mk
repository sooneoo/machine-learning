CC = gcc
AR = gcc-ar
CFLAGS += -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Iinclude
OBJ := $(CACHE)/stack.o $(CACHE)/arena.o $(CACHE)/pages_win32_x86_64.o
LIB_OUTPUT := $(RELEASE)/liballoc.a
TEST_OUTPUT := $(RELEASE)/alloc_test.exe
TEST_OBJ := $(CACHE)/test_main.o $(CACHE)/test_alloc.o $(CACHE)/unit.o
LIBS += -lkernel32
MKDIR_CMD = cmd /C mkdir
CLEAN_CMD = cmd /C if exist .cache rmdir /S /Q .cache
DIR_PATH = $(subst /,\,$@)

build: $(LIB_OUTPUT)

$(LIB_OUTPUT): $(OBJ) | env
	$(AR) rcs $@ $(OBJ)

$(TEST_OUTPUT): $(TEST_OBJ) $(LIB_OUTPUT) | env
	$(CC) $(TEST_OBJ) $(LIB_OUTPUT) $(LIBS) -o $@

$(CACHE)/stack.o: src/stack.c include/alloc/stack.h include/alloc/alloc.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/arena.o: src/arena.c include/alloc/arena.h include/alloc/pages.h include/alloc/alloc.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/pages_win32_x86_64.o: src/pages_win32_x86_64.c include/alloc/pages.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_main.o: tests/main.c tests/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_alloc.o: tests/test_alloc.c tests/unit.h include/alloc/alloc.h include/alloc/arena.h include/alloc/stack.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/unit.o: tests/unit.c tests/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@
