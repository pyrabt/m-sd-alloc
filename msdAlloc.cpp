#include "msdAlloc.h"
#include <sys/mman.h>

MSDAlloc::MSDAlloc() {
}

MSDAlloc::~MSDAlloc() {
    auto table = allocations.getTable();
    for (int i = 0; i < allocations.getCapacity_(); ++i) {
        if (table[i].first != nullptr) {
            munmap(table[i].first, table[i].second);
        }
    }
}

void *MSDAlloc::allocate(size_t bytesToAllocate) {
    int pageRatio = bytesToAllocate / 4096;
    size_t normalizedAllocation = (pageRatio * 4096) + 4096;
    void *ptr = mmap(nullptr, normalizedAllocation, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    allocations.insertNode(ptr, normalizedAllocation);
    return ptr;
}

void MSDAlloc::deallocate(void *pointer) {
    size_t bytesAllocated = allocations.deleteNode(pointer);
    munmap(pointer, bytesAllocated);
}
