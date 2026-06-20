#pragma once
#include <fstream>
#include <string>
#include "page.h"

class DiskManager {
private:
    std::string file_path;//complete data base(consisting of multiple pages) file's path
    std::fstream db_file;//cpp object to open/write/read a file
    int page_count;//noof pages in this folder/db

public:
    DiskManager(const std::string& path);
    ~DiskManager();//destructor used not to destroy any objects but to close the open files

    void writePage(Page& page);//adds a page from memory and saves it to its slot in the file
    void readPage(int page_id, Page& page);//loads a slot from the file into memory as a Page object (page-id tells the slot for disk)
    int allocateNewPage();//just gives the next empty slot 
    int getPageCount() const { return page_count; }
};