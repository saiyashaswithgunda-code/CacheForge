#pragma once
#include <string>
#include <fstream>

enum class WALStatus {
    PENDING,
    DONE
};

struct WALEntry {
    std::string operation;  // SET or DELETE
    std::string key;
    std::string value;
    WALStatus status;
};

class WAL {
private:
    std::string log_path;
    std::ofstream log_file;

public:
    WAL(const std::string& path);
    ~WAL();

    void logPending(const std::string& op,
                    const std::string& key,
                    const std::string& value);
    void logDone(const std::string& op,
                 const std::string& key,
                 const std::string& value);
    void replay(class StorageEngine& engine);
    void truncate();
};