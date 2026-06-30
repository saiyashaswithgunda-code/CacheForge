#include "storage_engine.h"
#include "wal.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>

StorageEngine::StorageEngine(const std::string& db_path,
                             const std::string& index_path,
                             const std::string& wal_path)
    : disk_manager(db_path), hash_index(index_path),buffer_pool(3,disk_manager),op_count_since_checkpoint(0){
    wal = new WAL(wal_path);
    wal->replay(*this); // replay incomplete operations on startup
}

StorageEngine::~StorageEngine() {
    checkpoint();//checkpoint on clean shutdown
    buffer_pool.flushAll();
    delete wal;
}
void StorageEngine::checkpoint() {
    std::cout << "[Checkpoint] Saving index snapshot and truncating WAL...\n";
    hash_index.save();   // one full write  but only every N ops now
    wal->truncate();      // clear WAL since everything is now safely in index.dat
    op_count_since_checkpoint = 0;
}

void StorageEngine::set(const std::string& key, const std::string& value) {
    wal->logPending("SET", key, value);

    // Write to disk
    int pid = disk_manager.allocateNewPage();
    Page p(pid);

    int klen = key.size(), vlen = value.size();
    int offset = 0;

    p.write(offset, (char*)&klen, sizeof(int)); offset += sizeof(int);
    p.write(offset, key.c_str(), klen);         offset += klen;
    p.write(offset, (char*)&vlen, sizeof(int)); offset += sizeof(int);
    p.write(offset, value.c_str(), vlen);

    buffer_pool.pinPage(pid,p);
    buffer_pool.markDirty(pid);
    hash_index.insert(key, pid);
    // hash_index.save();//not every operation


    wal->logDone("SET", key, value);

    op_count_since_checkpoint++;
    if (op_count_since_checkpoint >= CHECKPOINT_INTERVAL) {//checkpoint saved only after writing DONE in wal
        checkpoint();
    }
}

std::string StorageEngine::get(const std::string& key) {
    int pid = hash_index.get(key);
    if (pid == -1) return "KEY NOT FOUND";

    Page& p=buffer_pool.getPage(pid);

    int offset = 0, klen, vlen;
    p.read(offset, (char*)&klen, sizeof(int)); offset += sizeof(int);
    offset += klen;
    p.read(offset, (char*)&vlen, sizeof(int)); offset += sizeof(int);
    std::string value(vlen, ' ');
    p.read(offset, &value[0], vlen);

    return value;
}

void StorageEngine::remove(const std::string& key) {
    wal->logPending("DELETE", key, "");
    
    int pid = hash_index.get(key);
    if (pid != -1) {
        buffer_pool.evictPage(pid);//remove from buffer pool also if exists
    }
    
    hash_index.remove(key);
    // hash_index.save();


    wal->logDone("DELETE", key, "");
    op_count_since_checkpoint++;
    if (op_count_since_checkpoint >= CHECKPOINT_INTERVAL) {
        checkpoint();
    }
}
void StorageEngine::compact() {
    std::cout << "Starting compaction..." << std::endl;
    checkpoint();//ensuring data is uptodate by check pointing and saving all indices

    // Step 1 - get all valid keys and their values from index
    std::vector<std::pair<std::string, std::string>> live_data;

    // read index.dat to get all live keys
    std::ifstream index_file("data/index.dat");
    std::string key;
    int old_pid;

    while (index_file >> key >> old_pid) {
        // read value from disk for this key
        Page p(old_pid);
        disk_manager.readPage(old_pid, p);

        int offset = 0, klen, vlen;
        p.read(offset, (char*)&klen, sizeof(int)); offset += sizeof(int);
        offset += klen;
        p.read(offset, (char*)&vlen, sizeof(int)); offset += sizeof(int);
        std::string value(vlen, ' ');
        p.read(offset, &value[0], vlen);

        live_data.push_back({key, value});
    }

    int live_count = live_data.size();
    std::cout << "Live keys found: " << live_count << std::endl;

    // Step 2 - delete old db and index files
    index_file.close();
    // index_file.close();
    disk_manager.closeFile();
    std::remove("data/cacheforge.db");
    std::remove("data/index.dat");
    std::remove("data/wal.log");

    // Step 3 - reopen disk manager with fresh file
    disk_manager = DiskManager("data/cacheforge.db");
    hash_index   = HashIndex("data/index.dat");
    delete wal;
    wal = new WAL("data/wal.log");

    // Step 4 - rewrite only live data
    for (auto& [k, v] : live_data) {
        set(k, v);
    }

    buffer_pool.flushAll();
    checkpoint();//updating all indices again
    std::cout << "Compaction done! " << live_count
              << " keys rewritten." << std::endl;
}