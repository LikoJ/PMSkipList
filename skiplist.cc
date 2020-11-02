#include "skiplist.h"

namespace pmskiplist {

static const int max_height = 32;   // The max height of pmskiplist
static const std::string path = "/mnt/persist-memory/pmem_fs_lhd/";

Skiplist::Skiplist(std::string name): rnd_(0xdeadbeef),
                                      manifest(path + name) {
    head_ = NewNode("", "", max_height);
    now_height_ = 1;
    /*std::ifstream ifs(manifest);
    if (!ifs.is_open()) {
        // Create new pmskiplist
        head_ = NewNode("", "", max_height);
        now_height_ = 1;
    } else {
        arena_.Recover(ifs);
        ifs >> head_;
        ifs >> now_height_;
        ifs.close();
    }*/
}

Skiplist::~Skiplist() {
    /*std::ofstream ofs(manifest);
    arena_.Save(ofs);
    ofs << head_;
    ofs << now_height_;*/
}

Node* Skiplist::NewNode(const std::string key, const std::string value, const int height) {
    size_t tmp = sizeof(Node) + sizeof(Node*) * (height - 1);
    void *node_offset, *key_offset, *value_offset;
    Node *n = (Node*)arena_.Allocate(tmp, node_offset);

    n->key_len = key.length();
    if (n->key_len == 0) {
        n->key = NULL;
    } else {
        n->key = (char*)arena_.Allocate(n->key_len, key_offset);
        memcpy(n->key, key.data(), n->key_len);
        arena_.Sync(n->key, n->key_len);
        n->key = key_offset;
    }

    n->value_len = value.length();
    if (n->value_len == 0) {
        n->value = NULL;
    } else {
        n->value = (char*)arena_.Allocate(n->value_len, value_offset);
        memcpy(n->value, value.data(), n->value_len);
        arena_.Sync(n->value, n->value_len);
        n->value = value_offset;
    }

    n->node_height = height;
    for (int i = 0; i < n->node_height; i++) {
        n->SetNext(i, NULL);
    }

    arena_.Sync(n, tmp);
    return (Node*)node_offset;
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
    std::string tmp((char*)arena_.Translate((void*)n->key), n->key_len);
    if (key > tmp) {
        return true;
    } else {
        return false;
    }
}

Node* Skiplist::FindGreaterOrEqual(const std::string key, Node** prev) {
    Node *x = (Node*)arena_.Translate((void*)head_);
    int level = now_height_ - 1;
    while (true) {
        Node *next;
        if (x->Next(level) == NULL) {
            next = NULL;
        } else {
            next = (Node*)arena_.Translate((void*)x->Next(level));
        }
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
    Node *offset = NewNode(key, value, height);
    Node *n = (Node*)arena_.Translate((void *)offset);

    if (height > now_height_) {
        for (int i = now_height_; i < height; i++) {
            prev[i] = head_;
        }
        now_height_ = height;
    }

    for (int i = 0; i < height; i++) {
        n->SetNext(i, prev[i]->Next(i));
        prev[i]->SetNext(i, offset);
        prev[i] = n;
    }

    if (x != NULL) {
        std::string keystr((char*)arena_.Translate((void*)x->key), x->key_len);
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
    std::string xkey((char*)arena_.Translate((void*)x->key), x->key_len);
    std::string xvalue((char*)arena_.Translate((void*)x->value), x->value_len);

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
    std::string xkey((char*)arena_.Translate((void*)x->key), x->key_len);
    if (xkey == key) {
        for (int i = 0; i < x->node_height; i++) {
            prev[i]->SetNext(i, x->Next(i));
        }
        return true;
    } else {
        return false;
    }
}

Iterator* Skiplist::NewIterator() {
    return new Iterator(this);
}

Iterator::Iterator(Skiplist *list): list_(list),
                                    node_(NULL) {}

Iterator::~Iterator() {}

bool Iterator::Valid() {
    return node_ != NULL;
}

void Iterator::Next() {
    assert(Valid());
    node_ = (Node*)list_->arena_.Translate((void*)node_->Next(0));
}

std::string Iterator::Key() {
    assert(Valid());
    std::string key(list_->arena_.Translate((void*)node_->key), node_->key_len);
    return key;
}

std::string Iterator::Value() {
    assert(Valid());
    std::string value(list_->arena_.Translate((void*)node_->value), node_->value_len);
    return value;
}

void Iterator::Seek(const std::string key) {
    node_ = list_->FindGreaterOrEqual(key, NULL);
}

void Iterator::SeekToFirst() {
    node_ = (Node*)list_->arena_.Translate((void*)list_->head_->Next(0));
}

}   // pmskiplist