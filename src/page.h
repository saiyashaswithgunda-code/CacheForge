#pragma once//only uses this file once
#include<cstring>
#include<cstdint>
#include<stdexcept>
static const int PAGE_SIZE=4096;//each page 4kb and static keeps this variable local to this file

class Page{
public:
    char data[PAGE_SIZE];
    int page_id;
    bool is_dirty;//this flag simple tracks whether this page has been modified in memory but not saved in to disk ,
    //so dirty is true means we need to write it to the disk before discarding this
    Page(int id):page_id(id),is_dirty(false){
        memset(data,0,PAGE_SIZE);//assign 0 for the whole data of size page_size
    }
    void write(int offset,const char*src,int length){
        if(offset + length > PAGE_SIZE)throw std::out_of_range("Page overflow");

        memcpy(data+offset,src,length);//copies 'length' bytes from 2nd argument and writes onto 1st argument
        is_dirty=true;
    }
    void read(int offset,char* dest,int length){
        memcpy(dest,data+offset,length);
    }
};