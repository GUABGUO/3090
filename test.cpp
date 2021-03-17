#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

using namespace std;

typedef pair<string, int> PAIR;

struct CmpByValue {
  bool operator()(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
    }
};


int main()
{
    map<string,int> test;
    test.emplace("a",1);
    test.emplace("b",3);
    test.emplace("c",2);
    vector<PAIR> ss(test.begin(),test.end());
    sort(ss.begin(),ss.end(),CmpByValue());
    for (int i = 0; i != ss.size(); ++i) 
    {  
        cout << ss[i].second << endl;  
    }  
    return 0;

}