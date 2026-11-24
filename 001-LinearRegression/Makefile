PLATFORM ?= linux_x86_64
CACHE := .cache/$(PLATFORM)
RELEASE := $(CACHE)/release
TARGET := linear_regression

.PHONY: all build test exec env clean
all: build

include config/$(PLATFORM).mk

exec: build
	$(RELEASE)/$(TARGET)

env:
	mkdir -pv $(CACHE)
	mkdir -pv $(RELEASE)

clean:
	rm -rvf .cache
