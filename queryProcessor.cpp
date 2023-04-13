#include "queryProcessor.hpp"

// constructor for QueryProcessor
QueryProcessor::QueryProcessor(int numFrames){
    bufferManager = new ClockBufferManager(numFrames);
}

// process a select from query
void QueryProcessor::processQuery(FILE *fp, int col1, string value){
    int numPages = 0;

    // find number of pages in file
    fseek(fp, 0, SEEK_END);
    numPages = ftell(fp)/PAGE_SIZE;
    fseek(fp, 0, SEEK_SET);

    // first page is header, so get the number of records in each page
    // from header
    int numRecords[numPages-1];
    fread(numRecords, sizeof(int), numPages-1, fp);

    // header then contains the number of columns in each record and column size
    int numCols;
    fread(&numCols, sizeof(int), 1, fp);
    int colSize;
    fread(&colSize, sizeof(int), 1, fp);

    for(int i=1; i<numPages; i++){
        // get page from buffer manager
        char* pageData = bufferManager->getPage(fp, i);
        // iterate over all records in page
        for(int j=0; j<numRecords[i-1]; j++){
            // get the value of the column
            string colValue = "";
            for(int k=0; k<colSize; k++){
                colValue += pageData[j*colSize*numCols + col1*colSize + k];
            }
            // check if value matches
            if(colValue == value){
                // value matches
                // print the record
                for(int k=0; k<numCols; k++){
                    string colValue = "";
                    for(int l=0; l<colSize; l++){
                        colValue += pageData[j*colSize*numCols + k*colSize + l];
                    }
                    cout << colValue << " ";
                }
                cout << endl;
            }
        }
        // unpin page
        bufferManager->unpinPage(fp, i);
    }

}