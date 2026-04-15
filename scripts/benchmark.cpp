#include <iostream>
#include <chrono>
#include <vector>
#include <filesystem>
#include "src/storage/pager.h"
#include "src/catalog/catalog.h"
#include "src/execution/executor.h"
#include "src/parser/parser.h"
#include "src/storage/log_manager.h"

using namespace minidb;

void RunBenchmark() {
    std::string db_file = "benchmark.db";
    std::filesystem::remove(db_file);
    std::filesystem::remove("benchmark.log");

    Status status = Status::OK();
    auto pager = Pager::Open(db_file, status);
    pager->AllocatePage(); // Catalog
    Catalog catalog;
    LogManager log_manager("benchmark.log");
    Executor executor(catalog, *pager, log_manager);


    // 1. Setup Table
    {
        Lexer lexer("CREATE TABLE DATA (ID INT);");
        auto tokens = lexer.Tokenize();
        Parser parser(tokens);
        auto stmt = parser.Parse(status);
        executor.Execute(*stmt);
    }

    const int NUM_RECORDS = 2000;
    std::cout << "Inserting " << NUM_RECORDS << " records..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RECORDS; ++i) {
        InsertStatement ins;
        ins.table_name = "DATA";
        ins.raw_values = {std::to_string(i)};
        executor.Execute(ins);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Insertion took: " << diff.count() << "s (" << (NUM_RECORDS/diff.count()) << " rows/s)" << std::endl;

    // 2. Benchmark Scan (No Index)
    std::cout << "Benchmarking Sequential Scan (searching for last ID)..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    {
        SelectStatement sel;
        sel.table_name = "DATA";
        sel.where = std::make_unique<WhereClause>();
        sel.where->column_name = "ID";
        sel.where->value = std::to_string(NUM_RECORDS - 1);
        executor.Execute(sel);
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Scan took: " << diff.count() << "s" << std::endl;

    // 3. Create Index
    std::cout << "Creating Hash Index on DATA(ID)..." << std::endl;
    {
        CreateIndexStatement idx;
        idx.index_name = "IDX_ID";
        idx.table_name = "DATA";
        idx.column_name = "ID";
        executor.Execute(idx);
    }

    // 4. Benchmark Index Lookup
    std::cout << "Benchmarking Index Lookup (searching for last ID)..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    {
        SelectStatement sel;
        sel.table_name = "DATA";
        sel.where = std::make_unique<WhereClause>();
        sel.where->column_name = "ID";
        sel.where->value = std::to_string(NUM_RECORDS - 1);
        executor.Execute(sel);
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Index Lookup took: " << diff.count() << "s" << std::endl;

    pager->Close();
    std::filesystem::remove(db_file);
}

int main() {
    try {
        RunBenchmark();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
