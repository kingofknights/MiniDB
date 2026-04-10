#include "src/storage/table_heap.h"
#include <cstring>

namespace minidb {

/**
 * Simple page layout for v1:
 * [0-3]: num_records (uint32_t)
 * [4...]: raw serialized records
 */
struct PageHeader {
    uint32_t num_records;
};

Status TableHeap::InsertRecord(const Record& record) {
    auto serialized = Record::Serialize(schema_, record);
    if (serialized.size() + sizeof(PageHeader) > PAGE_SIZE) {
        return Status::IOError("Record too large to fit in a single page");
    }
    
    // Find a page with enough space or allocate a new one
    PageID target_page_id = INVALID_PAGE_ID;
    Page page;
    
    for (PageID i = 1; i < pager_.GetPageCount(); ++i) {
        pager_.ReadPage(i, page);
        PageHeader* header = reinterpret_cast<PageHeader*>(page.GetData());
        
        // Calculate used space (v1: very simple scan)
        size_t used_space = sizeof(PageHeader);
        size_t bytes_read = 0;
        uint8_t* ptr = page.GetData() + sizeof(PageHeader);
        for (uint32_t r = 0; r < header->num_records; ++r) {
            Record::Deserialize(schema_, ptr, bytes_read);
            used_space += bytes_read;
            ptr += bytes_read;
        }

        if (used_space + serialized.size() <= PAGE_SIZE) {
            target_page_id = i;
            break;
        }
    }

    if (target_page_id == INVALID_PAGE_ID) {
        target_page_id = pager_.AllocatePage();
        pager_.ReadPage(target_page_id, page);
        PageHeader* header = reinterpret_cast<PageHeader*>(page.GetData());
        header->num_records = 0;
    }

    // Append to the end of the page
    PageHeader* header = reinterpret_cast<PageHeader*>(page.GetData());
    uint8_t* ptr = page.GetData() + sizeof(PageHeader);
    size_t bytes_read = 0;
    for (uint32_t r = 0; r < header->num_records; ++r) {
        Record::Deserialize(schema_, ptr, bytes_read);
        ptr += bytes_read;
    }

    std::memcpy(ptr, serialized.data(), serialized.size());
    header->num_records++;
    
    return pager_.WritePage(page);
}

std::vector<Record> TableHeap::Scan() {
    std::vector<Record> records;
    Page page;
    
    for (PageID i = 1; i < pager_.GetPageCount(); ++i) {
        pager_.ReadPage(i, page);
        PageHeader* header = reinterpret_cast<PageHeader*>(page.GetData());
        uint8_t* ptr = page.GetData() + sizeof(PageHeader);
        
        for (uint32_t r = 0; r < header->num_records; ++r) {
            size_t bytes_read = 0;
            Record rec = Record::Deserialize(schema_, ptr, bytes_read);
            if (!rec.IsDeleted()) {
                records.push_back(std::move(rec));
            }
            ptr += bytes_read;
        }
    }
    
    return records;
}

} // namespace minidb
