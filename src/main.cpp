#include <iostream>
#include "storage_engine.h"

int main() {
    StorageEngine engine(
        "data/cacheforge.db",
        "data/index.dat",
        "data/wal.log"
    );

    engine.set("name",    "Arjun");
    engine.set("college", "IITH");
    engine.set("branch",  "non-CSE");
    engine.set("city",    "Hyderabad"); // this will cause eviction - capacity is 3

    std::cout << "\n--- GET operations ---\n";
    std::cout << "name: "    << engine.get("name")    << std::endl;
    std::cout << "college: " << engine.get("college") << std::endl;
    std::cout << "name: "    << engine.get("name")    << std::endl; // cache hit
    std::cout << "branch: "  << engine.get("branch")  << std::endl;

    return 0;
}