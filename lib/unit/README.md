# unit

Shared C11 unit-test helper for repository projects. Include `unit/unit.h` by
adding this library's `include/` directory to the compiler's `-I` paths.
Compile `src/unit.c` into each project's test binary; keep `tests/main.c` and
project test cases local. Production binaries do not need this library.

The API contract, architecture, acceptance checks, and proposed conclusion
are in [the Czech specification](spec/specifikace-unit.md).

Run `make PLATFORM=linux_x86_64 test` on Linux or
`mingw32-make PLATFORM=win32_x86_64 test` on Windows. Build artifacts stay in
`.cache/<platform>/`. Moving this library requires a change to the consumer's
`-I` and source path, without changing `#include "unit/unit.h"`.
