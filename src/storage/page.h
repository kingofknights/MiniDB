#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <cstring>

namespace minidb {

using PageID = uint32_t;
static constexpr size_t PAGE_SIZE = 4096;
static constexpr PageID INVALID_PAGE_ID = 0xFFFFFFFF;
static constexpr uint32_t SLOTTED_PAGE_MAGIC = 0x4D444231; // "MDB1"

/**
 * Slotted Page Header
 */
struct SlottedPageHeader {
    uint32_t magic;
    uint32_t num_slots;
    uint32_t free_space_offset;
};

/**
 * Slot Entry
 */
struct Slot {
    uint32_t offset;
    uint32_t length;
    bool deleted;
};

class Page {
public:
    Page() : page_id_(INVALID_PAGE_ID) {
        data_.fill(0);
        InitializeHeader();
    }

    void InitializeHeader() {
        auto* header = GetHeader();
        header->magic = SLOTTED_PAGE_MAGIC;
        header->num_slots = 0;
        header->free_space_offset = PAGE_SIZE;
    }

    SlottedPageHeader* GetHeader() { return reinterpret_cast<SlottedPageHeader*>(data_.data()); }
    const SlottedPageHeader* GetHeader() const { return reinterpret_cast<const SlottedPageHeader*>(data_.data()); }

    Slot* GetSlot(uint32_t i) { 
        return reinterpret_cast<Slot*>(data_.data() + sizeof(SlottedPageHeader) + (i * sizeof(Slot))); 
    }
    const Slot* GetSlot(uint32_t i) const {
        return reinterpret_cast<const Slot*>(data_.data() + sizeof(SlottedPageHeader) + (i * sizeof(Slot)));
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
