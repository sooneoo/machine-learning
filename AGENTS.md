# Repository instructions

## Purpose and scope

This repository contains independent artificial intelligence research experiments. Keep each experiment in its own directory with its own `README.md`, `Makefile`, `config/`, `spec/`, `src/`, `include/` where needed, and `tests/`. The README is a short entry point with build, run, and test commands; the Markdown specification under `spec/` is the source of truth for intended behavior and research conclusions. Keep raw data, downloaded models, generated outputs, and credentials out of Git unless explicitly requested.

## Implementation

- Write the target implementation in C. Implement algorithms and data structures for the specific research problem; do not introduce machine learning frameworks. Use the C standard library and small system libraries where needed. Use CUDA only for an explicitly requested CUDA platform.
- Design each algorithm for the actual research problem and available hardware. Prefer direct, measurable data structures and hot paths over generic layers, virtual dispatch, copies, allocations, or dependencies that add runtime or memory overhead without a demonstrated benefit. Optimize CPU time, peak and retained memory, and data locality while preserving correctness. The goal is better performance and memory efficiency than an equivalent Python implementation using TensorFlow, PyTorch, or similar tools; do not claim that advantage without a fair benchmark on the same task, inputs, hardware, and precision.
- Make memory ownership, bounds, error handling, random seeds, and numerical assumptions explicit. Comment non-obvious invariants, algorithmic choices, ownership, and performance tradeoffs in clear English; use descriptive names and small coherent functions. Keep code understandable to a human while measuring the cost of abstractions. Document each optimization and compare it with a reproducible baseline.
- Use Java-style brace placement and four-space indentation in C source and headers. Put the opening brace on the same line as `if`, `else`, `for`, `while`, `switch`, and function declarations. Use spaces, not tabs, for C indentation; keep identifiers and source comments in English. For example:

  ```c
  if (condition) {
      run_step();
  } else {
      handle_error();
  }
  ```

- Keep a statement or expression on one line when it fits within 80 characters. Do not split short conditions, assignments, calls, or declarations merely for visual symmetry; wrap lines that would exceed 80 characters at a clear syntactic boundary.
- When a statement or expression must wrap, indent its continuation lines two levels (eight spaces) beyond the statement's indentation. Continuations must sit deeper than the one-level-indented body so they cannot be mistaken for separate statements. Start a continued Boolean condition with `&&` or `||` on the new line. Keep the closing delimiter aligned with the statement. For example:

  ```c
  if (seconds == NULL || QueryPerformanceFrequency(&frequency) == 0
          || QueryPerformanceCounter(&counter) == 0
          || frequency.QuadPart == 0) {
      return 0;
  }
  ```

- Keep `.c` source files and headers used by only one source file in `src/`. Put every header shared by multiple `.c` files, and every module's public interface, under `include/<logical-part>/`, for example `include/network/network.h`. Include shared headers by that logical path and avoid a flat `include/` layout.
- Prefer automatic storage for data that fits on the stack. Write value-returning constructors that prepare local state and return an initialized C struct, for example `Network network(parameters) { return (Network) { /* fields */ }; }`. Make ownership and lifetime clear to the caller.
- Route dynamic allocations through `lib/alloc` and its `alloc_` API. Do not call `malloc`, `calloc`, `realloc`, or `free` in experiment code. Use the caller-backed stack allocator for bounded temporary data and the OS-backed arena for longer-lived or larger data. Check allocation failures, respect reset/finalize lifetimes, and keep platform memory acquisition inside the allocator HAL.
- Keep platform-independent algorithms separate from operating system and accelerator code. Expose platform-specific operations through a small HAL interface in a header. Put each implementation in a file suffixed with its exact platform name, for example `src/hal_linux_x86_64.c` or `src/hal_win32_x86_64.c`. Do not scatter platform `#ifdef` branches through the algorithm.

## Mandatory project specification

