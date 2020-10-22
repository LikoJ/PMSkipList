#include "skiplist.h"

namespace PMSkiplist {

static const int max_height = 32;   // The max height of PMSkiplist

Skiplist::SkipList(): head_(NewNode(0, NULL, 0, NULL, max_height)),
                      rnd_(0xdeadbeef),
                      scan_tmp_(NULL) {}

Node* Skiplist::NewNode(const int key_len, const std::string key, const int value_len, const std::string value, int height) {
    char * node_memory = arena_.Allocate(sizeof(Node) + sizeof(Node*) * (height - 1));
    return new (node_memory) Node(key_len, &key, value_len, &value, height);
}
int Skiplist::RandomHeight() {
    static const unsigned int kBranching = 4;
    int height = 1;
    while (height < max_height && ((rnd_.Next() % kBranching) == 0)) {
        height++;
    }
    assert(height > 0);
    assert(height <= max_height);
    return height;
}
}   // PMSkiplist