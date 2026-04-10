# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-milestone-4] - 2026-04-10
### Added
- `TableHeap` class for physical row storage across multiple pages.
- Simple page layout with header and record-level serialization.
- Sequential scan support for data retrieval.
- Multi-page insertion and persistence tests in `tests/table_test.cpp`.
