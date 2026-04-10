# AGENTS.md

## Project objective
Build a small educational relational database in C++ from scratch.

The goal is not to compete with SQLite or Postgres. The goal is to produce a clean, understandable implementation of core database concepts:
- persistent storage
- schemas
- row layout
- table scans
- a tiny SQL-like parser
- a simple execution engine

## Agent priorities
When working on this repository, optimize for:
1. correctness
2. clarity
3. testability
4. small incremental changes
5. educational value

Avoid overengineering. Prefer the simplest design that can support the current milestone.

## Version 1 scope
In scope:
- file-backed persistence
- fixed-size pages
- row serialization and deserialization
- schema and catalog metadata
- CREATE TABLE
- INSERT
- SELECT * FROM <table>
- DELETE FROM <table>
- optional WHERE with simple equality predicate if easy to support

Out of scope unless explicitly requested:
- concurrency control
- transactions
- WAL/recovery
- MVCC
- cost-based optimization
- distributed systems
- advanced indexing
- joins beyond naive experimentation

## Expected architecture
Organize code into clear subsystems.

Suggested layout:
- `src/common/` shared types, status/error helpers, utilities
- `src/storage/` pager, page layout, record encoding, table heap
- `src/catalog/` schema, column definitions, table metadata
- `src/parser/` lexer, parser, AST nodes
- `src/execution/` executors and statement handling
- `src/repl/` command loop or shell
- `tests/` subsystem and integration tests

If the existing codebase differs, follow the established layout unless a refactor is explicitly requested.

## Coding rules
- Use C++17 or newer.
- Prefer RAII and standard library containers.
- Prefer explicit, readable code over abstraction-heavy patterns.
- Keep headers minimal and implementation details in `.cpp` files when practical.
- Avoid global mutable state.
- Keep functions focused and small.
- Document invariants near the code that relies on them.

## Change strategy
For each task:
1. understand the exact subsystem being changed
2. propose the minimum viable design
3. implement in small steps
4. add or update tests
5. run verification commands
6. summarize what changed and any remaining limitations

Do not mix unrelated refactors into the same change.

## Database design guidance
Default assumptions unless the task says otherwise:
- fixed-size page architecture
- append-friendly row storage
- explicit serialization format
- simple catalog persistence
- full table scan as the default query strategy
- schema types kept intentionally minimal

Recommended initial column types:
- INT
- TEXT

Recommended first milestone:
- one database file
- one or more tables
- row insert/select/delete
- basic metadata persistence

## Testing expectations
Every meaningful change should include tests when feasible.

Prioritize tests for:
- row serialization round-trips
- page boundary conditions
- schema validation
- parser correctness
- end-to-end query execution
- persistence across restart

If a bug is fixed, add a regression test whenever practical.

## Build and verification
Before finishing a task, run the relevant checks that exist in the repo.

Preferred commands if available:
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

If sanitizers or lint targets exist, run them for non-trivial changes.

If a command fails because the repo is not fully set up yet, say so clearly and continue with the best validated result possible.

## Communication style
When responding after a code change, include:
- what was changed
- why it was changed
- what tests were run
- known limitations
- the next best step

When planning work, break it into milestones with concrete deliverables.

## Milestone order
Default implementation order:
1. project scaffolding and build setup
2. pager and page abstraction
3. row format and serialization
4. schema/catalog
5. table heap operations
6. parser and AST
7. execution engine
8. REPL and integration tests
9. simple indexing experiments

## Decision rule
When unsure between a clever design and a simple one, choose the simple one.
