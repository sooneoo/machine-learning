LIB_ROOT ?= ../lib
CFLAGS += -I$(LIB_ROOT)/alloc/include -I$(LIB_ROOT)/unit/include

# Compilation rules follow after the project specification is agreed.
build test:
	@echo "Specification pending; no implementation exists yet." >&2
	@exit 1
