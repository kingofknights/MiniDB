# Changelog

All notable changes to the Mini DB project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-milestone-3] - 2026-04-10
### Added
- `Column` and `Schema` classes for table metadata in `src/catalog/`.
- `Value` and `Record` classes with binary serialization for `INT` and `TEXT`.
- Round-trip serialization tests in `tests/record_test.cpp`.
- `minidb_catalog` library and updated build system.
