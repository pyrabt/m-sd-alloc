//
// Created by William Marringa on 2019-02-28.
//

#ifndef MSDALLOC_ALLOCHASHTABLE_H
#define MSDALLOC_ALLOCHASHTABLE_H


#include <cstdio>
#include <utility>

class AllocHashtable {

    std::pair<void *, size_t> *table; // node pointer
    size_t capacity_;
    int size_;

public:

    // constructor
    AllocHashtable();

    // destructor
    ~AllocHashtable();

    // Hashing function
    int hashCode(void *address);

    void insertNode(void *address, size_t allocationSize);

    size_t deleteNode(void *address);

    void grow();

    std::pair<void *, size_t> *getTable() const;

    size_t getCapacity_() const;

    int getSize_() const;

    void probeAndInsert(void *address, size_t allocationSize, int hash);

    size_t probeAndDelete(const void *address, int hash);
};


#endif //MSDALLOC_ALLOCHASHTABLE_H
