PLATFORM ?= linux_x86_64
CACHE := .cache/$(PLATFORM)
RELEASE := $(CACHE)/release
MKDIR_CMD ?= mkdir -p
CLEAN_CMD ?= rm -rf .cache
DIR_PATH = $@

.PHONY: all build test exec env clean
all: build

include config/$(PLATFORM).mk

test: $(TEST_OUTPUT)
	$(TEST_OUTPUT)

exec: test

env: $(CACHE) $(RELEASE)

$(CACHE):
	$(MKDIR_CMD) $(DIR_PATH)

$(RELEASE): | $(CACHE)
	$(MKDIR_CMD) $(DIR_PATH)

clean:
	$(CLEAN_CMD)
