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


    Person p[100000];
    for(int i=0; i<100000; i++){
        string name = "abc" + to_string(i);
        strcpy(p[i].name, name.c_str());
        p[i].age = rand()%20+5;
        p[i].weight = rand()%50+20;
        cout<<p[i].name<<" "<<p[i].age<<" "<<p[i].weight<<endl;
    }

    int recordSize = sizeof(Person);

    int i=0;
    while(1){

        int numLeft = PAGE_SIZE;
        while(numLeft >= recordSize){
            fwrite(&p[i], recordSize, 1, fp);
            numLeft -= recordSize;
            i++;
            if(i==100000){
                break;
            }
        }
        while(numLeft > 0){
            char c = '\0';
            fwrite(&c, sizeof(char), 1, fp);
            numLeft--;
        }
        if(i==100000){
            break;
        }
    }

    fclose(fp);
}