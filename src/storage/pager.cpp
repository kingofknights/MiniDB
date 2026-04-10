#include "src/storage/pager.h"
#include <iostream>
#include <filesystem>

namespace minidb {

Pager::Pager(const std::string& filename, std::fstream file, size_t page_count)
    : filename_(filename), file_(std::move(file)), page_count_(page_count) {}

Pager::~Pager() {
    Close();
}

std::unique_ptr<Pager> Pager::Open(const std::string& filename, Status& status) {
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!file.is_open()) {
        // Create the file if it doesn't exist
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            status = Status::IOError("Could not open or create file: " + filename);
            return nullptr;
        }
    }

    size_t file_size = std::filesystem::file_size(filename);
    if (file_size % PAGE_SIZE != 0) {
        status = Status::IOError("Database file size is not a multiple of PAGE_SIZE");
        return nullptr;
    }

    status = Status::OK();
    return std::unique_ptr<Pager>(new Pager(filename, std::move(file), file_size / PAGE_SIZE));
}

Status Pager::ReadPage(PageID page_id, Page& page) {
    if (page_id >= page_count_) {
        return Status::IOError("Invalid page ID: " + std::to_string(page_id));
    }

    file_.seekg(page_id * PAGE_SIZE, std::ios::beg);
    file_.read(reinterpret_cast<char*>(page.GetData()), PAGE_SIZE);
    
    if (file_.fail()) {
        file_.clear();
        return Status::IOError("Failed to read page: " + std::to_string(page_id));
    }

    page.SetPageID(page_id);
    return Status::OK();
}

Status Pager::WritePage(const Page& page) {
    PageID page_id = page.GetPageID();
    if (page_id >= page_count_) {
        return Status::IOError("Invalid page ID: " + std::to_string(page_id));
    }

    file_.seekp(page_id * PAGE_SIZE, std::ios::beg);
    file_.write(reinterpret_cast<const char*>(page.GetData()), PAGE_SIZE);
    
    if (file_.fail()) {
        file_.clear();
        return Status::IOError("Failed to write page: " + std::to_string(page_id));
    }

    file_.flush();
    return Status::OK();
}

PageID Pager::AllocatePage() {
    PageID new_page_id = static_cast<PageID>(page_count_);
    page_count_++;

    // Physically extend the file
    file_.seekp((page_count_ * PAGE_SIZE) - 1, std::ios::beg);
    file_.put('\0');
    file_.flush();

    return new_page_id;
}

void Pager::Close() {
    if (file_.is_open()) {
        file_.close();
    }
}

} // namespace minidb
