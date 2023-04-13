#include <bits/stdc++.h>
using namespace std;


int main(){

    FILE *fp;
    fp = fopen("fileBinary.bin", "wb");
    int numPages = 10;
    fwrite(&numPages, sizeof(int), 1, fp);
    numPages = 20;
    fwrite(&numPages, sizeof(int), 1, fp);
    numPages = 30;
    fwrite(&numPages, sizeof(int), 1, fp);
    numPages = 40;
    fwrite(&numPages, sizeof(int), 1, fp);
    numPages = 50;
    fwrite(&numPages, sizeof(int), 1, fp);


    char c = 'a';
    fwrite(&c, sizeof(char), 1, fp);

    fclose(fp);

    fp = fopen("fileBinary.bin", "rb");
    int numbers[5];
    int readSize = fread(numbers, sizeof(int), 5, fp);

    cout << readSize << endl;
    // cout << buffer << endl;
    for(int i=0; i<5; i++){
        cout << numbers[i] << endl;
    }

    fclose(fp);
}