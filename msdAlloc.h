//
// Created by William Marringa on 2019-02-28.
//

#ifndef MSDALLOC_MSDALLOC_H
#define MSDALLOC_MSDALLOC_H


#include <cstdio>
#include "allocHashtable.h"

class MSDAlloc {
public:
    AllocHashtable allocations;

    // constructor
    MSDAlloc();

    // destructor
    ~MSDAlloc();

    // 'malloc'
    void* allocate(size_t bytesToAllocate);

    // 'free'
    void deallocate(void* pointer);

};


#endif //MSDALLOC_MSDALLOC_H
