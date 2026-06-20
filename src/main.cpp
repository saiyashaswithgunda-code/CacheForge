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

    std::cout << "name: "    << engine.get("name")    << std::endl;
    std::cout << "college: " << engine.get("college") << std::endl;
    std::cout << "branch: "  << engine.get("branch")  << std::endl;
    std::cout << "age: "     << engine.get("age")     << std::endl;

    return 0;
}