#include "storage_engine.h"
#include "wal.h"
#include <iostream>

StorageEngine::StorageEngine(const std::string& db_path,
                             const std::string& index_path,
                             const std::string& wal_path)
    : disk_manager(db_path), hash_index(index_path),buffer_pool(3,disk_manager){
    wal = new WAL(wal_path);
    wal->replay(*this); // replay incomplete operations on startup
}

StorageEngine::~StorageEngine() {
    delete wal;
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

    disk_manager.writePage(p);
    buffer_pool.pinPage(pid,p);
    hash_index.insert(key, pid);
    hash_index.save();

    wal->logDone("SET", key, value);
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

    hash_index.remove(key);
    hash_index.save();

    wal->logDone("DELETE", key, "");
}