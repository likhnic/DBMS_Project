#ifndef __QUERY_PROCESSOR_H_
#define __QUERY_PROCESSOR_H_

#include <bits/stdc++.h>
#include "bufferManager.hpp"
using namespace std;    

#define LRU 1
#define MRU 2
#define CLOCK 3
#define PAGE_SIZE 4096

// process a select from query

class QueryProcessor{
    private:
    ReplacementPolicy* bufferManager;
    public:
    QueryProcessor(int numFrames, int replacementPolicy);
    void processQuery(FILE *fp, int col1, string value);
};


#endif