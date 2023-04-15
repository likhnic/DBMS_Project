#ifndef __BUF_MANAGER_H_
#define __BUF_MANAGER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <unordered_map>
#include <iterator>
#include <list>
using namespace std;
#define PAGE_SIZE 4096

// // implementing buffer manager for LRU using Pinned and Unpinned pages 
// class of Frame which is an entity in Memory to store a Page
class Frame{
    private:
    int pageNum;       // page number of page
    char* pageData;    // data in page
    FILE *fp;          // file to which this page belongs to
    bool pinned;       // either pinned or unpinned
    bool second_chance;         // for clock replacement algorithm

    void setFrame(FILE*fp, int pageNum, char* pageData, bool pinned);
    void unpinFrame();


    public:
    Frame();
    Frame(const Frame& f);
    ~Frame();
    friend class LRUBufferManager;
    friend class ClockBufferManager;
    friend class MRUBufferManager;
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

    BufStats();
    void clear();
};

class ReplacementPolicy {
public:
    virtual ~ReplacementPolicy() {}
    virtual char* getPage(FILE*fp, int pageNum) = 0;
    virtual void unpinPage(FILE*fp, int pageNum) = 0;
    virtual BufStats getStats() = 0;
    virtual void clearStats() = 0;
};


class LRUBufferManager: public ReplacementPolicy{

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


// implement clock replacementr algorithm
class ClockBufferManager: public ReplacementPolicy{

    private:
    int numFrames;    // number of frames that can be fit in pool
    Frame* bufferPool;  // list to implement clock
    int clock_hand;   // clock hand
    BufStats stats;
    int numPages;

    public:
    ClockBufferManager(int numFrames);
    char* getPage(FILE*fp, int pageNum);
    ~ClockBufferManager();
    void unpinPage(FILE*fp, int pageNum);
    BufStats getStats();
    void clearStats();
};


class MRUBufferManager: public ReplacementPolicy{

    private:
    int numFrames;    // number of frames that can be fit in pool
    list<Frame> mru;  // list to implement MRU
    unordered_map<pair<FILE*, int>, list<Frame>::iterator, PairHash> mp;   // map to identify whether a page is present in buffer or not
    BufStats stats;

    public:
    MRUBufferManager(int numFrames);
    char* getPage(FILE*fp, int pageNum);
    ~MRUBufferManager();
    BufStats getStats();
    void clearStats();
    void unpinPage(FILE*fp, int pageNum);
};








#endif