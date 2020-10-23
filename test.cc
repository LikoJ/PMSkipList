#include "arena.h"
#include <iostream>
struct Node {
    int len;
    char *key;
};
int main() {
    PMSkiplist::Arena a;
    Node *n = a.Allocate(sizeof(Node));
    n->key = a.Allocate(sizeof(char) * 50);
    n->len = 50;
    strcpy(n->key, "hello, world!");
    a.Sync(n, sizeof(Node));
    a.Sync(n->key, sizeof(char) * 50);
    return 0;
}