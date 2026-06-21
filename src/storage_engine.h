#pragma once
#include <string>
#include "disk_manager.h"
#include "hash_index.h"
#include "buffer_pool.h"

class WAL; // forward declaration to avoid circular include

class StorageEngine {
private:
    DiskManager  disk_manager;
    HashIndex    hash_index;
    BufferPool   buffer_pool;
    WAL*         wal;//here we use WAL* pointer bcz at this point we only know a WAL class exists 
                    // but we dont know its elements so we cant assign the storage for it here itself so we use ptr which is of same size always 

public:
    StorageEngine(const std::string& db_path,
                  const std::string& index_path,
                  const std::string& wal_path);
    ~StorageEngine();

    void        set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void        remove(const std::string& key);
};