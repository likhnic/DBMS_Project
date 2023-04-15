#ifndef __QUERY_PROCESSOR_H_
#define __QUERY_PROCESSOR_H_

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
    void processSelectQuery(FILE *fp, int col1, string value);
    void processJoinQuery(FILE *fp1, FILE *fp2, int col1, int col2);
};


#endif