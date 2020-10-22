#include "arena.h"
#include <iostream>

int main() {
    PMSkiplist::Arena a;
    char *str = a.Allocate(50);
    strcpy(str, "hello, world!");
    a.Sync();
    return 0;
}