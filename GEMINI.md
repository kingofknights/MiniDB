# Gemini Project Instructions - Mini DB

This document serves as the foundational mandate for Gemini CLI when working on the **Mini DB** project. It takes precedence over general defaults.

## 1. Project Identity & Objective
**Mini DB** is an educational, relational database built from scratch in C++17. The goal is a clean, understandable implementation of core database concepts rather than a high-performance production engine.

### Core Priorities
1. **Correctness:** Data must be accurately persisted and retrieved.
2. **Clarity:** Code should be readable and educational.
3. **Testability:** Every subsystem must have unit and integration tests.
4. **Simplicity:** Prefer the simplest design that satisfies the current milestone.

---

## 2. Technical Stack & Standards
- **Language:** C++17 (or newer).
- **Build System:** CMake.
- **Persistence:** Fixed-size 4KB pages in a single database file.
- **Concurrency:** Single-process, single-threaded for Version 1.
- **Error Handling:** Use explicit status/error helpers in `src/common/`.
- **Coding Style:** RAII, standard library containers, minimal headers, and no global mutable state.

---

## 3. System Architecture
Follow the established five-layer separation of concerns:
1. **Storage (`src/storage/`):** Pager (File I/O), Page layout, Record serialization.
2. **Catalog (`src/catalog/`):** Schema definitions (`INT`, `TEXT`), Table metadata persistence.
3. **Parser (`src/parser/`):** Lexer, AST nodes, and SQL-like grammar (CREATE, INSERT, SELECT, DELETE).
4. **Execution (`src/execution/`):** Simple dispatcher to bridge Parser and Storage.
5. **REPL (`src/repl/`):** Interactive shell and built-in commands (`.exit`, `.help`).

---

## 4. Development Roadmap
1. **Milestone 1-7:** Core Version 1 (Scaffold to REPL). [COMPLETED]
2. **Milestone 8: Catalog Persistence & Advanced SQL:**
   - Serialize/Deserialize Catalog to/from Page 0.
   - Implement `DELETE` and `WHERE` equality predicates.
   - Ensure table schemas survive process restarts.
3. **Milestone 9: Basic Indexing (Future):**
   - Single-column B-Tree or Hash index.

---

## 5. Operational Mandates for Gemini
- **Surgical Updates:** Modify only what is necessary for the current task.
- **Validation-First:** Every feature MUST have **GoogleTest** tests. No change is complete without passing tests.
- **Reproduce First:** For bug fixes, create a reproduction test case before applying the fix.
- **Incremental Progress:** Commit changes milestone-by-milestone. Each milestone MUST be tagged in Git (e.g., `v0.1.0-milestone-1`).
- **Documentation:** Everything must be documented. Maintain inline code comments, `ARCHITECTURE.md`, and this `GEMINI.md`.
- **Changelog:** Always update `CHANGELOG.md` for every notable change or milestone completion.

---

## 6. Version 1 Constraints
- **Scanning:** Default to full table scans (no indexes/joins).
- **Types:** Only `INT` (fixed-width) and `TEXT` (length-prefixed).
- **Storage:** Fixed-size pages; append-friendly row storage.
- **Metadata:** Simple schema persistence in a predictable location.
