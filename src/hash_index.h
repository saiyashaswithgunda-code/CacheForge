#pragma once
#include<string>
static const int BUCKET_COUNT=16;//compile time computed value so no error in using it for array declaration

struct IndexNode{
    std::string key;
    int page_id;
    IndexNode* next;
    IndexNode(const std::string& k,int pid):key(k),page_id(pid),next(nullptr){ }
};

class HashIndex{
private:
    IndexNode* buckets[BUCKET_COUNT];//16 no of buckets each having its own chain  
    std::string index_file_path;//we store this whole keys indexes in disk aftering saving to prevent loss of indexes when turned off
    int hash(const std::string& k) const;//a hash function that maps from string to ints
public:
    HashIndex(const std::string& path);
    ~HashIndex();

    HashIndex& operator=(const HashIndex& other) {
        // clean up existing nodes
        for (int i = 0; i < BUCKET_COUNT; i++) {
            IndexNode* curr = buckets[i];
            while (curr) {
                IndexNode* next = curr->next;
                delete curr;
                curr = next;
            }
            buckets[i] = nullptr;
        }
        index_file_path = other.index_file_path;
        load();
        return *this;
    }

    void insert(const std::string& key,int page_id);
    int get(const std::string& key)const;
    void remove(const std::string& key);

    void save() const;
    void load();
};