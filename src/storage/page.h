#pragma once
#include <cstdint>
#include <vector>
#include <array>

namespace minidb {

using PageID = uint32_t;
static constexpr size_t PAGE_SIZE = 4096;
static constexpr PageID INVALID_PAGE_ID = 0xFFFFFFFF;

class Page {
public:
    Page() : page_id_(INVALID_PAGE_ID) {
        data_.fill(0);
    }

    PageID GetPageID() const { return page_id_; }
    void SetPageID(PageID id) { page_id_ = id; }

    uint8_t* GetData() { return data_.data(); }
    const uint8_t* GetData() const { return data_.data(); }

private:
    PageID page_id_;
    std::array<uint8_t, PAGE_SIZE> data_;
};

} // namespace minidb
