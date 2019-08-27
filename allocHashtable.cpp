//
// Created by William Marringa on 2019-02-28.
//

#include "allocHashtable.h"
#include <sys/mman.h>

typedef __uint64_t pointer_t;

AllocHashtable::AllocHashtable() {
    capacity_ = 256; // sizeOf(pair) = 16 bytes, 1 page -> 4096 bytes, 4096/16 = 256 bytes per page
    size_ = 0;
    table = static_cast<std::pair<void *, size_t>*>(mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0));
    for (int x = 0; x < capacity_; ++x) {
        table[x].first = nullptr;
        table[x].second = 0;
    }
}

AllocHashtable::~AllocHashtable() {
    munmap(table, capacity_);
}

int AllocHashtable::hashCode(void *address) {
    auto hash = (pointer_t)address;
    hash = hash << 12;
    return static_cast<int>((hash >> 12) % capacity_);
}

void AllocHashtable::insertNode(void *address, size_t allocationSize) {
    if ((capacity_ - size_) <= (capacity_ * 0.25)) {
        grow();
    }
    int hash = hashCode(address);
    probeAndInsert(address, allocationSize, hash);
    ++size_;
}

void AllocHashtable::probeAndInsert(void *address, size_t allocationSize, int hash) {
    while (table[hash].second != 0) {
        hash = static_cast<int>((hash + 1) % capacity_);
    }
    table[hash].first = address;
    table[hash].second = allocationSize;
}

size_t AllocHashtable::deleteNode(void *address) {
    int hash = hashCode(address);
    size_t cap = probeAndDelete(address, hash);
    --size_;
    return cap;
}

size_t AllocHashtable::probeAndDelete(const void *address, int hash) {
    while (table[hash].first != nullptr && table[hash].first != address) {
        hash = static_cast<int>((hash + 1) % capacity_);
    }
    size_t cap = table[hash].second;
    table[hash].first = nullptr;
    table[hash].second = 0;
    return cap;
}

void AllocHashtable::grow() {
    size_t oldAllocation = capacity_ * 16;
    size_t newAllocation = (capacity_ * 2) * 16;
    size_t oldCapacity = capacity_;
    capacity_ *= 2;
    std::pair<void *, size_t> *oldTable;
    oldTable = table;

    // map the new table to memory
    table = static_cast<std::pair<void *, size_t>*>(mmap(nullptr, newAllocation, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0));
    // copy over entries from old table
    for (int i = 0; i < oldCapacity; ++i) {
        if (oldTable[i].first != nullptr) {
            int hash = hashCode(oldTable[i].first);
            probeAndInsert(oldTable[i].first, oldTable[i].second, hash);
        }
    }
    munmap(oldTable, oldAllocation); // finished copying over, retire old table
}

std::pair<void *, size_t> *AllocHashtable::getTable() const {
    return table;
}

size_t AllocHashtable::getCapacity_() const {
    return capacity_;
}

int AllocHashtable::getSize_() const {
    return size_;
}
