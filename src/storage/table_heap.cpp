#include "src/storage/table_heap.h"
#include <cstring>

namespace minidb {

Status TableHeap::InsertRecord(const Record& record) {
    auto serialized = Record::Serialize(schema_, record);
    uint32_t record_size = static_cast<uint32_t>(serialized.size());
    
    if (record_size + sizeof(Slot) + sizeof(SlottedPageHeader) > PAGE_SIZE) {
        return Status::IOError("Record too large for a single page");
    }

    PageID target_page_id = INVALID_PAGE_ID;
    Page page;
    
    for (PageID i = 1; i < pager_.GetPageCount(); ++i) {
        pager_.ReadPage(i, page);
        auto* header = page.GetHeader();
        
        if (header->magic != SLOTTED_PAGE_MAGIC) {
            page.InitializeHeader();
            header = page.GetHeader();
        }
        
        uint32_t slot_dir_end = sizeof(SlottedPageHeader) + (header->num_slots * sizeof(Slot));
        uint32_t free_space = header->free_space_offset - slot_dir_end;

        if (free_space >= record_size + sizeof(Slot)) {
            target_page_id = i;
            break;
        }
    }

    if (target_page_id == INVALID_PAGE_ID) {
        target_page_id = pager_.AllocatePage();
        pager_.ReadPage(target_page_id, page);
        page.InitializeHeader();
    }

    auto* header = page.GetHeader();
    uint32_t slot_id = header->num_slots++;
    header->free_space_offset -= record_size;
    
    Slot* slot = page.GetSlot(slot_id);
    slot->offset = header->free_space_offset;
    slot->length = record_size;
    slot->deleted = false;

    std::memcpy(page.GetData() + slot->offset, serialized.data(), record_size);
    
    return pager_.WritePage(page);
}

std::vector<Record> TableHeap::Scan() {
    std::vector<Record> records;
    Page page;
    
    for (PageID i = 1; i < pager_.GetPageCount(); ++i) {
        pager_.ReadPage(i, page);
        auto* header = page.GetHeader();
        
        if (header->magic != SLOTTED_PAGE_MAGIC) continue;

        for (uint32_t s = 0; s < header->num_slots; ++s) {
            Slot* slot = page.GetSlot(s);
            if (!slot->deleted) {
                size_t bytes_read = 0;
                records.push_back(Record::Deserialize(schema_, page.GetData() + slot->offset, bytes_read));
            }
        }
    }
    
    return records;
}

} // namespace minidb
