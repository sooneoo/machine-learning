CC = gcc
LIB_ROOT ?= ../lib
CFLAGS += -std=c11 -O2 -Wall -Wextra -Werror -pedantic
CFLAGS += -Iinclude -I$(LIB_ROOT)/alloc/include
CFLAGS += -I$(LIB_ROOT)/unit/include
LDLIBS += -lm

OBJ := $(CACHE)/main.o $(CACHE)/perceptron.o $(CACHE)/optimizer.o
BIN_OUTPUT := $(RELEASE)/$(TARGET)
TEST_OUTPUT := $(RELEASE)/perceptron_test
TEST_OBJ := $(CACHE)/test_main.o $(CACHE)/test_perceptron.o
TEST_OBJ += $(CACHE)/unit.o $(CACHE)/perceptron.o $(CACHE)/optimizer.o

build: $(BIN_OUTPUT)

$(BIN_OUTPUT): $(OBJ) | env
	$(CC) $(OBJ) $(LDLIBS) -o $@

$(TEST_OUTPUT): $(TEST_OBJ) | env
	$(CC) $(TEST_OBJ) $(LDLIBS) -o $@

$(CACHE)/main.o: src/main.c include/perceptron/perceptron.h include/perceptron/optimizer.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/perceptron.o: src/perceptron.c include/perceptron/perceptron.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/optimizer.o: src/optimizer.c include/perceptron/optimizer.h include/perceptron/perceptron.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_main.o: tests/main.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_perceptron.o: tests/test_perceptron.c $(LIB_ROOT)/unit/include/unit/unit.h include/perceptron/optimizer.h include/perceptron/perceptron.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/unit.o: $(LIB_ROOT)/unit/src/unit.c $(LIB_ROOT)/unit/include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@