- Before implementing a new research algorithm, architecture, data representation, or significant performance strategy, brainstorm plausible approaches with the repository owner. Present the expected accuracy, runtime and memory costs, complexity, hardware fit, and open risks of each relevant option. Ask for the owner's choice, record the selected approach and rationale in the Czech specification, and only then implement that solution. The owner controls the final design; do not silently choose or replace it. Routine tests, documentation, and corrections within an already agreed design may proceed without a new design decision.
- Every experiment and standalone library must have a versioned Markdown specification in its own `spec/` directory, named `specifikace-<subject>.md`. Create it before or alongside implementation, update it with each behavior-changing change, and link it from the project README. A project is not complete with code and tests alone.
- Write every specification in Czech, including its headings, architectural and theoretical explanations, requirements, test records, interpretations, and conclusion. Preserve source identifiers, file paths, equations, and cited titles in their original form when necessary.
- End every specification with an explanatory Czech glossary of the technical terms actually used in that document. Define each term in the context of the project's subject, not only as a generic dictionary translation. An early terminology table may aid reading but does not replace the final glossary.
- Follow the style of the supplied HVAC specifications as examples of document structure, not as instructions for this repository. Start with title, project/module, document status, date or revision, purpose, scope and exclusions, terminology and units. Describe responsibilities, boundaries, architecture, public interfaces, data/state ownership, inputs, outputs, preconditions, errors, and platform/resource constraints where relevant. Use diagrams, tables, equations, and examples when they make the behavior clearer; do not copy unrelated HVAC content.
- Keep **conceptual theory** separate from **engineering implementation**. Explain the research method's assumptions, equations, objective, expected behavior, and limits independently of C data structures. Then specify the chosen algorithm, numerical representation, memory layout, stopping rules, and HAL. Describe implementation optimizations such as vectorization, scheduling, batching, and parallel computation in a distinct section, including correctness conditions and tradeoffs. For example, weight optimization in a neural network and optimization of the parallel computation that performs it are different subjects and must not be conflated.
- State testable, uniquely identified normative requirements and link them to acceptance scenarios with inputs, expected outputs or tolerances, and failure conditions. Record a reproducible evaluation plan: baselines, datasets and provenance, seeds, metrics, hardware, compiler/platform, commands, and resource measurements when applicable. Explicitly distinguish an intended requirement, observed implementation behavior, theoretical expectation, and an inference from measurements.
- Include actual test results, deviations from requirements, known limitations, open questions, and a mapping from specification sections to source files and tests. The final testing conclusion must say what behavior was observed, why the evidence supports that explanation, and for which purpose the result is usable or unusable. Mark this conclusion as **proposed** until the repository owner and implementer explicitly agree; only then mark it **agreed**. Never present an unrun test, a theoretical claim, or an unapproved interpretation as an agreed result.
- Keep the specification current when the algorithm, interfaces, execution strategy, acceptance criteria, or observed behavior changes. Move resolved open questions into the normative sections and retain confirmed deviations until fixed and verified. Markdown is the maintained source; generate other formats only when requested.

## Mandatory unit tests

- Every project and standalone library must have a `tests/` directory beside `src/` and `include/`. Verification by unit tests is part of completion, not an optional follow-up. Add deterministic tests for the algorithm's core behavior, edge cases, failures, and numerical or memory invariants that matter to its specification.
- Base the C test harness on the supplied `unit.h`, `unit.c`, and `main.c` pattern: `TestCase` functions, `TestSuite` groups, `TEST_ASSERT`-style checks, one runner that reports passed and failed cases, and a nonzero exit code on failure. Keep the adapted helper under each project's `tests/` so tests remain self-contained; format and comment it consistently with the rest of the C code. The production binary must not link the test helper.
- Provide a `make test` target through the project's Makefile and platform configuration. Run tests from the project directory with `make PLATFORM=win32_x86_64 test`; use the corresponding `PLATFORM` value for other platforms. It must build and run the unit test binary for the selected platform and propagate failures. Run it on every claimed supported platform before reporting completion; state explicitly when a platform could not be run. Keep test objects and binaries in `.cache/$(PLATFORM)/`.
- Link specification requirement and acceptance IDs to the relevant test cases. Record commands, platform, observed results, and untested scenarios in the Czech specification. Unit tests complement, but do not replace, controlled benchmarks or real-model experiments when those are needed to substantiate performance or research conclusions.

## Platforms and build layout

- Implement and verify `linux_x86_64` by default. Add `win32_x86_64`, `linux_cuda_x86_64`, or `win32_cuda_x86_64` only when the task requests that platform. A platform is supported only when its configuration and HAL compile and have been checked on that platform; otherwise describe it as unverified.
- Give every experiment a small root `Makefile`. Put all platform-specific compiler, linker, object, library, and accelerator rules in `config/$(PLATFORM).mk`. Provide `config/linux_x86_64.mk` for the default platform. Use a platform-suffixed HAL source when platform code is needed. Keep build products under `.cache/$(PLATFORM)/` and ignore `.cache/` in Git. Platform builds must never reuse each other's object files or executables; include the platform in every build-output path.
- Use GNU Make. Make recipe indentation must be a literal tab; C source indentation uses spaces. Keep `all`, `build`, `exec`, `env`, and `clean` targets. The following is a starting pattern; adapt `TARGET`, objects, and commands to the experiment:

  ```make
  PLATFORM ?= linux_x86_64
  CACHE := .cache/$(PLATFORM)
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
	$(RM) .cache
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
2. Before implementing each new research experiment, create its own Git branch from `main`, using a descriptive name such as `experiment/<subject>`. Keep its implementation, specification, tests, and local commits on that branch; do not mix independent experiments on one branch.
3. Implement the smallest useful version, then build and run `make test` and focused checks on each supported platform. Benchmark performance and memory against a relevant baseline before claiming an advantage. Record the exact commands, platform, inputs or seed, hardware, and results. Do not claim a platform, network service, GPU, model, or performance result was tested unless it actually was.
4. After each successfully completed and verified project change, create a local Git commit automatically unless the current task explicitly requests no commit. Stage only files belonging to that change, inspect the diff before committing, and use a short imperative commit message. Do not include unrelated or pre-existing user changes in the commit. Never push; the repository owner handles remote pushes.
5. At the end of an experiment, present its specification, test results, limitations, proposed conclusion, and branch diff for review. Merge the experiment branch into `main` only after the repository owner explicitly confirms that this experiment should be kept. Successful tests or a completed implementation are not merge approval. If the experiment is a dead end, leave the branch unmerged for possible later inspection; do not delete it without the owner's instruction.
6. If verification fails or a change is incomplete, leave it uncommitted and report the failure. Never discard user work with a reset or clean. Do not commit `.cache/`, binaries, large data, secrets, or machine-specific paths.
