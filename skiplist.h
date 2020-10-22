#ifndef PMSKIPLIST_SKIPLIST_H
#define PMSKIPLIST_SKIPLIST_H
#include <string>
#include <assert.h>
#include "arena.h"
#include "random.h"

namespace PMSkiplist {

struct Node {
    size_t key_len;
    std::string *key;
    size_t value_len;
    std::string *value;
    int node_height;
    Node* next_[1];

    explicit Node(const size_t kl, 
                  const std::string* k, 
                  const size_t vl, 
                  const std::string* v, 
                  const int h): key_len(kl),
                                key(k),
                                value_len(vl),
                                value(v),
                                node_height(h) {}

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
    SkipList();
    ~SkipList();

    bool Write(const std::string key, const std::string value);
    bool Read(const std::string key, std::string* value);
    bool ScanStart(const std::string key);
    bool ScanNext(const std::string key);
private:
    Arena arena_;
    Random rnd_;
    Node* head_;
    Node* scan_tmp_;

    Node* NewNode(const size_t key_len, const std::string key, const size_t value_len, const std::string value, int height);
    int RandomHeight();
};
}   // PMSkiplist

#endif  // PMSKIPLIST_SKIPLIST_H