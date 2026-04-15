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

## [0.1.0-milestone-18] - 2026-04-10
### Added
- **Foreign Key Constraints**: Supported `FOREIGN KEY (col) REFERENCES parent_table(parent_col)` in `CREATE TABLE`.
- **Referential Integrity**: Implemented runtime validation to ensure parent records exist before inserting child records.
- **Dependency Protection**: Added checks to prevent deletion of parent records that are referenced by existing child records.
- **Catalog Enhancements**: Updated `Schema` and `Catalog` to persist foreign key metadata across restarts.
- **Parser/Lexer Updates**: Added `FOREIGN`, `KEY`, and `REFERENCES` tokens and grammar rules.
- **Verification**: Verified with comprehensive integration tests for valid and invalid referential operations.

## [0.1.0-milestone-17] - 2026-04-10
### Added
- **Multi-column Indexes**: Supported `CREATE INDEX idx ON table (col1, col2, ...)` for composite keys.
- **Index Enhancements**: Updated `HashIndex` and `BTreeIndex` to handle vectors of `Value` objects with delimiter-based key serialization.
- **Parser Support**: Expanded SQL grammar to accept comma-separated column lists in index creation.
- **Execution Logic**: Refactored `Executor` to populate and maintain multi-column indexes during `INSERT` and `CREATE INDEX` operations.
- **Verification**: All 100+ tests passing, including fixes for benchmarking and extensive regression suites.

## [0.1.0-milestone-16] - 2026-04-10
### Added
- **Aggregations**: Supported `COUNT()`, `SUM()`, `AVG()`, `MIN()`, and `MAX()` functions in `SELECT` statements.
- **Grouping**: Implemented `GROUP BY` clause for categorizing data and calculating aggregates per group.
- **Analytical Engine**: Refactored `Executor` to handle multi-stage query processing (filtering, then grouping/aggregating).
- **Parser Support**: Updated SQL grammar to recognize analytical keywords and complex select-list expressions.
- **Verification**: Verified with 100+ tests including specific analytical and grouping scenarios.

## [0.1.0-milestone-15] - 2026-04-10
### Added
- **Transactions**: Supported `BEGIN`, `COMMIT`, and `ROLLBACK` for multi-statement atomic changes.
- **Write-Ahead Logging (WAL)**: Implemented `LogManager` to persist all database changes to a `*.log` file before applying them.
- **Transaction Context**: Updated `Executor` to track transaction state and log all DML operations (`INSERT`, `DELETE`, `UPDATE`) to the WAL.
- **Integration**: Updated REPL, Server, and test suite to utilize the new WAL-backed execution engine.
- **Persistence**: Ensured all data modification operations are durable across crashes via log flushing.

## [0.1.0-milestone-14] - 2026-04-10
### Added
- **UPDATE Statement**: Support for modifying existing records via `UPDATE table SET col = val WHERE condition`.
- **Nested Loop Join**: Implemented relational joining between two tables using a simple nested loop algorithm.
- **Parser Support**: Updated SQL grammar to handle `UPDATE`, `SET`, and `JOIN ... ON ...` syntax.
- **Relational Capabilities**: Enabled multi-table querying and data modification, further expanding Mini DB's SQL coverage.
- **Robustness**: All 100+ tests passing, including new DML and relational logic.

## [0.1.0-milestone-13] - 2026-04-10
### Added
- **Slotted Page Storage**: Implemented a professional slot-directory page layout.
- **Space Management**: Records now grow from the end of the page, while the slot directory grows from the start.
- **Improved Deletion**: Moved from record-level tombstones to slot-level deletion flags for better management.
- **Page Safety**: Added a 4-byte magic number (`MDB1`) to the page header to identify and initialize database pages.
- **Robustness**: Refactored `TableHeap` and `Executor` to use slotted offsets, passing 100+ regression tests.

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
