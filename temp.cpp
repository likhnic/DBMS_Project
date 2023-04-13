// class Frame{
//     public:
//     int pageNum;
//     char* pageData;
//     FILE *fp;

//     Frame(FILE*fp, int pageNum, char* pageData) {
//         this->pageNum = pageNum;
//         this->pageData = pageData;
//         this->fp = fp;
//     }
// };


// class BufferPool {
// private:

//     deque<Frame> frames;
//     int pageSize;
//     int numFrames;
//     int clockHand;

//     void addFrame(Frame frame) {
//         // if buffer pool is not full, add frame to end of list
//         // else remove least recently used frame and add new frame to front of list
//         if (frames.size() < numFrames) {
//             frames.push_back(frame);
//         } 
//         else {
//             frames.pop_back();
//             frames.push_front(frame);
//         }
//     }

// public:
//     BufferPool(int numFrames) {
//         this->numFrames = numFrames;
//         this->pageSize = PAGE_SIZE;
//         clockHand = 0;
//     }

//     char* getPage(FILE*fp, int pageNum) {

//         // check if page is already in buffer
//         // if yes, return page data
//         // else, read page from disk and add to buffer

//         for (int i = 0; i < frames.size(); i++) {
//             if (frames[i].pageNum == pageNum && frames[i].fp == fp) {
//                 return frames[i].pageData;
//             }
//         }

//         // page not in buffer, read from disk
//         char* pageData = new char[pageSize];
//         fseek(fp, pageNum * pageSize, SEEK_SET);
//         fread(pageData, pageSize, 1, fp);

//         // add page to buffer
//         Frame frame(fp, pageNum, pageData);
//         addFrame(frame);

//         return pageData;
//     }
// };

// class BufferManager{

//     private:
//     BufferPool* bufferPool;
//     unordered_map<int, int> pageFrameMap;

//     public:
//     BufferManager(int numFrames) {
//         bufferPool = new BufferPool(numFrames);
//     }

//     char *getPage(FILE *fp, int pageNum) {
//         return bufferPool->getPage(fp, pageNum);
//     }
// };