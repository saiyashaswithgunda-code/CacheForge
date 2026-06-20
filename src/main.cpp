#include <iostream>
#include "disk_manager.h"
#include "page.h"
#include "hash_index.h"

void writeKV(DiskManager& dm, HashIndex& idx,
             const std::string& key, const std::string& value) {
    int pid = dm.allocateNewPage();
    Page p(pid);

    int klen = key.size(), vlen = value.size();
    int offset = 0;

    p.write(offset, (char*)&klen, sizeof(int)); offset += sizeof(int);
    p.write(offset, key.c_str(), klen);         offset += klen;
    p.write(offset, (char*)&vlen, sizeof(int)); offset += sizeof(int);
    p.write(offset, value.c_str(), vlen);

    dm.writePage(p);
    idx.insert(key, pid);  // register key → page_id in index
    idx.save();            // persist index to disk
}

std::string readKV(DiskManager& dm, HashIndex& idx, const std::string& key) {
    int pid = idx.get(key);  // lookup page_id from index
    if (pid == -1) return "KEY NOT FOUND";

    Page p(pid);
    dm.readPage(pid, p);

    int offset = 0, klen, vlen;
    p.read(offset, (char*)&klen, sizeof(int)); offset += sizeof(int);
    offset += klen; // skip the key
    p.read(offset, (char*)&vlen, sizeof(int)); offset += sizeof(int);
    std::string value(vlen, ' ');
    p.read(offset, &value[0], vlen);

    return value;
}

int main() {
    DiskManager dm("data/cacheforge.db");
    HashIndex idx("data/index.dat");

    // Write 3 key-value pairs
    writeKV(dm, idx, "name", "Arjun");
    writeKV(dm, idx, "college", "IITH");
    writeKV(dm, idx, "branch", "non-CSE");

    // Read them back
    std::cout << "name: "    << readKV(dm, idx, "name")    << std::endl;
    std::cout << "college: " << readKV(dm, idx, "college") << std::endl;
    std::cout << "branch: "  << readKV(dm, idx, "branch")  << std::endl;
    std::cout << "age: "     << readKV(dm, idx, "age")     << std::endl;

    return 0;
}