# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
