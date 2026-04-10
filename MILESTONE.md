# Milestone Checklist

## Milestone 1: Scaffold
- [ ] create CMake project
- [ ] set C++ standard to 17 or newer
- [ ] add test framework
- [ ] create source tree
- [ ] compile hello-world main

## Milestone 2: Pager
- [ ] define page size
- [ ] implement Page class
- [ ] implement Pager open/read/write/allocate
- [ ] add page I/O tests

## Milestone 3: Schema + Records
- [ ] define Column type
- [ ] define Schema type
- [ ] implement row serializer
- [ ] implement row deserializer
- [ ] add round-trip tests

## Milestone 4: Table Heap
- [ ] append rows
- [ ] scan rows
- [ ] support delete marker
- [ ] add persistence tests

## Milestone 5: Parser
- [ ] implement lexer
- [ ] implement AST
- [ ] parse CREATE TABLE
- [ ] parse INSERT
- [ ] parse SELECT
- [ ] parse DELETE
- [ ] add parser tests

## Milestone 6: Execution
- [ ] wire AST to executor
- [ ] create tables through catalog
- [ ] insert rows through table heap
- [ ] select rows through scan
- [ ] delete rows
- [ ] add end-to-end tests

## Milestone 7: REPL
- [x] implement prompt loop
- [x] add .exit
- [x] add .help
- [x] print basic results
- [x] document usage in README

## Milestone 8: Catalog Persistence & Advanced SQL
- [x] Implement Catalog serialization/deserialization.
- [x] Persist table schemas in the database file (Page 0).
- [x] Implement `DELETE` statement execution.
- [x] Implement `WHERE` equality filter (e.g., `WHERE id = 1`).
- [x] Add persistence tests for schemas and deletion.

## Milestone 9: Basic Indexing
- [x] Parse `CREATE INDEX index_name ON table_name (column_name)`.
- [x] Store index metadata in the Catalog.
- [x] Implement a simple Hash Index for equality lookups.
- [x] Integrate index usage into the Executor for `SELECT` and `DELETE`.
- [x] Update index automatically on `INSERT`.
- [x] Add index functional tests.

## Milestone 10: Extensive Testing & Benchmarking
- [x] Achieve 100+ unit tests across all subsystems.
- [x] Implement edge-case testing (max string lengths, empty tables, etc.).
- [x] Create a dedicated benchmarking script for Insert/Select/Index performance.
- [x] Audit error handling and status codes.
- [x] Document performance characteristics in ARCHITECTURE.md.

## Milestone 11: Networking & Remote Queries
- [x] Implement a basic TCP Socket Server.
- [x] Define a simple text-based wire protocol.
- [x] Create `minidb_server` and `minidb_client`.
- [x] Add network integration tests.

## Milestone 12: B-Tree Indexes
- [ ] Implement B-Tree node structure (Internal vs Leaf nodes).
- [ ] Support range query operators in Parser (`>`, `<`, `>=`, `<=`).
- [ ] Implement B-Tree Insert and Search logic.
- [ ] Support node splitting and rebalancing.
- [ ] Integrate B-Tree into Executor for range scans.

## Milestone 13: Slotted Pages
- [x] Redesign Page layout to use a Slot Directory.
- [x] Implement free space management within pages.
- [x] Support true row deletion and space reclamation.
- [x] Update TableHeap to use slotted page offsets.

## Milestone 14: UPDATE & Nested Loop Join
- [x] Support `UPDATE table SET col = val WHERE condition`.
- [x] Implement `UPDATE` execution logic.
- [x] Support `SELECT * FROM t1 JOIN t2 ON t1.c1 = t2.c2` syntax.
- [x] Implement a basic Nested Loop Join algorithm.
- [x] Add functional tests for updates and joins.

## Milestone 15: Transactions & WAL [ON HOLD]
- [ ] Implement a Log Manager for Write-Ahead Logging.
- [ ] Support `BEGIN`, `COMMIT`, and `ROLLBACK` statements.
- [ ] Implement ACID atomicity (all or nothing) for multi-statement changes.
- [ ] Support crash recovery by replaying the log.

## Milestone 16: Aggregations & Group By [ON HOLD]
- [ ] Support `COUNT()`, `SUM()`, `AVG()`, `MIN()`, `MAX()`.
- [ ] Implement `GROUP BY` logic.
- [ ] Add `HAVING` clause filtering.

## Stretch
- [ ] single-column index
- [ ] benchmark script
- [ ] better error reporting
