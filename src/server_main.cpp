#include "src/network/server.h"
#include <iostream>

using namespace minidb;

int main(int argc, char* argv[]) {
    std::string db_file = "minidb_remote.db";
    uint16_t port = 5432;

    Status status = Status::OK();
    auto pager = Pager::Open(db_file, status);
    if (!status.ok()) {
        std::cerr << "Error: " << status.message() << std::endl;
        return 1;
    }

    std::unique_ptr<Catalog> catalog;
    if (pager->GetPageCount() == 0) {
        catalog = std::make_unique<Catalog>();
        pager->AllocatePage(); // Page 0 for catalog
    } else {
        Page p0;
        pager->ReadPage(0, p0);
        catalog = Catalog::Deserialize(p0.GetData());
    }

    LogManager log_manager("minidb_remote.log");
    Server server(*catalog, *pager, log_manager, port);
    server.Start();

    return 0;
}
