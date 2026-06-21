#include <iostream>
#include <sstream>
#include <string>
#include "storage_engine.h"
#include <chrono>

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
                      << "  EXIT               — exit CacheForge\n"
                      << "  BENCH              — run performance benchmark\n"
                      << "  COMPACT            — remove orphaned pages and shrink db\n";

        } else if (cmd == "EXIT") {
            std::cout << "Bye!" << std::endl;
            break;

        }else if (cmd == "BENCH") {
            int n = 1000;
            
            // Benchmark SET
            auto set_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < n; i++) {
                engine.set("bench_key_" + std::to_string(i),
                        "bench_val_" + std::to_string(i));
            }
            auto set_end = std::chrono::high_resolution_clock::now();

            // Benchmark GET — cache miss (fresh keys)
            auto get_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < n; i++) {
                engine.get("bench_key_" + std::to_string(i));
            }
            auto get_end = std::chrono::high_resolution_clock::now();

            // Benchmark GET — cache hit (repeat same key)
            auto hit_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < n; i++) {
                engine.get("bench_key_0"); // same key every time
            }
            auto hit_end = std::chrono::high_resolution_clock::now();

            auto set_ms = std::chrono::duration_cast<std::chrono::milliseconds>
                        (set_end - set_start).count();
            auto get_ms = std::chrono::duration_cast<std::chrono::milliseconds>
                        (get_end - get_start).count();
            auto hit_ms = std::chrono::duration_cast<std::chrono::milliseconds>
                        (hit_end - hit_start).count();

            std::cout << "\n--- Benchmark Results (" << n << " operations) ---\n";
            std::cout << "SET  : " << set_ms << "ms  ("
                    << (n * 1000 / (set_ms + 1)) << " ops/sec)\n";
            std::cout << "GET (cache miss): " << get_ms << "ms  ("
                    << (n * 1000 / (get_ms + 1)) << " ops/sec)\n";
            std::cout << "GET (cache hit) : " << hit_ms << "ms  ("
                    << (n * 1000 / (hit_ms + 1)) << " ops/sec)\n"; 
        }else if(cmd=="COMPACT"){
            engine.compact();
        }
        else {
            std::cout << "Unknown command: " << cmd
                      << ". Type HELP for commands." << std::endl;
        }
    }

    return 0;
}