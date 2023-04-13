#ifndef __BUF_MANAGER_H_
#define __BUF_MANAGER_H_

#include <bits/stdc++.h>
using namespace std;
#define PAGE_SIZE 4096

// // implementing buffer manager for LRU using Pinned and Unpinned pages 
// class of Frame which is an entity in Memory to store a Page
class Frame{
    public:
    int pageNum;       // page number of page
    char* pageData;    // data in page
    FILE *fp;          // file to which this page belongs to
    bool pinned;       // either pinned or unpinned

    void setFrame(FILE*fp, int pageNum, char* pageData, bool pinned);
    void unpinFrame();
};


struct PairHash {
    size_t operator()(const pair<FILE *, int>& p) const {
        size_t h1 = hash<FILE *>()(p.first);
        size_t h2 = hash<int>()(p.second);
        return h1 ^ h2;
    }
};



class BufStats{
    public:
    int accesses;
    int diskreads;
    int diskwrites;

    BufStats();
    void clear();
};

class LRUBufferManager{

    private:
    int numFrames;    // number of frames that can be fit in pool
    list<Frame> lru;  // list to implement LRU
    unordered_map<pair<FILE*, int>, list<Frame>::iterator, PairHash> mp;   // map to identify whether a page is present in buffer or not
    BufStats stats;

    public:
    LRUBufferManager(int numFrames);
    char* getPage(FILE*fp, int pageNum);
    ~LRUBufferManager();
    BufStats getStats();
    void clearStats();
    void unpinPage(FILE*fp, int pageNum);
};


class ClockBufferManager{
    private:
    
};








#endif