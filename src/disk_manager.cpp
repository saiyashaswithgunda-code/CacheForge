#include "disk_manager.h"
#include <iostream>

DiskManager::DiskManager(const std::string& path) : file_path(path), page_count(0) {
    db_file.open(path, std::ios::in | std::ios::out | std::ios::binary);//open the file to write/read and also in binary format
    
    if (!db_file.is_open()) {
        //file doesnt exist yet so cant open for reading so open it for writing and in binary format        
        db_file.open(path, std::ios::out | std::ios::binary);
        db_file.close();
        db_file.open(path, std::ios::in | std::ios::out | std::ios::binary);//now the file already exists so we can open the file to w/r
    }

    db_file.seekg(0, std::ios::end);//from std::ios::end that is from the end of the file move '0' no of steps
    int file_size = db_file.tellg();//gives current position
    page_count = file_size / PAGE_SIZE;
}

DiskManager::~DiskManager() {
    if (db_file.is_open()) db_file.close();
}

void DiskManager::writePage(Page& page) {
    int offset = page.page_id * PAGE_SIZE;
    db_file.seekp(offset);
    db_file.write(page.data, PAGE_SIZE);
    db_file.flush();//forces write to disk immediately
    page.is_dirty = false;//so page is now sync with the database
}

void DiskManager::readPage(int page_id, Page& page) {
    int offset = page_id * PAGE_SIZE;
    db_file.seekg(offset);
    db_file.read(page.data, PAGE_SIZE);
}

int DiskManager::allocateNewPage() {
    return page_count++;
}