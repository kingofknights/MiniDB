#pragma once
#include "src/common/status.h"
#include "src/storage/page.h"
#include <string>
#include <fstream>
#include <memory>

namespace minidb {

class Pager {
public:
    static std::unique_ptr<Pager> Open(const std::string& filename, Status& status);

    Status ReadPage(PageID page_id, Page& page);
    Status WritePage(const Page& page);
    PageID AllocatePage();

    size_t GetPageCount() const { return page_count_; }
    void Close();

    ~Pager();

private:
    Pager(const std::string& filename, std::fstream file, size_t page_count);

    std::string filename_;
    std::fstream file_;
    size_t page_count_;
};

} // namespace minidb
