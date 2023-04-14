#include <bits/stdc++.h>
#define PAGE_SIZE 4096
using namespace std;


// make a struct for database
typedef struct{

    char name[20];
    int age;
    int weight;
}Person;

int main(){

    FILE *fp;
    fp = fopen("fileBinary.bin", "wb");


    Person p[1000];
    for(int i=0; i<1000; i++){
        string name = "abc" + to_string(i);
        strcpy(p[i].name, name.c_str());
        p[i].age = rand()%20+5;
        p[i].weight = rand()%50+20;
        cout<<p[i].name<<" "<<p[i].age<<" "<<p[i].weight<<endl;
    }

    int recordSize = sizeof(Person);

    // add first 4 bytes in page 1 as number of records in last page


    int i=0;
    int totalRecords = 1000;
    while(1){
        int numLeft = PAGE_SIZE;
        // starting of each page add number of records in this page
        int possible = (PAGE_SIZE-4)/recordSize;
        int recordsInPage = min(possible, totalRecords);
        fwrite(&recordsInPage, sizeof(int), 1, fp);
        totalRecords -= recordsInPage;
        numLeft -= sizeof(int);

        while(numLeft >= recordSize){
            fwrite(&p[i], recordSize, 1, fp);
            numLeft -= recordSize;
            i++;
            if(i==1000){
                break;
            }
        }
        while(numLeft > 0){
            char c = '\0';
            fwrite(&c, sizeof(char), 1, fp);
            numLeft--;
        }
        if(i==1000){
            break;
        }
    }

    fclose(fp);
}