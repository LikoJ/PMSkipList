#include "arena.h"
#include <iostream>

int main() {
    PMSkiplist::Arena a;
    char *str = (char*)a.Allocate(50);
    strcpy(str, "hello, world!");
    a.Sync(str, 50);

    int *n = (int *)a.Allocate(sizeof(int));
    *n = 20;
    a.Sync(n, sizeof(int));
    return 0;
}