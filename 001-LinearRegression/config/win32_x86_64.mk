CC = gcc
LIB_ROOT ?= ../lib
CFLAGS += -std=c11 -O2 -Wall -Wextra -Werror -pedantic
CFLAGS += -Iinclude -I$(LIB_ROOT)/alloc/include
CFLAGS += -I$(LIB_ROOT)/unit/include
MKDIR_CMD = cmd /C mkdir
CLEAN_CMD = cmd /C if exist .cache rmdir /S /Q .cache
DIR_PATH = $(subst /,\,$@)

OBJ := $(CACHE)/main.o $(CACHE)/regression.o $(CACHE)/optimizer.o
BIN_OUTPUT := $(RELEASE)/$(TARGET).exe
TEST_OUTPUT := $(RELEASE)/linear_regression_test.exe
TEST_OBJ := $(CACHE)/test_main.o $(CACHE)/test_regression.o
TEST_OBJ += $(CACHE)/unit.o $(CACHE)/regression.o $(CACHE)/optimizer.o

build: $(BIN_OUTPUT)

$(BIN_OUTPUT): $(OBJ) | env
	$(CC) $(OBJ) -o $@

$(TEST_OUTPUT): $(TEST_OBJ) | env
	$(CC) $(TEST_OBJ) -o $@

$(CACHE)/main.o: src/main.c include/regression/regression.h include/regression/optimizer.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/regression.o: src/regression.c include/regression/regression.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/optimizer.o: src/optimizer.c include/regression/optimizer.h include/regression/regression.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_main.o: tests/main.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_regression.o: tests/test_regression.c $(LIB_ROOT)/unit/include/unit/unit.h include/regression/optimizer.h include/regression/regression.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/unit.o: $(LIB_ROOT)/unit/src/unit.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@
