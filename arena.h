#ifndef PMSKIPLIST_ARENA_H
#define PMSKIPLIST_ARENA_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libpmem.h>

namespace PMSkiplist {

class Arena {
public:
    Arena();
    ~Arena();
    void Sync();
    void* Allocate();

private:
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
};

}   // PMSkiplist
#endif //PMSKIPLIST_ARENA_H