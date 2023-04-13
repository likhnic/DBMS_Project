#include "queryProcessor.hpp"

// constructor for QueryProcessor
QueryProcessor::QueryProcessor(int numFrames, int replacementPolicy){
    if(replacementPolicy == CLOCK) bufferManager = new ClockBufferManager(numFrames);
    else if(replacementPolicy == LRU) bufferManager = new LRUBufferManager(numFrames);
}

// process a select from query
void QueryProcessor::processQuery(FILE *fp, int col1, string value){
    int numPages = 0;

    int recordSize = 20*sizeof(char) + 2*sizeof(int);

    int i=0;
    // get number of pages
    fseek(fp, 0, SEEK_END);
    numPages = ftell(fp)/PAGE_SIZE;
    fseek(fp, 0, SEEK_SET);

    cout<<numPages<<endl;
    // iterate over all pages
    int hello = 0;
    for(int i=0;i<numPages;++i){
        // get page from buffer
        char *pageData = bufferManager->getPage(fp, i);
        int numLeft = PAGE_SIZE;
        while(numLeft >= recordSize){
            char name[20];
            hello++;
            int age;
            int weight;
            memcpy(name, pageData, 20*sizeof(char));
            memcpy(&age, pageData+20*sizeof(char), sizeof(int));
            memcpy(&weight, pageData+20*sizeof(char)+sizeof(int), sizeof(int));
            // cout<<name;
            pageData += recordSize;
            numLeft -= recordSize;
            if(age == atoi(value.c_str())){
                // cout<<name<<" "<<age<<" "<<weight<<endl;
                cout<<name;
            }
        }
        // unpin page
        bufferManager->unpinPage(fp, i);
    }
    cout<<hello<<endl;

}

int main(){

    FILE *fp;
    fp = fopen("fileBinary.bin", "rb");

    QueryProcessor qp(10, CLOCK);
    qp.processQuery(fp, 2, "12");

    fclose(fp);
}