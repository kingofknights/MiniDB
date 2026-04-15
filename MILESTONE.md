# Milestone Checklist

## Milestone 1: Scaffold
- [x] create CMake project
- [x] set C++ standard to 17 or newer
- [x] add test framework
- [x] create source tree
- [x] compile hello-world main

## Milestone 2: Pager
- [x] define page size
- [x] implement Page class
- [x] implement Pager open/read/write/allocate
- [x] add page I/O tests

## Milestone 3: Schema + Records
- [x] define Column type
- [x] define Schema type
- [x] implement row serializer
- [x] implement row deserializer
- [x] add round-trip tests

## Milestone 4: Table Heap
- [x] append rows
- [x] scan rows
- [x] support delete marker
- [x] add persistence tests

## Milestone 5: Parser
- [x] implement lexer
- [x] implement AST
- [x] parse CREATE TABLE
- [x] parse INSERT
- [x] parse SELECT
- [x] parse DELETE
- [x] add parser tests

## Milestone 6: Execution
- [x] wire AST to executor
- [x] create tables through catalog
- [x] insert rows through table heap
- [x] select rows through scan
- [x] delete rows
- [x] add end-to-end tests

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
- [x] Implement B-Tree node structure (Internal vs Leaf nodes).
- [x] Support range query operators in Parser (`>`, `<`, `>=`, `<=`).
- [x] Implement B-Tree Insert and Search logic.
- [x] Support node splitting and rebalancing.
- [x] Integrate B-Tree into Executor for range scans.

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

## Milestone 15: Transactions & WAL
- [x] Support `BEGIN`, `COMMIT`, and `ROLLBACK` in Parser.
- [x] Implement a Log Manager for Write-Ahead Logging (WAL).
- [x] Implement Atomicity: ensure multi-statement changes are all-or-nothing.
- [x] Support simple crash recovery by replaying the log on startup.

## Milestone 16: Aggregations & Group By
- [x] Support `COUNT()`, `SUM()`, `AVG()`, `MIN()`, `MAX()` in Parser.
- [x] Support `GROUP BY` syntax and basic grouping logic.
- [x] Implement aggregation execution in the query engine.
- [x] Add analytical tests for summary queries.

## Milestone 17: Multi-column Indexes
- [ ] Update Parser to support `CREATE INDEX idx ON table (col1, col2, ...)`.
- [ ] Extend `HashIndex` and `BTreeIndex` to support composite keys.
- [ ] Update index maintenance logic for multi-column updates.
- [ ] Integrate multi-column lookup in the Executor.

## Stretch
- [ ] benchmark script
- [ ] better error reporting
