# alloc

Small C11 allocator library for experiments that need dynamic memory without `malloc`, `calloc`, or `free`. It provides a stack allocator backed by caller-owned storage and an arena backed by OS virtual memory. The API is in `include/alloc/`; `include/alloc/pages.h` is an internal interface shared by the arena and its platform implementations.

The normative architecture, allocator theory, acceptance scenarios, observed results, and proposed testing conclusion are in [the library specification](spec/specifikace-alloc.md).

## Usage

Prefer an ordinary stack value when the data fits there. For a caller-owned buffer:

```c
#include "alloc/stack.h"

unsigned char storage[4096];
Alloc_Stack stack = alloc_stack(storage, sizeof(storage));
Alloc *allocator = ALLOC(&stack);
void *data = alloc_new(allocator, 128);
alloc_finalize(allocator);
```

`alloc_stack` returns an `Alloc_Stack` value and aligns the usable portion of the buffer to `max_align_t`. The buffer must outlive every allocation from it. `alloc_finalize` resets the stack allocator; it does not free caller-owned storage. The original `alloc_stack_new(capacity, buffer)` entry point remains available when the allocator structure must be placed inside the buffer.

For an OS-backed arena:

```c
#include "alloc/arena.h"

Alloc *allocator = alloc_arena_new(4096);
if (allocator != NULL) {
    void *data = alloc_new(allocator, 128);
    /* Use data while the arena is alive. */
    alloc_finalize(allocator);
}
```

`alloc_arena_new(0)` uses a 4096-byte default segment. The arena acquires segments through `mmap` on Linux and `VirtualAlloc` on Windows. `alloc_reset` releases its segments while leaving the arena reusable; `alloc_finalize` releases the segments and the arena itself. Call `alloc_finalize` exactly once on a live arena.

## Allocation contract

- All returned payloads are aligned to `max_align_t`. A zero-byte request consumes a minimum payload slot. Invalid or oversized requests return `NULL`.
- `alloc_delete` reclaims only the newest allocation in the stack, or the newest allocation in the arena's active segment. Deleting an older allocation is a no-op. Use `alloc_reset` to discard all allocations at once.
- `alloc_resize(NULL, size)` allocates. Resizing the newest allocation is in place when it fits; otherwise it allocates a new block, copies the lesser of the old and new requested sizes, and leaves the old block for later reset. On failure the original allocation remains valid. `alloc_resize(pointer, 0)` attempts `alloc_delete` and returns `NULL`.
- Do not access pointers after their block is reclaimed, after `alloc_reset`, or after `alloc_finalize`. Neither allocator is thread safe. Always use the allocator that created a pointer.
- The original short function names `new`, `delete`, `resize`, `reset`, and `finalize` remain as compatibility wrappers. Prefer the `alloc_` names in new code.

## Build and verification

Linux x86-64 with GCC and GNU Make:

```sh
make -j4 test
make clean
```

Windows x86-64 with MSYS2 MinGW-w64 GCC and `mingw32-make`:

```powershell
mingw32-make PLATFORM=win32_x86_64 -j4 test
mingw32-make PLATFORM=win32_x86_64 clean
```

`build` creates `.cache/<platform>/release/liballoc.a`; `test` builds and runs the five-case unit suite based on the supplied `unit.c`/`unit.h` pattern. `exec` is an alias for `test`. The platform configuration selects exactly one OS memory implementation. On 2026-11-24, all five cases passed with the Windows MSYS2 toolchain and with GCC in WSL Arch. The Linux suite also passed under AddressSanitizer and UndefinedBehaviorSanitizer. The checks cover alignment, LIFO deletion, in-place and moving resize, content preservation, reset, failure paths, and finalization. No performance or memory comparison with a system allocator or Python framework has been run.

This library allocates host memory on Linux and Windows. CUDA device memory needs a separate accelerator-specific interface when a GPU experiment requires it; it is not covered by these builds or tests.
