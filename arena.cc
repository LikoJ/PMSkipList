#include "arena.h"

namespace PMSkiplist {

Arena::Arena() {
    if ((pmemaddr = pmem_map_file(PATH, PMEM_LEN, PMEM_FILE_CREATE,
                                  0666, &mapped_len, &is_pmem)) == NULL) {
        perror("pmem_map_file");
        exit(1);
    }
}

Arena::~Arena() {
    pmem_unmap(pmemaddr, mapped_len);
    pmemaddr = NULL;
}

void Arena::Sync() {
    if (is_pmem) {
        pmem_persist(pmemaddr, mapped_len);
    } else {
        pmem_msync(pmemaddr, mapped_len);
    }
}

void *Arena::Allocate() {
    return NULL;
}
}   //PMSkiplist