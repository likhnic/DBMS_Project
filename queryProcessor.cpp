#include "queryProcessor.hpp"

// constructor for QueryProcessor
QueryProcessor::QueryProcessor(int numFrames, int replacementPolicy){
    if(replacementPolicy == CLOCK) bufferManager = new ClockBufferManager(numFrames);
    else if(replacementPolicy == LRU) bufferManager = new LRUBufferManager(numFrames);
    else if(replacementPolicy == MRU) bufferManager = new MRUBufferManager(numFrames);
    else{
        cout<<"Invalid replacement policy\n";
        exit(1);
    }
}

// process a select from query
void QueryProcessor::processSelectQuery(FILE *fp, int col1, string value){
    
    int numPages = 0;
    int recordSize = 20*sizeof(char) + 2*sizeof(int);

    // get number of pages
    fseek(fp, 0, SEEK_END);
    numPages = ftell(fp)/PAGE_SIZE;
    fseek(fp, 0, SEEK_SET);

    cout<<numPages<<endl;
    // iterate over all pages
    for(int i=0;i<numPages;++i){
        // get page from buffer
        char *pageData = bufferManager->getPage(fp, i);
        int numLeft = PAGE_SIZE;
        while(numLeft >= recordSize){
            char name[20];
            int age;
            int weight;
            memcpy(name, pageData,20*sizeof(char));
            memcpy(&age, pageData+20*sizeof(char), sizeof(int));
            memcpy(&weight, pageData+20*sizeof(char)+sizeof(int), sizeof(int));
            pageData += recordSize;
            numLeft -= recordSize;
            if(age == atoi(value.c_str())){
                cout<<name<<" "<<age<<" "<<weight<<endl;
            }
        }
        // unpin page
        bufferManager->unpinPage(fp, i);
    }
}


void QueryProcessor::processJoinQuery(FILE *fp1, FILE *fp2, int col1, int col2){

    int numPages1 = 0;
    int recordSize1 = 20*sizeof(char) + 2*sizeof(int);

    int numPages2 = 0;
    int recordSize2 = 20*sizeof(char) + 2*sizeof(int);
    // get number of pages
    fseek(fp1, 0, SEEK_END);
    numPages1 = ftell(fp1)/PAGE_SIZE;
    fseek(fp1, 0, SEEK_SET);

    fseek(fp2, 0, SEEK_END);
    numPages2 = ftell(fp2)/PAGE_SIZE;
    fseek(fp2, 0, SEEK_SET);

    for(int i=0;i<numPages1;++i){
        char* pageData1 = bufferManager->getPage(fp1, i);        
        if(pageData1 == NULL) cout<<"NULL\n";
        for(int j=0;j<numPages2;++j){
            char* pageData2 = bufferManager->getPage(fp2, j);
            if(pageData2 == NULL) cout<<"NULL\n";
            int x = 0;
            int page1Offset = 0;
            int numRecords1;
            memcpy(&numRecords1, pageData1, sizeof(int));
            page1Offset += sizeof(int);
            cout<<"*** "<<numRecords1<<endl;
            while(numRecords1--){
                char name1[20];
                x++;
                int age1;
                int weight1;
                int page2Offset = 0;
                memcpy(name1, pageData1+page1Offset,20*sizeof(char));
                memcpy(&age1, pageData1+page1Offset+20*sizeof(char), sizeof(int));
                memcpy(&weight1, pageData1+page1Offset+20*sizeof(char)+sizeof(int), sizeof(int));
                page1Offset += recordSize1;
                int numRecords2;
                memcpy(&numRecords2, pageData2, sizeof(int));
                page2Offset += sizeof(int);
                while(numRecords2--){
                    char name2[20];
                    int age2;
                    int weight2;
                    memcpy(name2, pageData2+page2Offset,20*sizeof(char));
                    memcpy(&age2, pageData2+page2Offset+20*sizeof(char), sizeof(int));
                    memcpy(&weight2, pageData2+page2Offset+20*sizeof(char)+sizeof(int), sizeof(int));
                    page2Offset += recordSize2;
                    if(age1 == age2){
                        cout<<name1<<" "<<name2<<" "<<age1<<" "<<weight1<<" "<<weight2<<endl;
                    }
                }
            }
            bufferManager->unpinPage(fp2, j);
        }
        bufferManager->unpinPage(fp1, i);
    }

}

int main(){

    FILE *fp1 = fopen("fileBinary.bin", "rb");
    FILE *fp2 = fopen("fileBinary.bin", "rb");

    QueryProcessor qp(10, MRU);
    qp.processJoinQuery(fp1, fp2, 1, 1);

    fclose(fp1);
    fclose(fp2);
}