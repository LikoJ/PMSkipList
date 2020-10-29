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
#include <cstdint>
#include <string>

namespace pmskiplist {

class Arena {
public:
    Arena();
    ~Arena();
    void Sync(void *start, size_t len);
    void* Allocate(size_t bytes);

private:
    void *pmemaddr;
    size_t mapped_len;
    int is_pmem;
    size_t used;
    size_t free;
};

}   // pmskiplist
#endif // PMSKIPLIST_ARENA_H