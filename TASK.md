# TASK.md

## Task
Build a basic database from scratch in C++ as an educational project.

## Main goal
Implement a minimal relational database engine that persists data to disk and supports a small SQL-like interface.

## Success criteria
The project should:
- compile cleanly
- support creating a table
- insert rows
- select rows
- delete rows
- persist data across process restarts
- include unit and integration tests for core behavior

## Functional requirements

### Schema
Support table definitions with a small set of column types:
- INT
- TEXT

### Queries
Implement a minimal subset of SQL:
- `CREATE TABLE table_name (col TYPE, ...)`
- `INSERT INTO table_name VALUES (...)`
- `SELECT * FROM table_name`
- `DELETE FROM table_name`

Optional stretch:
- `SELECT * FROM table_name WHERE col = value`

### Storage
Implement:
- fixed-size pages
- table storage in a file
- row serialization/deserialization
- metadata persistence for schemas

### Execution
Implement:
- statement parsing into an AST
- execution for create/insert/select/delete
- full table scan for reads
- clear error reporting for invalid input

## Non-goals for v1
Do not implement unless specifically requested:
- transactions
- crash recovery
- concurrent readers/writers
- SQL optimizer
- advanced indexing
- joins
- network protocol

## Suggested milestones

### Milestone 1: Scaffold
- set up CMake
- choose C++ standard
- add test framework
- create source directory structure

### Milestone 2: Storage primitives
- page abstraction
- pager/file manager
- binary read/write helpers
- page-level tests

### Milestone 3: Records and schema
- define column and schema classes
- implement row serialization
- persist and reload metadata

### Milestone 4: Table storage
- append rows
- scan rows
- mark/delete rows
- persistence tests

### Milestone 5: Parser
- tokenizer
- parser
- AST for supported statements
- parser tests

### Milestone 6: Execution engine
- execute create/insert/select/delete
- wire parser to execution
- end-to-end tests

### Milestone 7: REPL
- add command-line interface
- allow interactive statements
- add example usage in README

### Milestone 8: Catalog Persistence & Advanced SQL
- Persist catalog/schema metadata in Page 0.
- Execute `DELETE` statements (tombstone markers).
- Support `WHERE <col> = <value>` for SELECT and DELETE.
- Integration tests for cross-restart schema availability.

### Milestone 9: Basic Indexing
- Support `CREATE INDEX` syntax.
- Implement Hash Index storage for O(1) equality lookups.
- Automatically use index in `SELECT` and `DELETE` with `WHERE`.
- Maintain index integrity during `INSERT`.

### Milestone 10: Extensive Testing & Benchmarking
- Expand test suite to 100+ individual test cases.
- Comprehensive coverage for Pager, Parser, and Execution engine.
- Benchmarking for O(N) vs O(1) query performance.

### Milestone 11: Networking & Remote Queries
- Implement a TCP server listening for SQL strings.
- Create a client-server bridge for remote database operations.
- Support "Mini DB over the wire" functionality.

## Deliverable expectations
For each milestone:
- code
- tests
- short design note
- known limitations
- next step recommendation

## Preferred implementation style
- modern C++17+
- readable and explicit interfaces
- minimal magic
- strong separation between parser, storage, and execution
- small PR-sized changes
