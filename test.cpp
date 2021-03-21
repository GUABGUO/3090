#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <utility>

#include <string>
#include <cstring>
#include <sstream> //stringstrea

using namespace std;
/*
typedef pair<string, int> PAIR;

struct CmpByValue {
  bool operator()(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
    }
};
struct cmp{
        template<typename T, typename U>
        bool operator()(T const& left, U const &right) {
            if (left.second > right.second) return true;
            return false;
        }
    };
    */
vector <string> stringsplit(const string &str, const char *delim)
{
	vector <std::string> strlist;
			int size = str.size();
			char *input = new char[size+1];
			strcpy(input, str.c_str());
			char *token = strtok(input, delim);
			while (token != NULL) {
				strlist.push_back(token);
				token = strtok(NULL, delim);
			}
			delete []input;
			return strlist;
}

int str2int(const string &str)
{
	stringstream stream(str);  
	int a = 0;
	stream >> a;
	return a;
}

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
   /*
   int b=1;
   int c=2;
   vector<int> a ;
   a=vector<int>(b,c);
    cout<<a.at(0)<<endl<<a.at(1);
    string line;
		vector<string> str;

    getline(cin,line);
		line.erase(line.size()-1);
		line.erase(0,1);
		str =  stringsplit(line,", ");


    //cout<<endl<<str2int(str.at(3)) + str2int(str.at(4))<<endl;
   // cout<<endl<<str.size();
    int a=111;
    //string a=" p" ;
    cout<<endl<<line+to_string(a);
  
    */
   cout << string("aa")+string("11");


    return 0;
}
/*
(purchase, 2)
(NV603, 1)
(NV604, 1) 
(migration, 0)
(0, A)
(0, B)


(purchase, 0)
(migration, 0)
(1) 
(purchase, 0) 
(migration, 0)
(1, B)
*/