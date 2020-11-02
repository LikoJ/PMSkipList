#ifndef PMSKIPLIST_SKIPLIST_H
#define PMSKIPLIST_SKIPLIST_H
#include <string>
#include <fstream>
#include <assert.h>
#include "arena.h"
#include "random.h"

namespace pmskiplist {

struct Node {
    size_t key_len;
    int64_t key;
    size_t value_len;
    int64_t value;
    int node_height;
    int64_t next_[1];

    int64_t Next(int level) {
        return next_[level];
    }

    void SetNext(int level, int64_t x) {
        assert(level >= 0 && level < node_height);
        next_[level] = x;
    }
};

class Iterator;

class Skiplist {
    friend class Iterator;
public:
    explicit Skiplist(std::string name);
    ~Skiplist();

    bool Write(const std::string key, const std::string value);
    bool Read(const std::string key, std::string* value);
    bool Delete(const std::string key);
    Iterator* NewIterator();
private:
    Arena arena_;
    Random rnd_;
    int64_t head_;
    int now_height_;
    std::string manifest;

    int64_t NewNode(const std::string key, const std::string value, const int height);
    Node* FindGreaterOrEqual(const std::string key, Node** prev);
    bool KeyIsAfterNode(const std::string key, const Node* n);
    int RandomHeight();
};

class Iterator {
public:
    explicit Iterator(Skiplist* list);
    ~Iterator();
    bool Valid();
    void Next();
    std::string Key();
    std::string Value();
    void Seek(const std::string key);
    void SeekToFirst();
private:
    Skiplist* list_;
    Node* node_;
};
}   // pmskiplist

#endif  // PMSKIPLIST_SKIPLIST_H