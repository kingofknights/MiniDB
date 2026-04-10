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
- [ ] Implement Catalog serialization/deserialization.
- [ ] Persist table schemas in the database file (Page 0).
- [ ] Implement `DELETE` statement execution.
- [ ] Implement `WHERE` equality filter (e.g., `WHERE id = 1`).
- [ ] Add persistence tests for schemas and deletion.

## Stretch
- [ ] single-column index
- [ ] benchmark script
- [ ] better error reporting
