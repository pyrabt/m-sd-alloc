//
// Created by William Marringa on 2019-03-02.
//

#define CATCH_CONFIG_MAIN

#include <chrono>
#include "catch.hpp"
#include "allocHashtable.h"
#include "msdAlloc.h"

TEST_CASE("Hashtable Constructor Testing", "[allocHashTable()]") {
    SECTION("Proper member variable initialization") {
        AllocHashtable hashtable;
        REQUIRE(hashtable.getCapacity_() == 256);
        REQUIRE(hashtable.getSize_() == 0);
        REQUIRE(hashtable.getTable() != nullptr);
    }
    SECTION("Proper initialization of each node in the table") {
        AllocHashtable hashtable;
        int cap = static_cast<int>(hashtable.getCapacity_());
        for (int i = 0; i < cap; ++i) {
            REQUIRE(hashtable.getTable()[i].first == nullptr);
            REQUIRE(hashtable.getTable()[i].second == 0);
        }
    }
}

TEST_CASE("Hashing Function Testing", "[hashCode]") {
    SECTION("Hashing is reproducable") {
        int num = 21;
        int *npntr = &num;
        AllocHashtable hashtable;
        int testHash = hashtable.hashCode(npntr);
        REQUIRE(testHash == hashtable.hashCode(npntr));
    }

    SECTION("Hashing is unique to given pointer") {
        int num1 = 21;
        int *pntr1 = &num1;
        int num2 = 21;
        int *pntr2 = &num2;
        AllocHashtable hashtable;
        int hash1 = hashtable.hashCode(pntr1);
        int hash2 = hashtable.hashCode(pntr2);
        REQUIRE(hash1 != hash2);
    }
}

TEST_CASE("Hashtable node insertion table", "[insertNode]") {
    AllocHashtable hashtable;
    int num = 12;
    hashtable.insertNode(&num, 12);
    int hash = hashtable.hashCode(&num);
    REQUIRE(hashtable.getSize_() == 1);
    REQUIRE(hashtable.getTable()[hash].second == 12);

    std::string p = "potato";
    hashtable.insertNode(&p, 12);
    REQUIRE(hashtable.getSize_() == 2);

    SECTION("Verifying linear probing") {
        AllocHashtable hashtable;
        int num = 12;
        std::string test = "test";
        hashtable.probeAndInsert(&num, 7, 22);
        hashtable.probeAndInsert(&test, 12, 22);
        REQUIRE(hashtable.getTable()[22].first == &num);
        REQUIRE(hashtable.getTable()[23].first == &test);
    }
}

TEST_CASE("Hashtable node deletion", "[deleteNode]") {
    AllocHashtable hashtable;
    std::string cyka = "blyatt";
    int testNum = 256;
    hashtable.insertNode(&cyka, 35);
    hashtable.insertNode(&testNum, 52);
    REQUIRE(hashtable.deleteNode(&testNum) == 52);
    REQUIRE(hashtable.getSize_() == 1);

    SECTION("Verifying linear probing") {
        AllocHashtable hashtable;
        int num = 12;
        std::string test = "test";
        hashtable.probeAndInsert(&num, 7, 22);
        hashtable.probeAndInsert(&test, 12, 22);
        REQUIRE(hashtable.probeAndDelete(&test, 22) == 12);
        REQUIRE(hashtable.probeAndDelete(&num, 22) == 7);
    }
}

TEST_CASE("Hashtable growth", "[grow]") {
    SECTION("Explicit growth calls") {
        AllocHashtable hashtable;
        std::string seed = "find me";
        hashtable.insertNode(&seed, 69);
        hashtable.grow();
        REQUIRE(hashtable.getSize_() == 1);
        REQUIRE(hashtable.getCapacity_() == 512);
        hashtable.grow();
        REQUIRE(hashtable.getSize_() == 1);
        REQUIRE(hashtable.getCapacity_() == 1024);    
    }
   SECTION("Growth within insertion calls") {
        AllocHashtable hashtable;
        for (int i = 0; i < 192; ++i) {
            int filler = 69;
            hashtable.insertNode(&filler, 42);
        }
        REQUIRE(hashtable.getSize_() == 192);
        REQUIRE(hashtable.getCapacity_() == 256);
        std::string seed = "find me";
        hashtable.insertNode(&seed, 69);
        REQUIRE(hashtable.getSize_() == 193);
        REQUIRE(hashtable.getCapacity_() == 512);
    }
}

TEST_CASE("Memory Allocation/Deallocation", "[MSDAlloc.allocate/MSDAlloc.deallocate]") {
    MSDAlloc myAlloc;
    int p1[4];
    myAlloc.allocate(sizeof(p1));
    for(int n=0; n<4; ++n) { // populate the array
        p1[n] = n * n;
    }
    REQUIRE(p1[0] == 0);
    REQUIRE(p1[1] == 1);
    REQUIRE(p1[2] == 4);
    REQUIRE(p1[3] == 9);
    myAlloc.deallocate(p1);
}

TEST_CASE("MSDAlloc Timing vs Malloc", "[MSDAlloc]") {
    MSDAlloc myAlloc;
    double mallocTiming = 0;
    double MSDAllocTiming = 0;
    for (int i = 0; i < 10000; ++i) {
        int num = 69;
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        myAlloc.allocate(sizeof(num));
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        MSDAllocTiming += duration;

        int num2 = 69;
        std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
        auto x = malloc(sizeof(num2));
        std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
        mallocTiming += duration2;
    }

    mallocTiming /= 10000;
    MSDAllocTiming /= 10000;
    std::cout << "malloc timing: " << mallocTiming << " ms\n";
    std::cout << "MSDalloc timing: " << MSDAllocTiming << " ms\n";
}

