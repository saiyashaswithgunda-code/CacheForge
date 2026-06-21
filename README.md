# CacheForge
A LevelDB inspired persistent key-value storage engine built from scratch in C++ without any external libraries.

Built to understand storage engine internals by implementing concepts such as write-ahead logging, page-based storage, buffer pool management, indexing, and compaction.

---

## Architecture

```text
+-----------+
| CLI / REPL|
+-----------+
      |
      v
+-------------------+
|   StorageEngine   |
+-------------------+
   /      |      \
  v       v       v
HashIndex WAL BufferPool(LRU)
    \      |      /
     \     |     /
      v    v    v
     DiskManager
          |
          v
  cacheforge.db

---

## Features

- **Persistent storage** — data survives program restarts via binary page files
- **Hash Index** — custom persistent hash index using separate chaining for collision resolution
- **Write-Ahead Log (WAL)** — crash recovery via PENDING/DONE log replay
- **LRU Buffer Pool** — lazy disk writes with dirty page writeback on eviction
- **Log Compaction** — removes orphaned pages, shrinks database to active data only
- **Interactive CLI** — REPL shell with SET, GET, DELETE, BENCH, COMPACT commands

---

## Performance

Benchmarked on 1000 operations:

| Operation | Throughput |
|---|---|
| SET | ~220 ops/sec |
| GET (cache miss) | ~200,000 ops/sec |
| GET (cache hit) | ~1,000,000 ops/sec |

Cache hits are **~2000x faster** than writes — demonstrating buffer pool effectiveness.

---

## Concepts Demonstrated

| Domain | Concepts |
|---|---|
| C++ / OOP | 6 classes, encapsulation, composition, RAII, smart pointers |
| DSA | Hash map with chaining, doubly linked list LRU cache |
| DBMS | WAL, buffer pool, crash recovery, compaction, indexing |
| OS | Binary file I/O, page management, random access, memory disk hierarchy |

---

## Key Learnings

- Storage engines separate logical records from physical page layout.
- WAL guarantees recoverability after crashes.
- Buffer pools trade memory for reduced disk I/O.
- Compaction reclaims space from obsolete records.
- Indexes act as the source of truth for locating live data.

---

## Build & Run

**Requirements**
- C++17 compiler (g++ / MinGW)
- CMake 3.15+

**Build**
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"   # Windows
cmake ..                         # Linux/Mac
cmake --build .
cd ..
```

**Run**
```bash
.\build\cacheforge.exe   # Windows
./build/cacheforge       # Linux/Mac
```

---

## CLI Commands
CacheForge> SET <key> <value>   store a key-value pair

CacheForge> GET <key>           retrieve a value

CacheForge> DELETE <key>        delete a key

CacheForge> BENCH               run performance benchmark

CacheForge> COMPACT             remove orphaned pages, shrink db

CacheForge> HELP                show all commands

CacheForge> EXIT                exit CacheForge

---

## Project Structure
CacheForge/

├── src/

│   ├── page.h / page.cpp               4KB memory page representation

│   ├── disk_manager.h / .cpp           binary file I/O, page read/write

│   ├── hash_index.h / .cpp             persistent hash index with chaining

│   ├── wal.h / .cpp                    write-ahead log, crash recovery

│   ├── buffer_pool.h / .cpp            LRU cache, dirty page writeback

│   ├── storage_engine.h / .cpp         unified interface across all layers

│   └── main.cpp                        interactive CLI/REPL

├── data/

│   ├── cacheforge.db                   binary page storage

│   ├── index.dat                       persisted hash index

│   └── wal.log                         write-ahead log

└── CMakeLists.txt
---

## How WAL Crash Recovery Works
Normal flow:

SET name Arjun

→ log PENDING to wal.log

→ write to buffer pool

→ update hash index

→ log DONE to wal.log
On crash (PENDING with no DONE):

→ startup detects incomplete operation

→ replays SET name Arjun automatically

→ data recovered, no corruption

---

## Inspired By
- [CMU 15-445 Database Systems](https://15445.courses.cs.cmu.edu)
- [LevelDB](https://github.com/google/leveldb)
- [Let's Build a Simple Database](https://cstack.github.io/db_tutorial)