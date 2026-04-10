#include <iostream>
#include <string>
#include <memory>
#include "src/parser/lexer.h"
#include "src/parser/parser.h"
#include "src/execution/executor.h"

using namespace minidb;

void PrintHelp() {
    std::cout << "MiniDB v0.1.0\n"
              << "Available commands:\n"
              << "  .exit         Exit the REPL\n"
              << "  .help         Show this help message\n"
              << "  SQL queries:  CREATE TABLE, INSERT INTO, SELECT * FROM\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    std::string db_file = "minidb.db";
    if (argc > 1) {
        db_file = argv[1];
    }

    Status status = Status::OK();
    auto pager = Pager::Open(db_file, status);
    if (!status.ok()) {
        std::cerr << "Error: " << status.message() << std::endl;
        return 1;
    }

    std::unique_ptr<Catalog> catalog;
    if (pager->GetPageCount() == 0) {
        catalog = std::make_unique<Catalog>();
        // Allocate Page 0 for Catalog
        pager->AllocatePage();
    } else {
        Page page0;
        pager->ReadPage(0, page0);
        catalog = Catalog::Deserialize(page0.GetData());
    }

    Executor executor(*catalog, *pager);

    std::cout << "Welcome to MiniDB!" << std::endl;
    std::cout << "Connected to: " << db_file << std::endl;
    std::cout << "Type '.help' for more information." << std::endl;

    std::string line;
    while (true) {
        std::cout << "minidb> ";
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) continue;

        if (line == ".exit") break;
        if (line == ".help") {
            PrintHelp();
            continue;
        }

        Lexer lexer(line);
        auto tokens = lexer.Tokenize();
        if (tokens.empty() || tokens[0].type == TokenType::END_OF_FILE) continue;

        Parser parser(tokens);
        Status parse_status = Status::OK();
        auto stmt = parser.Parse(parse_status);

        if (!parse_status.ok()) {
            std::cerr << "Parser Error: " << parse_status.message() << std::endl;
            continue;
        }

        Status exec_status = executor.Execute(*stmt);
        if (!exec_status.ok()) {
            std::cerr << "Execution Error: " << exec_status.message() << std::endl;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}
