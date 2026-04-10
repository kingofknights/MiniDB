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
- [ ] implement prompt loop
- [ ] add .exit
- [ ] add .help
- [ ] print basic results
- [ ] document usage in README

## Stretch
- [ ] WHERE equality filters
- [ ] single-column index
- [ ] benchmark script
- [ ] better error reporting
