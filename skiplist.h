#ifndef PMSKIPLIST_SKIPLIST_H
#define PMSKIPLIST_SKIPLIST_H
#include <string>
#include <assert.h>
#include "arena.h"
#include "random.h"

namespace PMSkiplist {

struct Node {
    size_t key_len;
    char *key;
    size_t value_len;
    char *value;
    int node_height;
    Node* next_[1];

    Node* Next(int level) {
        return next_[level];
    }

    void SetNext(int level, Node* x) {
        assert(level >= 0 && level < node_height);
        next_[level] = x;
    }
};

class Skiplist {
public:
    Skiplist();
    ~Skiplist();

    bool Write(const std::string key, const std::string value);
    bool Read(const std::string key, std::string* value);
    bool ScanStart(const std::string key);
    bool ScanNext(const std::string key);
private:
    Arena arena_;
    Random rnd_;
    Node* head_;
    Node* scan_tmp_;
    int now_height_;

    Node* NewNode(const std::string key, const std::string value, const int height);
    Node* FindGreaterOrEqual(const std::string key, Node** prev);
    bool KeyIsAfterNode(const std::string key, const Node* n);
    int RandomHeight();
};
}   // PMSkiplist

#endif  // PMSKIPLIST_SKIPLIST_H