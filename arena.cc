#include "arena.h"

namespace PMSkiplist {
    static const size_t pmem_len = 80L * 1024 * 1024 * 1024;
    static const std::string path = "/mnt/persist-memory/pmem_fs_lhd/test.pool";

Arena::Arena(): used(0) {
    if ((pmemaddr = pmem_map_file(path.c_str(), pmem_len, PMEM_FILE_CREATE,
                                  0666, &mapped_len, &is_pmem)) == NULL) {
        perror("pmem_map_file");
        exit(1);
    }
    free = pmem_len;
}

Arena::~Arena() {
    pmem_unmap(pmemaddr, mapped_len);
    pmemaddr = NULL;
}

void *Arena::Allocate(size_t bytes) {
    void *result = NULL;
    if (free >= bytes) {
        free -= bytes;
        result = pmemaddr + used;
        used += bytes;
    }
    return result;
}

void Arena::Sync(void *start, size_t len) {
    if (is_pmem) {
        pmem_persist(start, len);
    } else {
        pmem_msync(start, len);
    }
}

}   // PMSkiplist