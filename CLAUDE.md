# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a fork of SQLite 2.5.0, modernized to compile with current GCC on MacOS 10.13+ and Debian 8+. The codebase is optimized for code reading and studying database implementation, with approximately 20,000 lines of ANSI C code.

**Purpose**: Educational database implementation for understanding SQL database design and architecture. This is an early SQLite version with a clean, comprehensible codebase.

## Build System

The project uses autoconf/libtool for configuration and Make for building.

### Basic Build Commands

```bash
# Standard build (in-tree)
./configure            # Generate Makefile from Makefile.in
make                   # Builds libsqlite.la and sqlite binary
make clean             # Remove build artifacts

# Recommended out-of-tree build
mkdir bld && cd bld
../configure
make
```

### Build Fixes for Modern Compilers

The Makefile has been updated to compile the lemon parser generator with flags compatible with modern GCC:
- Uses `-std=gnu89` to support old K&R C style code
- Adds `-Wno-implicit-function-declaration` and `-Wno-int-conversion` to suppress warnings

Additionally, `src/shell.c` has been fixed to remove a conflicting `isatty()` declaration (now properly included from `<unistd.h>`).

### Build Targets

```bash
make sqlite            # Build the sqlite CLI shell
make libsqlite.la      # Build the core library
make lemon             # Build the Lemon parser generator
make tclsqlite         # Build TCL extension
make testfixture       # Build test harness
```

### Testing

```bash
make test              # Run quick test suite
make fulltest          # Run complete test suite
```

Both test targets require TCL to be installed and build the `testfixture` binary.

### Documentation Generation

```bash
make doc               # Generate HTML documentation from www/*.tcl scripts
```

Requires TCL. Outputs to `doc/` directory.

**Note**: The `doc/` directory contains original source documentation (lemon.html, report1.txt) that is preserved during `make clean`. Only generated HTML files are removed.

## Architecture

SQLite 2.5.0 has a layered architecture with clear separation of concerns:

### Core Layers (Bottom to Top)

1. **OS Layer** (`os.c`, `os.h`)
   - Platform abstraction for file I/O and locking
   - Defines OS_UNIX and OS_WIN at compile time

2. **Pager** (`pager.c`, `pager.h`)
   - Page cache management (default 1024-byte pages)
   - Transaction support via journaling
   - Manages reads/writes at page granularity
   - Rollback capability through journal files

3. **B-Tree** (`btree.c`, `btree.h`)
   - Key-value storage engine
   - Implements both tables (integer keys) and indices (arbitrary keys)
   - Cursor-based navigation API
   - Built on top of pager

4. **Virtual Database Engine (VDBE)** (`vdbe.c`, `vdbe.h`)
   - Stack-based virtual machine that executes database operations
   - SQL queries are compiled into bytecode programs
   - Each VDBE instruction has opcode + up to 3 operands (p1, p2, p3)
   - See `www/opcode.tcl` for opcode documentation generation

5. **SQL Compiler**
   - **Parser** (`parse.y`) - Lemon-generated from grammar file
   - **Tokenizer** (`tokenize.c`) - Lexical analysis
   - **Code Generator** (`build.c`, `expr.c`, `select.c`, `insert.c`, `update.c`, `delete.c`, `where.c`, `trigger.c`) - Converts parse tree to VDBE bytecode

6. **Public API** (`main.c`, `sqlite.h.in`)
   - C API for external applications
   - TCL binding in `tclsqlite.c`

### Supporting Components

- **Hash Table** (`hash.c`, `hash.h`) - Used throughout for symbol tables
- **Utility Functions** (`util.c`) - String manipulation, error handling
- **printf** (`printf.c`) - Custom printf implementation with %q/%Q for SQL escaping
- **Random** (`random.c`) - Random number generation for temp file names

### Build Process Flow

1. `sqlite.h` is generated from `src/sqlite.h.in` (version + encoding substitution)
2. Lemon parser generator is built from `tool/lemon.c`
3. `parse.c` and `parse.h` are generated from `src/parse.y` using lemon
4. All `.c` files in src/ are compiled to `.lo` (libtool objects)
5. Objects are linked into `libsqlite.la` (shared library)
6. Shell (`src/shell.c`) is linked against library to create `sqlite` binary

## Key Internal Headers

- `sqliteInt.h` - Main internal header, includes all other headers, defines core types
- `vdbe.h` - VDBE instruction format and VM interface
- `btree.h` - B-tree storage engine API
- `pager.h` - Page cache interface
- `hash.h` - Hash table implementation
- `parse.h` - Generated from parse.y, defines token types

## Configuration

Encoding is set at build time via `ENCODING` in Makefile:
- `ISO8859` (default) - Single-byte character encoding
- `UTF8` - Multi-byte UTF-8 encoding

Constants in `sqliteInt.h`:
- `MAX_PAGES 2000` - Main database cache size
- `TEMP_PAGES 500` - Temporary table cache size
- `MAX_BYTES_PER_ROW 1048576` - 1MB row size limit

## Test Infrastructure

Tests are in `test/` directory, written in TCL using the `testfixture` binary which includes:
- Core SQLite library
- TCL bindings
- Test harness code (`test1.c`, `test2.c`, `test3.c`)
- MD5 implementation (`md5.c`) for test verification

Test suites:
- `quick.test` - Fast smoke tests
- `all.test` - Comprehensive test suite
- Individual `*.test` files for specific features

## Tools

- `tool/lemon.c` - Parser generator (similar to yacc/bison but thread-safe)
- `tool/lempar.c` - Lemon parser template
- `tool/*.awk` - Various AWK scripts for code generation and analysis
- `tool/speedtest.tcl` / `tool/speedtest2.tcl` - Performance benchmarks

## Common Development Patterns

When modifying the codebase:

1. **Adding SQL syntax**: Modify `src/parse.y`, rebuild with lemon
2. **Adding VDBE opcodes**: Add to `src/vdbe.c`, update opcode list
3. **B-tree changes**: Ensure pager transaction semantics are preserved
4. **Parser changes**: Run lemon to regenerate `parse.c` and `parse.h`

The separation between compilation (SQL → VDBE bytecode) and execution (VDBE interpreter) is fundamental to understanding the architecture.
