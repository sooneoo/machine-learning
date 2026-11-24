# Repository instructions

## Purpose and scope

This repository contains independent artificial intelligence research experiments. Keep each experiment in its own directory with its own `README.md`, `Makefile`, `config/`, source files, and any focused tests. Record the research question, implementation choices, data provenance, build and run commands, evaluation method, results, and known limitations in that experiment's README. Keep raw data, downloaded models, generated outputs, and credentials out of Git unless explicitly requested.

## Implementation

- Write the target implementation in C. Implement algorithms and data structures for the specific research problem; do not introduce machine learning frameworks. Use the C standard library and small system libraries where needed. Use CUDA only for an explicitly requested CUDA platform.
- Prefer simple, measurable implementations suited to limited hardware. Make memory ownership, bounds, error handling, random seeds, and numerical assumptions explicit. Avoid unnecessary allocations and large dependencies. Document any optimization and compare it with a reproducible baseline.
- Use Java-style brace placement and four-space indentation in C source and headers. Put the opening brace on the same line as `if`, `else`, `for`, `while`, `switch`, and function declarations. Use spaces, not tabs, for C indentation; keep identifiers and source comments in English. For example:

  ```c
  if (condition) {
      run_step();
  } else {
      handle_error();
  }
  ```

- Keep platform-independent algorithms separate from operating system and accelerator code. Expose platform-specific operations through a small HAL interface in a header. Put each implementation in a file suffixed with its exact platform name, for example `src/hal_linux_x86_64.c` or `src/hal_win32_x86_64.c`. Do not scatter platform `#ifdef` branches through the algorithm.

## Platforms and build layout

- Implement and verify `linux_x86_64` by default. Add `win32_x86_64`, `linux_cuda_x86_64`, or `win32_cuda_x86_64` only when the task requests that platform. A platform is supported only when its configuration and HAL compile and have been checked on that platform; otherwise describe it as unverified.
- Give every experiment a small root `Makefile`. Put all platform-specific compiler, linker, object, library, and accelerator rules in `config/$(PLATFORM).mk`. Provide `config/linux_x86_64.mk` for the default platform. Use a platform-suffixed HAL source when platform code is needed. Keep build products under `.cache/` and ignore that directory in Git.
- Use GNU Make. Make recipe indentation must be a literal tab; C source indentation uses spaces. Keep `all`, `build`, `exec`, `env`, and `clean` targets. The following is a starting pattern; adapt `TARGET`, objects, and commands to the experiment:

  ```make
  PLATFORM ?= linux_x86_64
  CACHE := .cache
  RELEASE := $(CACHE)/release
  TARGET := test
  MKDIR ?= mkdir -pv
  RM ?= rm -rvf

  .PHONY: all build exec env clean
  all: build

  include config/$(PLATFORM).mk

  exec: build
	$(RELEASE)/$(TARGET)

  env:
	$(MKDIR) $(CACHE)
	$(MKDIR) $(RELEASE)

  clean:
	$(RM) $(CACHE)
  ```

  `config/linux_x86_64.mk` can start with:

  ```make
  CC = gcc
  CFLAGS += -std=c11 -O2 -Wall -Wextra -pedantic -Iinclude
  LIBS +=
  OBJ += $(CACHE)/main.o

  build: $(RELEASE)/$(TARGET)

  $(RELEASE)/$(TARGET): $(OBJ) | env
	$(CC) $(OBJ) $(LIBS) -o $@

  $(CACHE)/main.o: src/main.c | env
	$(CC) $(CFLAGS) -c $< -o $@
  ```

  Add actual header dependencies and any needed libraries to the platform configuration. The order-only `env` prerequisite ensures directories exist for parallel builds. For a Windows build, adapt the platform configuration, executable suffix, and shell commands to the selected toolchain.

## Research workflow and Git

1. Define a small experiment or change with a testable question and a measurable success criterion. Keep unrelated research changes separate.
2. Implement the smallest useful version, then build and run focused checks. Record the exact commands, platform, inputs or seed, and results. Do not claim a platform, network service, GPU, or model run was tested unless it actually was.
3. After each successfully completed and verified project change, create a local Git commit automatically. Stage only files belonging to that change, inspect the diff before committing, and use a short imperative commit message. Do not include unrelated or pre-existing user changes in the commit. Never push; the repository owner handles remote pushes.
4. If verification fails or a change is incomplete, leave it uncommitted and report the failure. Never discard user work with a reset or clean. Do not commit `.cache/`, binaries, large data, secrets, or machine-specific paths.
