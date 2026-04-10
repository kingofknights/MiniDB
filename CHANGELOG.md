# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Planned
- **Milestone 11: Networking & Remote Queries**
  - Basic TCP server for remote SQL execution.
  - Wire protocol for string-based queries and results.
  - Remote client support.

## [0.1.0-milestone-12] - 2026-04-10
### Added
- **B-Tree Indexing**: Implemented an in-memory B-Tree index for ordered lookups.
- **Range Queries**: Supported `>`, `<`, `>=`, `<=` operators in `WHERE` clauses for both `SELECT` and `DELETE`.
- **Advanced Execution**: Refactored `Executor` to prioritize B-Tree indexes for range scans and fallback to Hash indexes for equality.
- **Lexer/Parser Updates**: Added support for multi-character range operators and strict token consumption.
- **Robustness**: Verified with 100+ tests including specific B-Tree range query scenarios.

## [0.1.0-milestone-11] - 2026-04-10
### Added
- **Networking Subsystem**: Implemented TCP Server and Client using POSIX sockets.
- **Wire Protocol**: Defined a simple length-prefixed protocol for sending SQL strings and receiving formatted results.
- **`minidb_server`**: New executable to host the database and listen for remote connections.
- **`minidb_client`**: New CLI utility for connecting to a remote MiniDB instance.
- **Refactoring**: Updated `Executor` to support output redirection to any `std::ostream`, enabling network response capture.
- **Network Tests**: Added end-to-end integration tests for remote query execution.

## [0.1.0-milestone-10] - 2026-04-10
### Added
- Expanded test suite to **102 individual test cases** covering Pager, Records, TableHeap, Parser, Executor, and Catalog.
- Implemented edge-case testing for boundary conditions and large data handling.
- Integrated `minidb_benchmark` utility for performance analysis.
- Documented performance results showing O(1) index lookup vs O(N) sequential scan (~90x speedup).
- Refined Parser error reporting and strict token consumption.

## [0.1.0-milestone-9] - 2026-04-10
### Added
- `HashIndex` class for O(1) equality lookups on `INT` and `TEXT` columns.
- Support for `CREATE INDEX index_name ON table_name (column_name)`.
- Index-accelerated `SELECT` queries for equality predicates.
- Automatic index updates during `INSERT` operations.
- Index functional tests in `tests/index_test.cpp`.

## [0.1.0-milestone-8] - 2026-04-10
### Added
- Catalog Persistence: Table schemas are now serialized to Page 0 and survive restarts.
- `DELETE` statement: Records can be marked as deleted using 1-byte tombstone markers.
- `WHERE` clause: Equality filtering supported for `SELECT` and `DELETE`.
- Name Normalization: Table and column names are now case-insensitive (stored as uppercase).
- Comprehensive tests for catalog persistence, deletion, and filtering.

## [0.1.0] - 2026-04-10
### Added
- Milestone 7: Interactive REPL shell for executing SQL.
- Support for `.exit` and `.help` commands.
- Configurable database filename via command-line arguments.
- Completed all v1 milestones: Pager, Record, Schema, TableHeap, Parser, Executor, and REPL.
