#include "wal.h"
#include "storage_engine.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<set>
#include<vector>

WAL::WAL(const std::string&path):log_path(path){
    log_file.open(path,std::ios::app);//this is opening a filestream in append mode we can only append to it ntg else
                                      //writing smtg like this is an atomic operation
}
WAL::~WAL(){
    if(log_file.is_open())log_file.close();
}
void WAL::logPending(const std::string&op,const std::string&key,const std::string&value){
    log_file<<op<<"|"<<key<<"|"<<value<<"|PENDING\n";
    log_file.flush();//force it to disk immediately - very important bcz we dont want to lose the data
}
void WAL::logDone(const std::string&op,const std::string&key,const std::string&value){
    log_file<<op<<"|"<<key<<"|"<<value<<"|DONE\n";
    log_file.flush();//force it to disk immediately 
}
void WAL::replay(StorageEngine& engine) {
    std::ifstream file(log_path);
    if (!file.is_open()) return;

    std::vector<WALEntry> entries;
    std::string line;

    // Pass 1 - read all entries in order
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string op, key, value, status;

        std::getline(ss, op,     '|');
        std::getline(ss, key,    '|');
        std::getline(ss, value,  '|');
        std::getline(ss, status, '|');

        WALEntry entry;
        entry.operation = op;
        entry.key       = key;
        entry.value     = value;
        entry.status    = (status == "DONE") ? WALStatus::DONE
                                             : WALStatus::PENDING;
        entries.push_back(entry);
    }

    // Pass 2 - for each PENDING, check if the VERY NEXT matching entry is DONE
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i].status == WALStatus::PENDING) {
            bool completed = false;

            // Look for matching DONE immediately after
            if (i + 1 < entries.size()) {
                WALEntry& next = entries[i + 1];
                if (next.status    == WALStatus::DONE &&
                    next.operation == entries[i].operation &&
                    next.key       == entries[i].key &&
                    next.value     == entries[i].value) {
                    completed = true;
                }
            }
            if (!completed) {
                std::cout << "Replaying: "
                          << entries[i].operation << " "
                          << entries[i].key << std::endl;
                if (entries[i].operation == "SET") {
                    engine.set(entries[i].key, entries[i].value);
                } else if (entries[i].operation == "DELETE") {
                    engine.remove(entries[i].key);
                }
            }
        }
    }
}
