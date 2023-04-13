#include "bufferManager.hpp"




// populates a frame in Memory
void Frame::setFrame(FILE*fp, int pageNum, char* pageData, bool pinned){
    this->pageNum = pageNum;
    this->pageData = pageData;
    this->fp = fp;
    this->pinned = pinned;
    this->second_chance = true;
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
BufStats::BufStats(): accesses(0), diskreads(0) {}

// clear stats
void BufStats::clear(){
    accesses = 0;
    diskreads = 0;
}


// unpin a page
void LRUBufferManager::unpinPage(FILE*fp, int pageNum){
    // check if page present in memory using map
    auto it = mp.find({fp, pageNum});
    if(it != mp.end()){
        // page present in memory
        // unpin page
        it->second->unpinFrame();
    }
}

// constructor for ClockBufferManager
ClockBufferManager::ClockBufferManager(int numFrames): numFrames(numFrames), clock_hand(0), numPages(0){
    bufferPool = new Frame[numFrames];
}

// destructor for ClockBufferManager
ClockBufferManager::~ClockBufferManager(){
    delete[] bufferPool;
}

// get a page from buffer
char *ClockBufferManager::getPage(FILE* fp, int pageNum){


    // check if the page is present in memory
    for(int i=0;i<numPages;++i){
        if(bufferPool[i].fp == fp && bufferPool[i].pageNum == pageNum){
            // page is present in memory
            // update stats
            stats.accesses++;
            // update second chance
            bufferPool[i].second_chance = true;
            bufferPool[i].pinned = true;
            return bufferPool[i].pageData;
        }
    }

    // page is not present in memory
    if(numPages < numFrames){
        fseek(fp, pageNum*PAGE_SIZE, SEEK_SET);
        char* pageData = new char[PAGE_SIZE];
        fread(pageData, PAGE_SIZE, 1, fp);
        bufferPool[numPages].setFrame(fp, pageNum, pageData, true);
        numPages++;
        stats.accesses++;
        stats.diskreads++;
        return pageData;
    }
    stats.accesses++;
    stats.diskreads++;
    // page is not present in memory and memory is full
    while(true){
        if(bufferPool[clock_hand].second_chance){
            // page has second chance
            bufferPool[clock_hand].second_chance = false;
            clock_hand = (clock_hand+1)%numFrames;
            continue;
        }
        if(bufferPool[clock_hand].pinned){
            // page is pinned
            clock_hand = (clock_hand+1)%numFrames;
            continue;
        }
        // page is not pinned and does not have second chance
        // seek the page in file
        fseek(fp, pageNum*PAGE_SIZE, SEEK_SET);
        fread(bufferPool[clock_hand].pageData, PAGE_SIZE, 1, fp);
        clock_hand = (clock_hand+1)%numFrames;
        stats.diskreads++;
        return bufferPool[clock_hand].pageData;
    }
}

// unpin a page
void ClockBufferManager::unpinPage(FILE* fp, int pageNum){
    
    // check if page is present in memory
    for(int i=0;i<numPages;++i){
        if(bufferPool[i].fp == fp && bufferPool[i].pageNum == pageNum){
            // page is present in memory
            // unpin page
            bufferPool[i].unpinFrame();
            return;
        }
    }
}

