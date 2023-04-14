#include "bufferManager.hpp"


// constructor for Frame
Frame::Frame(){}

// copy constructor for Frame
Frame::Frame(const Frame &frame){
    this->pageNum = frame.pageNum;
    this->pageData = new char[PAGE_SIZE];
    memcpy(this->pageData, frame.pageData, PAGE_SIZE);
    this->fp = frame.fp;
    this->pinned = frame.pinned;
    this->second_chance = frame.second_chance;
}

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

// destructor for Frame
Frame::~Frame(){
    delete[] pageData;
}


// constructor for LRUBufferManager
LRUBufferManager::LRUBufferManager(int numFrames): numFrames(numFrames) {}

// destructor for LRUBufferManager
LRUBufferManager::~LRUBufferManager(){
    lru.clear();    // calls destructor of Frame so delete of pageData happens
    mp.clear();
}

// get a page from buffer
char* LRUBufferManager::getPage(FILE*fp, int pageNum){

    // check if page present in memory using map
    auto it = mp.find({fp, pageNum});
    if(it!=mp.end()){
        stats.accesses++;
        // page present in memory
        lru.push_front(*it->second);
        lru.erase(it->second);
        mp[{fp, pageNum}] = lru.begin();
        lru.begin()->pinned = true;
        return lru.begin()->pageData;
    }
    // if page is not in memory
    // check if space is there in buffer

    if((int)lru.size() == numFrames){
        // cout<<"No space in buffer\n";
        if(lru.back().pinned){
            // no space in buffer
            // return null
            return NULL;
        }
        // remove last page from list
        mp.erase({lru.back().fp, lru.back().pageNum});
        lru.pop_back();
        // cout<<"Page removed from memory "<<lru.size()<<endl;
    }

    // add the page to buffer
    char* pageData = new char[PAGE_SIZE];
    fseek(fp, pageNum*PAGE_SIZE, SEEK_SET);
    fread(pageData, PAGE_SIZE, 1, fp);

    Frame frame = Frame();
    frame.setFrame(fp, pageNum, pageData, true);
    lru.push_front(frame);

    mp[{fp, pageNum}] = lru.begin();
    stats.accesses++;
    stats.diskreads++;
    char name[20];
    memcpy(name, pageData, 20);
    return lru.begin()->pageData;
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
        // send page to back of list
        lru.push_back(*it->second);
        lru.erase(it->second);
        mp[{fp, pageNum}] = --lru.end();
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
        bufferPool[clock_hand].fp = fp;
        bufferPool[clock_hand].pageNum = pageNum;
        bufferPool[clock_hand].pinned = true;
        bufferPool[clock_hand].second_chance = true;
        int store = clock_hand;
        clock_hand = (clock_hand+1)%numFrames;
        stats.diskreads++;
        return bufferPool[store].pageData;
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

// clear stats
void ClockBufferManager::clearStats(){
    stats.clear();
}

// get stats
BufStats ClockBufferManager::getStats(){
    return stats;
}

// constructor for MRUBufferManager
MRUBufferManager::MRUBufferManager(int numFrames): numFrames(numFrames) {}

// destructor for MRUBufferManager
MRUBufferManager::~MRUBufferManager(){
    mru.clear();      // the destructor of frame will be automatically called
    mp.clear();
}

// get a page from buffer
char *MRUBufferManager::getPage(FILE* fp, int pageNum){

    // check if page is present in memory
    auto it = mp.find({fp, pageNum});
    if(it!=mp.end()){
        stats.accesses++;
        // present so bring it to first and pin
        mru.push_front(*it->second);
        mru.erase(it->second);
        mp[{fp, pageNum}] = mru.begin();
        mru.begin()->pinned = true;
        return mru.begin()->pageData;
    }

    // not in memory, so check size
    if((int)mru.size() == numFrames){

        int removed = 0;
        for(auto it=mru.begin();it!=mru.end();++it){
            if(it->pinned){
                // page is pinned
                continue;
            }
            // page is not pinned
            // remove it from memory
            mp.erase({it->fp, it->pageNum});
            mru.erase(it);
            removed = 1;
            break;
        }

        if(!removed)return NULL;
    }

    // add the frame at start
    fseek(fp, pageNum*PAGE_SIZE, SEEK_SET);
    char* pageData = new char[PAGE_SIZE];
    fread(pageData, PAGE_SIZE, 1, fp);
    Frame frame = Frame();
        
    frame.setFrame(fp, pageNum, pageData, true);
    mru.push_front(frame);
    mp[{fp, pageNum}] = mru.begin();
    stats.accesses++;
    stats.diskreads++;
    return mru.begin()->pageData;
}


// unpin the frame

void MRUBufferManager::unpinPage(FILE *fp, int pageNum){

    // check if page is present in memory
    auto it = mp.find({fp, pageNum});
    if(it!=mp.end()){
        // page is present in memory
        // unpin page
        it->second->unpinFrame();
    }
}

void MRUBufferManager::clearStats(){
    stats.clear();
}

BufStats MRUBufferManager::getStats(){
    return stats;
}

