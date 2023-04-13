#include "bufferManager.hpp"




// populates a frame in Memory
void Frame::setFrame(FILE*fp, int pageNum, char* pageData, bool pinned){
    this->pageNum = pageNum;
    this->pageData = pageData;
    this->fp = fp;
    this->pinned = pinned;
}

// unpin a frame
void Frame::unpinFrame(){
    this->pinned = false;
}


// constructor for LRUBufferManager
LRUBufferManager::LRUBufferManager(int numFrames): numFrames(numFrames) {}

// destructor for LRUBufferManager
LRUBufferManager::~LRUBufferManager(){
    
}

// get a page from buffer
char* LRUBufferManager::getPage(FILE*fp, int pageNum){

    // check if page present in memory using map
    auto it = mp.find({fp, pageNum});
    if(it != mp.end()){
        // page present in memory
        // update stats
        stats.accesses++;
        // update LRU
        lru.splice(lru.begin(), lru, it->second);
        return it->second->pageData;
    }
    else{
        // page not present in memory
        // update stats
        stats.accesses++;
        stats.diskreads++;
        // check if memory is full
        if(lru.size() == numFrames){
            // memory is full
            // check if last page is pinned
            if(lru.back().pinned){
                // last page is pinned
                // update stats
                stats.diskreads--;
                return NULL;
            }
            else{
                // last page is unpinned
                // remove last page from memory
                mp.erase({lru.back().fp, lru.back().pageNum});
                lru.pop_back();
            }
        }
        // read page from disk
        char* pageData = new char[PAGE_SIZE];
        fseek(fp, pageNum*PAGE_SIZE, SEEK_SET);
        fread(pageData, PAGE_SIZE, 1, fp);
        // add page to memory
        lru.push_front(Frame());
        lru.front().setFrame(fp, pageNum, pageData, false);
        mp[{fp, pageNum}] = lru.begin();
        return pageData;
    }
}

// clear stats
void LRUBufferManager::clearStats(){
    stats.clear();
}

// get stats
BufStats LRUBufferManager::getStats(){
    return stats;
}

// constructor for BufStats
BufStats::BufStats(): accesses(0), diskreads(0), diskwrites(0) {}

// clear stats
void BufStats::clear(){
    accesses = 0;
    diskreads = 0;
    diskwrites = 0;
}

