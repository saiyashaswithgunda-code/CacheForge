#include <iostream>
#include <sstream>
#include <string>
#include "storage_engine.h"

int main() {
    StorageEngine engine(
        "data/cacheforge.db",
        "data/index.dat",
        "data/wal.log"
    );

    std::string line;
    std::cout << "CacheForge v1.0 — type HELP for commands\n";

    while (true) {
        std::cout << "\nCacheForge> ";
        std::getline(std::cin, line);

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd, key, value;

        ss >> cmd;

        // Convert command to uppercase
        for (char& c : cmd) c = toupper(c);

        if (cmd == "SET") {
            ss >> key >> value;
            if (key.empty() || value.empty()) {
                std::cout << "Usage: SET <key> <value>" << std::endl;
                continue;
            }
            engine.set(key, value);
            std::cout << "OK" << std::endl;

        } else if (cmd == "GET") {
            ss >> key;
            if (key.empty()) {
                std::cout << "Usage: GET <key>" << std::endl;
                continue;
            }
            std::cout << engine.get(key) << std::endl;

        } else if (cmd == "DELETE") {
            ss >> key;
            if (key.empty()) {
                std::cout << "Usage: DELETE <key>" << std::endl;
                continue;
            }
            engine.remove(key);
            std::cout << "OK" << std::endl;

        } else if (cmd == "HELP") {
            std::cout << "Commands:\n"
                      << "  SET <key> <value>  — store a key-value pair\n"
                      << "  GET <key>          — retrieve a value\n"
                      << "  DELETE <key>       — delete a key\n"
                      << "  EXIT               — exit CacheForge\n";

        } else if (cmd == "EXIT") {
            std::cout << "Bye!" << std::endl;
            break;

        } else {
            std::cout << "Unknown command: " << cmd
                      << ". Type HELP for commands." << std::endl;
        }
    }

    return 0;
}