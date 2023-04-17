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

/*
SELECT * FROM Person WHERE col1 = value
*/

// process a select from query
void QueryProcessor::processSelectQuery(FILE *fp, int col1, string value){
    
    int numPages = 0;
    int recordSize = 20*sizeof(char) + 2*sizeof(int);

    // get number of pages
    fseek(fp, 0, SEEK_END);
    numPages = ftell(fp)/PAGE_SIZE;
    fseek(fp, 0, SEEK_SET);

    // iterate over all pages
    for(int i=0;i<numPages;++i){
        // get page from buffer
        char *pageData = bufferManager->getPage(fp, i);
        int numRecords;
        memcpy(&numRecords, pageData, sizeof(int));
        pageData += sizeof(int);
        while(numRecords--){
            Person p;
            memcpy(&p, pageData, recordSize);
            pageData += recordSize;
            if(col1 == 1){
                if(strcmp(p.name, value.c_str()) == 0){
                    cout<<p.name<<" "<<p.age<<" "<<p.weight<<endl;
                }
            }
            else if(col1 == 2){
                if(p.age == stoi(value)){
                    cout<<p.name<<" "<<p.age<<" "<<p.weight<<endl;
                }
            }
            else if(col1 == 3){
                if(p.weight == stoi(value)){
                    cout<<p.name<<" "<<p.age<<" "<<p.weight<<endl;
                }
            }
        }
        // unpin page
        bufferManager->unpinPage(fp, i);
    }
    cout<<"Page Accesses: "<<bufferManager->getStats().accesses<<endl;
    cout<<"Page fault and Disk Reads: "<<bufferManager->getStats().diskreads<<endl;
    cout<<"Page Hits: "<<bufferManager->getStats().pageHits<<endl;

}


/*
SELECT * FROM Person JOIN Medical ON Person.col1 = Medical.col2
*/
void QueryProcessor::processJoinQuery(FILE *fp1, FILE *fp2, int col1, int col2){

    int numPages1 = 0;
    int recordSize1 = sizeof(Person);

    int numPages2 = 0;
    int recordSize2 = sizeof(Medical);
    // get number of pages
    fseek(fp1, 0, SEEK_END);
    numPages1 = ftell(fp1)/PAGE_SIZE;
    fseek(fp1, 0, SEEK_SET);

    fseek(fp2, 0, SEEK_END);
    numPages2 = ftell(fp2)/PAGE_SIZE;
    fseek(fp2, 0, SEEK_SET);

    for(int i=0;i<numPages1;++i){
        char* pageData1 = bufferManager->getPage(fp1, i);        
        if(pageData1 == NULL){
            cout<<"Number of Frames is too small\n";
            exit(0);
        }
        for(int j=0;j<numPages2;++j){
            char* pageData2 = bufferManager->getPage(fp2, j);
            if(pageData2 == NULL){
                cout<<"Number of Frames is too small\n";
                exit(0);
            }
            int page1Offset = 0;
            int numRecords1;
            memcpy(&numRecords1, pageData1, sizeof(int));
            page1Offset += sizeof(int);
            while(numRecords1--){
                Person p1;
                memcpy(&p1, pageData1+page1Offset, recordSize1);
                page1Offset += recordSize1;
                int numRecords2;
                memcpy(&numRecords2, pageData2, sizeof(int));
                int page2Offset = sizeof(int);
                while(numRecords2--){
                    Medical m;
                    memcpy(&m, pageData2+page2Offset, recordSize2);
                    page2Offset += recordSize2;
                    char field1[20];
                    char field2[20];

                    if(col1 == 1){
                        strcpy(field1, p1.name);
                    }
                    else if(col1 == 2){
                        strcpy(field1, to_string(p1.age).c_str());
                    }
                    else if(col1 == 3){
                        strcpy(field1, to_string(p1.weight).c_str());
                    }

                    if(col2 == 1){
                        strcpy(field2, m.name);
                    }
                    else if(col2 == 2){
                        strcpy(field2, to_string(m.vaccinated).c_str());
                    }
                    else if(col2 == 3){
                        strcpy(field2, m.disease);
                    }
                    else if(col2 == 4){
                        strcpy(field2, m.treatment);
                    }
                    else if(col2 == 5){
                        strcpy(field2, m.date);
                    }

                    if(strcmp(field1, field2) == 0){
                        cout<<p1.name<<" "<<p1.age<<" "<<p1.weight<<" "<<m.name<<" "<<m.vaccinated<<" "<<m.disease<<" "<<m.treatment<<" "<<m.date<<endl;
                    }
                }
            }
            bufferManager->unpinPage(fp2, j);
        }
        bufferManager->unpinPage(fp1, i);
    }

    cout<<"Page Accesses: "<<bufferManager->getStats().accesses<<endl;
    cout<<"Page fault and Disk Reads: "<<bufferManager->getStats().diskreads<<endl;
    cout<<"Page Hits: "<<bufferManager->getStats().pageHits<<endl;

}

int main(){

    FILE *fp1 = fopen("fileBinary.bin", "rb");
    FILE *fp2 = fopen("medicalBin.bin", "rb");

    cout<<"Select a Replacement Algorithm:\n";
    cout<<"1 LRU, 2 MRU, 3 CLOCK: ";
    int choice;
    cin>>choice;

    if(choice<1 || choice>3){
        cout<<"Invalid choice\n";
        exit(1);
    }

    cout<<"Enter number of frames: ";
    int numFrames;
    cin>>numFrames;

    QueryProcessor qp(numFrames, choice);

    cout<<"Enter 1 for select query, 2 for join query: ";
    int queryType;
    cin>>queryType;

    if(queryType == 1){
        cout<<"Enter column number of Database: ";
        int col;
        cin>>col;
        cout<<"Enter value to get all matching records: ";
        string value;
        cin>>value;
        qp.processSelectQuery(fp1, col, value);
    }
    else if(queryType == 2){
        cout<<"Enter column number of Database 1: ";
        int col1;
        cin>>col1;
        cout<<"Enter column number of Database 2: ";
        int col2;
        cin>>col2;
        qp.processJoinQuery(fp1, fp2, col1, col2);
    }
    else{
        cout<<"Invalid query type\n";
        exit(1);
    }


    fclose(fp1);
    fclose(fp2);
}