#pragma once
#include <unordered_map>
#include <list>
#include "page.h"
#include "disk_manager.h"

class BufferPool {
private:
    int capacity;
    DiskManager& disk_manager;

    // list stores {page_id, Page} pairs
    // front = most recently used
    // back  = least recently used
    std::list<std::pair<int, Page>> lru_list;

    // maps page_id → iterator into lru_list for O(1) access
    std::unordered_map<int,
        std::list<std::pair<int, Page>>::iterator> page_map;

public:
    BufferPool(int capacity, DiskManager& dm);

    Page& getPage(int page_id);
    void  pinPage(int page_id, const Page& page);
    void  markDirty(int page_id);
    void evictPage(int page_id);//to evict a specific page in buffer if the operation says to delete the key value pair
    void flushAll();

private:
    void evict();
};