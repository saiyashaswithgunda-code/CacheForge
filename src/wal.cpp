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

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string op, key, value, status;

        std::getline(ss, op,     '|');
        std::getline(ss, key,    '|');
        std::getline(ss, value,  '|');
        std::getline(ss, status, '|');

        // Replay every PENDING entry - rebuilds state since last checkpoint
        if (status == "PENDING") {
            std::cout << "Replaying: " << op << " " << key << std::endl;
            if (op == "SET") {
                engine.set(key, value);
            } else if (op == "DELETE") {
                engine.remove(key);
            }
        }
    }
}
void WAL::truncate() {
    log_file.close();
    log_file.open(log_path, std::ios::out | std::ios::trunc); // clear file
    log_file.close();
    log_file.open(log_path, std::ios::app); // reopen in append mode
}