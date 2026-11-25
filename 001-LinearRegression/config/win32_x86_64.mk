LIB_ROOT ?= ../lib
CFLAGS += -I$(LIB_ROOT)/alloc/include -I$(LIB_ROOT)/unit/include

# Windows compilation rules follow after the project specification is agreed.
SHELL := cmd.exe
build test:
	@echo Specification pending; no implementation exists yet.
	@cmd /C exit 1
