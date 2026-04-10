## ARCHITECTURE.md
# Architecture

## Overview

Mini DB is organized into a few independent subsystems:

1. storage
2. catalog
3. parser
4. execution
5. repl

The guiding principle is separation of concerns. Storage should not need to know SQL syntax. The parser should not know page layout details. Execution should connect the two.

---

## 1. Storage subsystem

The storage subsystem is responsible for reading and writing persistent data.

### Responsibilities
- manage the database file
- divide file contents into fixed-size pages
- serialize and deserialize rows
- append and scan records
- provide table-level data access

### Main components

#### Page
Represents one fixed-size block of data.

Suggested defaults:
- page size: 4096 bytes
- raw byte buffer
- page id
- utility methods for reading/writing bytes

#### Pager
Handles file I/O and page loading.

Responsibilities:
- open database file
- read page by id
- write page by id
- allocate new pages
- keep page count metadata

#### Record format
Encodes a row into bytes.

Version 1 suggestion:
- store a null bitmap only if needed later
- encode INT as fixed-width
- encode TEXT as length-prefixed bytes

#### Table heap
Stores rows for a table.

Version 1 strategy:
- append rows to pages
- scan rows sequentially
- optionally mark deleted rows with a flag

---

## 2. Catalog subsystem

The catalog stores metadata about tables.

### Responsibilities
- define schemas
- manage table names
- manage column definitions
- persist metadata so schemas survive restart

### Main components

#### Column
Represents one column definition:
- name
- type

#### Schema
Represents a table schema:
- ordered list of columns
- validation rules
- row layout support

#### Catalog
Maps table names to metadata.

Version 1 metadata might include:
- table name
- schema
- first page id or root page id
- row count if desired

---

## 3. Parser subsystem

The parser turns text input into structured statements.

### Responsibilities
- tokenize input
- parse tokens into AST nodes
- validate syntax
- produce helpful errors

### Main components

#### Lexer
Converts text into tokens.

Token examples:
- keywords
- identifiers
- integers
- strings
- commas
- parentheses
- semicolons
- operators

#### AST
Represents parsed statements in memory.

Statement examples:
- CreateTableStatement
- InsertStatement
- SelectStatement
- DeleteStatement

#### Parser
Consumes tokens and builds AST nodes.

Version 1 should support a very small grammar only.

---

## 4. Execution subsystem

The execution layer runs statements against storage and catalog.

### Responsibilities
- create tables
- insert rows
- scan and return rows
- delete rows
- bridge parser output and storage operations

### Suggested design
A simple dispatcher is enough for version 1:
- parse input into AST
- switch on statement type
- invoke appropriate handler

This does not need a full query planner for v1.

---

## 5. REPL subsystem

The REPL provides a command-line interface.

### Responsibilities
- read user input
- pass input to parser and executor
- print results
- handle exit commands

Suggested built-ins:
- `.exit`
- `.help`

---

## Data flow

Typical flow for a statement:

1. user enters SQL-like text
2. lexer tokenizes input
3. parser builds AST
4. executor interprets AST
5. catalog/storage are accessed
6. results are printed

Example:

`INSERT INTO users VALUES (1, "alice");`

Flow:
- parser creates `InsertStatement`
- executor looks up `users` in catalog
- executor validates row against schema
- row is serialized
- row is appended via table heap
- pager writes affected page to disk

---

## Version 1 design decisions

### Full table scans
All reads use sequential scans at first. This keeps the design simple and makes correctness easier.

### Minimal types
Support only:
- INT
- TEXT

### Fixed-size pages
Use fixed-size pages to teach the basic physical storage model used by real databases.

### Single-process assumption
No concurrency in version 1.

### Simplicity over performance
Readable code is preferred over a highly optimized design.

---

## Risks and tradeoffs

### Variable-length TEXT
TEXT complicates row layout. Use a clear length-prefixed encoding and validate page boundaries carefully.

### Delete behavior
Simplest option:
- mark deleted rows with a flag
- ignore deleted rows during scans

Physical compaction can be added later.

### Schema persistence
Keep schema metadata in a predictable format. A dedicated catalog page or metadata file is acceptable for v1.

---

## Suggested implementation order

1. CMake and test framework
2. page and pager
3. row encoding
4. schema and catalog
5. table heap
6. lexer
7. parser
8. executor
9. repl
10. integration and persistence tests

---

## Possible version 2 upgrades

- equality predicates in `WHERE`
- single-column indexes
- slotted pages
- free space tracking
- tombstone cleanup
- better result formatting
- basic explain/debug commands
