#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <utility>

using namespace std;
/*
typedef pair<string, int> PAIR;

struct CmpByValue {
  bool operator()(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
    }
};
*/
struct cmp{
        template<typename T, typename U>
        bool operator()(T const& left, U const &right) {
            if (left.second > right.second) return true;
            return false;
        }
    };

int main()
{
  /*
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
    int a=2;
    int b=3;
    float c=float(a)/float(b);
    cout<<c;
    unordered_map<int, int> mp;
    mp[3]=4;
    mp[2]=44;
    mp[12]=42;
    priority_queue<pair<int, int>, vector<pair<int, int>>, cmp>  pq(mp.begin(), mp.end());//完成pq的初始化
    cout<<pq.top().first<<endl;;
    pq.pop();
    cout<<pq.top().first<<endl;;
    pq.pop();
    cout<<pq.top().first<<endl;;
    pq.pop();
    */
   int b=1;
   int c=2;
   vector<int> a ;
   a=vector<int>(b,c);
    cout<<a.at(0)<<endl<<a.at(1);


    return 0;
}