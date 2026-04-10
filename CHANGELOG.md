# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-milestone-5] - 2026-04-10
### Added
- SQL Lexer for tokenizing keywords, literals, and punctuation.
- Recursive descent Parser for `CREATE TABLE`, `INSERT`, `SELECT`, and `DELETE`.
- Abstract Syntax Tree (AST) nodes for all supported statements.
- Parser unit tests in `tests/parser_test.cpp`.
