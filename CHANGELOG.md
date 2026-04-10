# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-milestone-2] - 2026-04-10
### Added
- `Status` class for unified error handling in `src/common/status.h`.
- `Page` class representing a 4KB data block.
- `Pager` class for file-backed storage (read/write/allocate).
- Unit tests for Pager (persistence, allocation, I/O) in `tests/storage_test.cpp`.
- Integrated `minidb_storage` library into CMake.
