#include "buffer_pool.h"
#include <iostream>

BufferPool::BufferPool(int cap, DiskManager& dm)
    : capacity(cap), disk_manager(dm) {}

Page& BufferPool::getPage(int page_id) {
    // Cache hit — page already in memory
    if (page_map.find(page_id) != page_map.end()) {
        std::cout << "[BufferPool] Cache HIT — page " 
                  << page_id << std::endl;

        // Move to front — most recently used
        auto it = page_map[page_id];
        lru_list.splice(lru_list.begin(), lru_list, it);

        return lru_list.front().second;
    }

    std::cout << "[BufferPool] Cache MISS — page "
              << page_id << ", loading from disk" << std::endl;

    // Cache miss — load from disk
    if (lru_list.size() >= capacity) {
        evict();
    }

    Page p(page_id);
    disk_manager.readPage(page_id, p);

    lru_list.push_front({page_id, p});
    page_map[page_id] = lru_list.begin();

    return lru_list.front().second;
}

void BufferPool::pinPage(int page_id, const Page& page) {
    // If page already in buffer — update it
    if (page_map.find(page_id) != page_map.end()) {
        auto it = page_map[page_id];
        it->second = page;
        lru_list.splice(lru_list.begin(), lru_list, it);
        return;
    }

    // Evict if full
    if (lru_list.size() >= capacity) {
        evict();
    }

    lru_list.push_front({page_id, page});
    page_map[page_id] = lru_list.begin();
}

void BufferPool::markDirty(int page_id) {
    if (page_map.find(page_id) != page_map.end()) {
        page_map[page_id]->second.is_dirty = true;
    }
}

void BufferPool::evict() {
    // Evict from back — least recently used
    auto last = lru_list.back();
    int evicted_page_id = last.first;
    Page& evicted_page  = last.second;

    // If dirty — write back to disk before evicting
    if (evicted_page.is_dirty) {
        std::cout << "[BufferPool] Evicting dirty page "
                  << evicted_page_id
                  << " — writing to disk first" << std::endl;
        disk_manager.writePage(evicted_page);
    } else {
        std::cout << "[BufferPool] Evicting clean page "
                  << evicted_page_id << std::endl;
    }

    page_map.erase(evicted_page_id);
    lru_list.pop_back();
}
void BufferPool::flushAll() {
    for (auto& entry : lru_list) {
        if (entry.second.is_dirty) {
            std::cout << "[BufferPool] Flushing dirty page "
                      << entry.first << " to disk" << std::endl;
            disk_manager.writePage(entry.second);
            entry.second.is_dirty = false;
        }
    }
}