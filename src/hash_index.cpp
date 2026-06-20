#include "hash_index.h"
#include<iostream>
#include<fstream>

HashIndex::HashIndex(const std::string& path):index_file_path(path){
    for(int i=0;i<BUCKET_COUNT;i++){
        buckets[i]=nullptr;//space is given for buckets but not initialised to smtg so we give nullptr
    }
    load();//load any previous data from disk if it is present
}
HashIndex::~HashIndex(){
    for(int i=0;i<BUCKET_COUNT;i++){
        IndexNode*curr=buckets[i];
        while(curr!=nullptr){
            IndexNode*next=curr->next;
            delete curr;//delete the elements that are created in free store
            curr=next;
        }
    }
}
//private function definition
int HashIndex::hash(const std::string& key) const{
    int hash_val=0;
    for(char c:key){
        hash_val=(hash_val*31+c)%BUCKET_COUNT;
    }
    return hash_val;
}
void HashIndex::insert(const std::string&key,int page_id) {
    int bucket=hash(key);
    IndexNode* curr=buckets[bucket];
    while(curr!=nullptr){
        if(curr->key==key){
            curr->page_id=page_id;
            return;
        }
        curr=curr->next;
    }
    IndexNode*new_node=new IndexNode(key,page_id);
    new_node->next=buckets[bucket];
    buckets[bucket]=new_node;
}
int HashIndex::get(const std::string& key) const{
    int bucket=hash(key);
    IndexNode*curr=buckets[bucket];
    while(curr!=nullptr){
        if(curr->key==key){
            return curr->page_id;
        }
        curr=curr->next;
    }
    return -1;
}
void HashIndex::remove(const std::string& key) {
    int bucket = hash(key);
    IndexNode* curr = buckets[bucket];
    IndexNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) {
                buckets[bucket] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
void HashIndex::save()const{
    std::ofstream file(index_file_path);
    for(int i=0;i<BUCKET_COUNT;i++){
        IndexNode*curr=buckets[i];
        while(curr!=nullptr){
            file<<curr->key<<" "<<curr->page_id<<"\n";//we doesnt store group of hashindex wise bcz later inserting in this 
                                                        // same order however creates same hsash structure
            curr=curr->next;
        }
    }
}
void HashIndex::load(){
    std::ifstream file(index_file_path);
    if(!file.is_open())return;
    std::string key;
    int page_id;
    while(file >> key >> page_id){
        insert(key,page_id);
    }
}