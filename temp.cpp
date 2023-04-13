#include <bits/stdc++.h>
using namespace std;


class x{
    public:

    x(){
        cout<<"Constr\n";
    }
    ~x(){
        cout<<"destructor"<<endl;
    }

    x(const x &x2){
        cout<<"copy constructor"<<endl;
    }
};

int main(){

    // list <int> l;
    // l.push_back(1);
    // l.push_back(2);
    // l.push_back(3);
    // l.push_back(4);
    // l.push_back(5);
    // l.push_back(6);

    // auto it = l.begin();
    // it++;
    // it++;

    // cout<<*it<<endl;

    // cout<<*it<<endl;

    list<x> l;
    l.push_back(x());
    l.push_back(x());
    l.push_back(x());

    map<int,list<x>::iterator>mp;


    // auto it = l.begin();
    // it++;
    // mp[1] = it;

    // mp.erase(1);

    x X = x();
    x X2 = x();
    
    cout<<"1\n";
    l.push_back(X);
    cout<<"2\n";

    cout<<"Hello\n";
}