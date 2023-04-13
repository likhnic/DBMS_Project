#ifndef __QUERY_PROCESSOR_H_
#define __QUERY_PROCESSOR_H_

#include <bits/stdc++.h>
#include "bufferManager.hpp"
using namespace std;    

#define PAGE_SIZE 4096

// process a select from query

class QueryProcessor{
    private:
    ClockBufferManager* bufferManager;
    public:
    QueryProcessor(int numFrames);
    void processQuery(FILE *fp, int col1, string value);
};


#endif