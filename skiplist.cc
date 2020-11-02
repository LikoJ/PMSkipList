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

int64_t Skiplist::NewNode(const std::string key, const std::string value, const int height) {
    size_t tmp = sizeof(Node) + sizeof(Node*) * (height - 1);
    int64_t node_offset, key_offset, value_offset;
    Node *n = (Node*)arena_.Allocate(tmp, node_offset);

    n->key_len = key.length();
    if (n->key_len == 0) {
        n->key = -1;
    } else {
        n->key = (int64_t)arena_.Allocate(n->key_len, key_offset);
        memcpy((char *)n->key, key.data(), n->key_len);
        arena_.Sync((void*)n->key, n->key_len);
        n->key = key_offset;
    }

    n->value_len = value.length();
    if (n->value_len == 0) {
        n->value = -1;
    } else {
        n->value = (int64_t)arena_.Allocate(n->value_len, value_offset);
        memcpy((char *)n->value, value.data(), n->value_len);
        arena_.Sync((void*)n->value, n->value_len);
        n->value = value_offset;
    }

    n->node_height = height;
    for (int i = 0; i < n->node_height; i++) {
        n->SetNext(i, -1);
    }

    arena_.Sync(n, tmp);
    return node_offset;
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
    std::string tmp((char*)arena_.Translate(n->key), n->key_len);
    if (key > tmp) {
        return true;
    } else {
        return false;
    }
}

Node* Skiplist::FindGreaterOrEqual(const std::string key, Node** prev) {
    Node *x = (Node*)arena_.Translate(head_);
    int level = now_height_ - 1;
    while (true) {
        Node *next;
        if (x->Next(level) == -1) {
            next = NULL;
        } else {
            next = (Node*)arena_.Translate(x->Next(level));
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
    int64_t offset = NewNode(key, value, height);
    Node *n = (Node*)arena_.Translate(offset);

    if (height > now_height_) {
        for (int i = now_height_; i < height; i++) {
            prev[i] = (Node*)arena_.Translate(head_);
        }
        now_height_ = height;
    }

    for (int i = 0; i < height; i++) {
        n->SetNext(i, prev[i]->Next(i));
        prev[i]->SetNext(i, offset);
        prev[i] = n;
    }

    if (x != NULL) {
        std::string keystr((char*)arena_.Translate(x->key), x->key_len);
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
    std::string xkey((char*)arena_.Translate(x->key), x->key_len);
    std::string xvalue((char*)arena_.Translate(x->value), x->value_len);

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
    std::string xkey((char*)arena_.Translate(x->key), x->key_len);
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
    node_ = (Node*)list_->arena_.Translate(node_->Next(0));
}

std::string Iterator::Key() {
    assert(Valid());
    std::string key((char*)list_->arena_.Translate(node_->key), node_->key_len);
    return key;
}

std::string Iterator::Value() {
    assert(Valid());
    std::string value((char*)list_->arena_.Translate(node_->value), node_->value_len);
    return value;
}

void Iterator::Seek(const std::string key) {
    node_ = list_->FindGreaterOrEqual(key, NULL);
}

void Iterator::SeekToFirst() {
    node_ = (Node*)list_->arena_.Translate(list_->head_);
    Next();
}

}   // pmskiplist