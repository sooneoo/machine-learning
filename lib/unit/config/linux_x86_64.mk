CC = gcc
AR = ar
CFLAGS += -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Iinclude

OBJ := $(CACHE)/unit.o
LIB_OUTPUT := $(RELEASE)/libunit.a
TEST_OUTPUT := $(RELEASE)/unit_test
TEST_OBJ := $(CACHE)/test_main.o

build: $(LIB_OUTPUT)

$(LIB_OUTPUT): $(OBJ) | env
	$(AR) rcs $@ $(OBJ)

$(TEST_OUTPUT): $(TEST_OBJ) $(LIB_OUTPUT) | env
	$(CC) $(TEST_OBJ) $(LIB_OUTPUT) -o $@

$(CACHE)/unit.o: src/unit.c include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@

$(CACHE)/test_main.o: tests/main.c include/unit/unit.h | env
	$(CC) $(CFLAGS) -c $< -o $@
