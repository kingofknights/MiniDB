# Mini DB

Mini DB is an educational, relational database built from scratch in C++17. The primary goal is to provide a clean, understandable implementation of core database concepts such as storage management, SQL parsing, and query execution.

## 🚀 Features

- **Slotted Page Storage**: Professional page layout with a slot directory for efficient variable-length record management.
- **SQL Interface**: Supports core SQL operations:
  - `CREATE TABLE` with `INT` and `TEXT` types.
  - `INSERT INTO` records.
  - `SELECT *` with optional `WHERE` filters and `JOIN`.
  - `DELETE` records with space reclamation support.
  - `UPDATE` existing records.
- **Indexing**:
  - **Hash Index**: O(1) equality lookups for fast retrieval.
  - **B-Tree Index**: Ordered indexing supporting range queries (`>`, `<`, `>=`, `<=`).
- **Networking**: TCP Client/Server architecture for remote SQL execution.
- **Persistence**: Fixed-size 4KB pages in a single database file; table schemas persist across restarts.
- **Robust Testing**: 100+ unit and integration tests using GoogleTest.
- **Benchmarking**: Built-in utility to compare scan vs. index performance.

## 🏗️ Architecture

Mini DB follows a clean layered architecture:

1.  **Storage (`src/storage/`)**: Manages the Pager (File I/O), Page layout (Slotted Pages), and Record serialization.
2.  **Catalog (`src/catalog/`)**: Handles metadata, table schemas, and persistence of definitions.
3.  **Parser (`src/parser/`)**: A custom Lexer and recursive-descent Parser that converts SQL strings into an AST.
4.  **Execution (`src/execution/`)**: A dispatcher that bridges the Parser and Storage/Catalog layers to run queries.
5.  **Network (`src/network/`)**: TCP server/client implementation for remote interactions.
6.  **REPL (`src/repl/`)**: An interactive command-line shell.

## 🛠️ Getting Started

### Prerequisites
- CMake (3.14+)
- C++17 Compatible Compiler (GCC 7+, Clang 5+)
- Git

### Build Instructions
```bash
# Clone the repository (if not already done)
git clone <repo-url>
cd database

# Configure and build
cmake -S . -B build
cmake --build build -j$(nproc)
```

### Running Tests
```bash
ctest --test-dir build --output-on-failure
```

### Running the REPL
```bash
./build/src/minidb_repl
```

### Running the Server
```bash
# Start the server in one terminal
./build/src/minidb_server

# Connect using the client in another terminal
./build/src/minidb_client 127.0.0.1 "SELECT * FROM users;"
```

## 📝 Example Usage

```sql
-- Create a table
CREATE TABLE users (id INT, name TEXT);

-- Create an index for fast lookups
CREATE INDEX id_idx ON users (id);

-- Insert data
INSERT INTO users VALUES (1, 'Alice');
INSERT INTO users VALUES (2, 'Bob');

-- Query with filtering
SELECT * FROM users WHERE id = 1;

-- Update data
UPDATE users SET name = 'Alicia' WHERE id = 1;

-- Relational Join
CREATE TABLE orders (order_id INT, user_id INT);
INSERT INTO orders VALUES (101, 1);
SELECT * FROM users JOIN orders ON users.id = orders.user_id;

-- Delete data
DELETE FROM users WHERE id = 2;
```

## 📜 Roadmap

- [x] Milestone 1-7: Core Engine (Pager to REPL)
- [x] Milestone 8: Catalog Persistence
- [x] Milestone 9: Basic Indexing (Hash)
- [x] Milestone 10: Extensive Testing & Benchmarking
- [x] Milestone 11: Networking
- [x] Milestone 12: B-Tree Indexes (Range Queries)
- [x] Milestone 13: Slotted Pages
- [x] Milestone 14: UPDATE & Joins
- [ ] Milestone 15: Transactions & WAL (Future)
- [ ] Milestone 16: Aggregations (Future)

## ⚖️ License
This project is for educational purposes.
