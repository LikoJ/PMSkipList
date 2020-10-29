#include "skiplist.h"

namespace pmskiplist {

static const int max_height = 32;   // The max height of pmskiplist

Skiplist::Skiplist(): rnd_(0xdeadbeef),
                      head_(NewNode("", "", max_height)),
                      now_height_(1) {}

Skiplist::~Skiplist() {}

Node* Skiplist::NewNode(const std::string key, const std::string value, const int height) {
    size_t tmp = sizeof(Node) + sizeof(Node*) * (height - 1);
    Node *n = (Node*)arena_.Allocate(tmp);
    n->key_len = key.length();
    if (n->key_len == 0) {
        n->key = NULL;
    } else {
        n->key = (char*)arena_.Allocate(n->key_len);
        memcpy(n->key, key.data(), n->key_len);
    }
    n->value_len = value.length();
    if (n->value_len == 0) {
        n->value = NULL;
    } else {
        n->value = (char*)arena_.Allocate(n->value_len);
        memcpy(n->value, value.data(), n->value_len);
    }

    n->node_height = height;
    for (int i = 0; i < n->node_height; i++) {
        n->SetNext(i, NULL);
    }

    arena_.Sync(n->key, n->key_len);
    arena_.Sync(n->value, n->value_len);
    arena_.Sync(n, tmp);
    return n;
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

bool Skiplist::KeyIsAfterNode(const std::string key, const Node* n) {
    if (n == nullptr) {
        return false;
    }
    std::string tmp(n->key, n->key_len);
    if (key > tmp) {
        return true;
    } else {
        return false;
    }
}

Node* Skiplist::FindGreaterOrEqual(const std::string key, Node** prev) {
    Node *x = head_;
    int level = now_height_ - 1;
    while (true) {
        Node *next = x->Next(level);
        if (KeyIsAfterNode(key, next)) {
            // Keep searching in this list
            x = next;
        } else {
            if (prev != nullptr) prev[level] = x;
            if (level == 0) {
                return next;
            } else {
                // Switch to next list
                level--;
            }
        }
    }
}

bool Skiplist::Write(const std::string key, const std::string value) {
    Node *prev[max_height];
    Node *x = FindGreaterOrEqual(key, prev);    // if x->key == key, update needs delete; else insert

    // Insert
    int height = RandomHeight();
    Node *n = NewNode(key, value, height);

    if (height > now_height_) {
        for (int i = now_height_; i < height; i++) {
            prev[i] = head_;
        }
        now_height_ = height;
    }

    for (int i = 0; i < height; i++) {
        n->SetNext(i, prev[i]->Next(i));
        prev[i]->SetNext(i, n);
        prev[i] = n;
    }

    if (x != NULL) {
        std::string keystr(x->key, x->key_len);
        if (keystr == key) {
            //Delete
            for (int i = 0; i < x->node_height; i++) {
                prev[i]->SetNext(i, x->Next(i));
            }
        }
    }
    return true;
}

bool Skiplist::Read(const std::string key, std::string *value) {
    Node *x = FindGreaterOrEqual(key, NULL);
    if (x == NULL) {
        value = NULL;
        return false;
    }
    std::string xkey(x->key, x->key_len);
    std::string xvalue(x->value, x->value_len);

    if (xkey == key) {
        *value = xvalue;
        return true;
    } else {
        value = NULL;
        return false;
    }
}

bool Skiplist::Delete(const std::string key) {
    Node *prev[max_height];
    Node *x = FindGreaterOrEqual(key, prev);
    if (x == NULL) {
        return false;
    }
    std::string xkey(x->key, x->key_len);
    if (xkey == key) {
        for (int i = 0; i < x->node_height; i++) {
            prev[i]->SetNext(i, x->Next(i));
        }
        return true;
    } else {
        return false;
    }
}

Iterator::Iterator(Skiplist *list): list_(list),
                                    node_(list_->head_->Next(0)) {}

Iterator::~Iterator() {}

bool Iterator::Valid() {
    return node_ != NULL;
}

void Iterator::Next() {
    assert(Valid());
    node_ = node_->Next(0);
}

std::string Iterator::Key() {
    assert(Valid());
    std::string key(node_->key, node_->key_len);
    return key;
}

std::string Iterator::Value() {
    assert(Valid());
    std::string value(node_->value, node_->value_len);
    return value;
}

void Iterator::Seek(const std::string key) {
    node_ = list_->FindGreaterOrEqual(key, NULL);
}

}   // pmskiplist