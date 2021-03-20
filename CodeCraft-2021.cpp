#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>

#include <string>
#include <cstring>
#include <sstream> //stringstrea

using namespace std;

//Server information
class ServerInfo
{
private:
	unordered_map<string,vector<int>> server_info_;

	vector<int> value(int cpu, int ram, int buycost, int runcost)
	{
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		temp.push_back(buycost);
		temp.push_back(runcost);
		return temp;
	}

public:
	void insertdata(string type, int cpu, int ram, int buycost, int runcost)
	{
		server_info_.emplace(type,value(cpu, ram, buycost, runcost));
	}

	void sortdata()
	{
	}

	vector<int> searchdata(string type)
	{
		unordered_map<string,vector<int>>::iterator it = server_info_.find(type);
		return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2),(*it).second.at(3));
	}


	void showdata()
	{
		unordered_map<string,vector<int>>::iterator it = server_info_.find("NV603");
    	if (it == server_info_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			cout<<(*it).first<<endl;
			cout<<(*it).second.at(0)<<endl;
			cout<<(*it).second.at(1)<<endl;
			cout<<(*it).second.at(2)<<endl;
			cout<<(*it).second.at(3)<<endl;
		}
	}
};

//Virtual Machine information
class VMInfo
{
private:
	unordered_map<string,vector<int>> vm_info_;

	vector<int> value(int cpu, int ram, int nodetype)
	{
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		temp.push_back(nodetype);
		return temp;
	}

public:
	void insertdata(string type, int cpu, int ram, int nodetype)
	{
		vm_info_.emplace(type, value(cpu, ram, nodetype));
	}

	vector<int> searchdata(string type)
	{
		unordered_map<string,vector<int>>::iterator it = vm_info_.find(type);
		return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2));
	}

	void showdata()
	{
		unordered_map<string,vector<int>>::iterator it = vm_info_.find("c3.small.1");
		cout<<(*it).first<<endl;
		cout<<(*it).second.at(0)<<endl;
		cout<<(*it).second.at(1)<<endl;
		cout<<(*it).second.at(2)<<endl;

	}

};

/*---------------------------------------------------------------------------------*/

class Server
{
private:

public:
	string type_;
	int cpu_;
	int ram_;
	int buycost_;
	int runcost_;
	//表示AB节点的剩余空间
	int cpu_left_[2];
	int ram_left_[2];
	float proportion_[2];

	Server(string type, int cpu, int ram, int buycost, int runcost)
	{
		type_ = type;
		cpu_ = cpu;
		ram_ = ram;
		buycost_ = buycost;
		runcost_ = runcost;
		cpu_left_[0]= cpu_/2;
		cpu_left_[1]= cpu_/2;
		ram_left_[0] = ram_/2;
		ram_left_[1] = ram_/2;
		proportion_[0] = float(cpu_left_[0])/float(ram_left_[0]);
		proportion_[1] = float(cpu_left_[1])/float(ram_left_[1]);
	}
};


class ServerCurrent
{
private:
	unordered_map<int,Server> server_current_;
	int id = 0;

	struct cmp{
        template<typename T, typename U>
        bool operator()(T const& left, U const &right) {
            if (left.second > right.second) return true;
            return false;
        }
    };

public:
	void buyserver(string type, int cpu, int ram, int buycost, int runcost)
	{
		server_current_.emplace(id,Server(type, cpu, ram, buycost, runcost));
		id++;
	}

	Server searchdata(int server_id)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
		return (*it).second;
	}

	vector<int> deployserver(int cpu, int ram, int nodetype)
	{
		float vm_proportion = float(cpu)/float(ram);
		vector<int> return_temp;

    	priority_queue<pair<vector<int>, float>, vector<pair<vector<int>, float>>, cmp> server_queue;
    	for(unordered_map<int,Server>::iterator it = server_current_.begin(); it!=server_current_.end(); it++)
		{
			int server_id = (*it).first;
			float server_proportion0 = (*it).second.proportion_[0];
			float server_proportion1 = (*it).second.proportion_[1];
			if (-1 != server_proportion0 && 0 != server_proportion0)
			{
				vector<int> temp0;
				temp0.push_back(server_id);
				temp0.push_back(0);
				float proportion0 = fabs(vm_proportion - server_proportion0);
				server_queue.push(make_pair(temp0,server_proportion0));

				if (-1 != server_proportion1 && 0 != server_proportion1)
				{
					vector<int> temp1;
					temp1.push_back(server_id);
					temp1.push_back(1);
					float proportion1 = fabs(vm_proportion - server_proportion1);
					server_queue.push(make_pair(temp1,server_proportion1));
				}
			}
		}
		if (0 == nodetype)
		{
			while (1)
			{
				int server_id = server_queue.top().first.at(0);
				int server_flag = server_queue.top().first.at(1);
				int server_cpu = searchdata(server_id).cpu_left_[server_flag];
				int server_ram = searchdata(server_id).ram_left_[server_flag];

				if (server_cpu - cpu < 0 || server_ram - ram < 0)
				{
					server_queue.pop();
				}
				else 
				{
					return_temp.push_back(server_id);
					return_temp.push_back(server_flag);
					break;
				}
			}
		}
		else if (1 == nodetype)
		{
			while (1)
			{
				int server_id = server_queue.top().first.at(0);
				int server_cpu0 = searchdata(server_id).cpu_left_[0];
				int server_ram0 = searchdata(server_id).ram_left_[0];
				int server_cpu1 = searchdata(server_id).cpu_left_[1];
				int server_ram1 = searchdata(server_id).ram_left_[1];


				if (server_cpu0 - cpu/2 < 0 || server_ram0 - ram/2 < 0 || server_cpu1 - cpu/2 < 0 || server_ram1 - ram/2 )
				{
					server_queue.pop();
				}
				else 
				{
					return_temp.push_back(server_id);
					return_temp.push_back(0);
					break;
				}
			}
		}
		
		return return_temp;
	}

	void deployvm(int server_id,  int cpu, int ram, int nodetype, int nodeplace)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
		if (0 == nodetype)
		{
			if (0 == nodeplace)
			{
				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram;
				if (0 == (*it).second.ram_left_[0])
				{
					(*it).second.proportion_[0] = -1;
				}
				else 
				{
					(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
				}
			}
			else if (1 == nodeplace)
			{
				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram;
				if (0 == (*it).second.ram_left_[1])
				{
					(*it).second.proportion_[1] = -1;
				}
				else
				{
					(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
				}
			}
		}
		else if (1 == nodetype)
		{
			(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu/2;
			(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu/2;
			(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram/2;
			(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram/2;
			if (0 == (*it).second.ram_left_[0])
			{
				(*it).second.proportion_[0] = -1;
			}
			else 
			{
				(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
			}

			if (0 == (*it).second.ram_left_[1])
			{
				(*it).second.proportion_[1] = -1;
			}
			else 
			{
				(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
			}
		}
	}

	void deletedata(int server_id, int cpu, int ram, int nodetype, int nodeplace)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
		if (0 == nodetype)
		{
			if (0 == nodeplace)
			{
				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram;
				(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
			}
			else if (1 == nodeplace)
			{
				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram;
				(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
			}
		}
		else if (1 == nodetype)
		{
			(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu/2;
			(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu/2;
			(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram/2;
			(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram/2;
			(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
			(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
		}
	}
};

/*---------------------------------------------------------------------------------*/

class VM
{
private:

public:
	int server_id_;
	int nodeplace_;
	string type_;
	int cpu_;
	int ram_;
	int nodetype_;

	VM(int server_id, int nodeplace, string type, int cpu, int ram, int nodetype)
	{
		server_id_ = server_id;
		nodeplace_ = nodetype;
		type_ = type;
		cpu_ = cpu;
		ram_ = ram;
		nodetype_ = nodetype;
		//
	}

};

class VMCurrent
{
private:
	unordered_map<int,VM> vm_current_;

public:
	VM searchdata(int vm_id)
	{
		unordered_map<int,VM>::iterator it = vm_current_.find(vm_id);
		return (*it).second;
	}

	void deployvm(int vm_id, int server_id, int nodeplace, string type, int cpu, int ram, int nodetype)
	{
		vm_current_.emplace(vm_id,VM(server_id, nodeplace, type, cpu, ram, nodetype));
	}

	void deletevm(int vm_id)
	{
		vm_current_.erase(vm_id);
	}


};



/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
vector <string> stringsplit(const string &str, const char *delim)
{
	vector <std::string> strlist;
	int size = str.size();
	char *input = new char[size+1];
	strcpy(input, str.c_str());
	char *token = strtok(input, delim);
	while (token != NULL) 
	{
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


int main(int argc, char** argv)
{
	ServerInfo server_info;
	VMInfo vm_info;
	ServerCurrent server_current;
	VMCurrent vm_current;

	//1. 服务器与虚拟机信息存储
	string line;
	vector<string> str;
	queue<string> buy_out;
	queue<string> migration_out;
	queue<string> deploy_out;
	string left = "(";
	string middle = ", ";
	string right = ")";
	string wordA = "A";
	string wordB = "B";

	int server_num;
	cin >> server_num;
	for (size_t i = 0; i < server_num; i++)
	{
		getline(cin,line);
		line.erase(line.size()-1);
		line.erase(0,1);
		str = stringsplit(line,", ");

		string type = str.at(0);
		int cpu = str2int(str.at(1));
		int ram = str2int(str.at(2)); 
		int buycost = str2int(str.at(3));
		int runcost = str2int(str.at(4));
		server_info.insertdata(type, cpu, ram, buycost, runcost);
	}

	int vm_num;
	cin >> vm_num;
	for (size_t i = 0; i < vm_num; i++)
	{
		getline(cin,line);
		line.erase(line.size()-1);
		line.erase(0,1);
		str = stringsplit(line,", ");

		string type = str.at(0);
		int cpu = str2int(str.at(1));
		int ram = str2int(str.at(2)); 
		int nodetype = str2int(str.at(3));
		vm_info.insertdata(type, cpu, ram, nodetype);
	}
	
	//2. 购买初始服务器
	//购买策略未定
	{
		string type = "host2D54I";
		int cpu = server_info.searchdata(type).at(0);
		int ram = server_info.searchdata(type).at(1);
		int buycost = server_info.searchdata(type).at(2);
		int runcost = server_info.searchdata(type).at(3);
		server_current.buyserver(type,cpu,ram,buycost,runcost);

		//string str_temp = "("
		//buy_out.push()
	}

	//3. 每天进行处理
	int day;
	cin >> day;
	for (size_t i = 0; i < day; i++)
	{
		//3.1 迁移
		//策略未定

		//3.2 每条指令进行处理
		int command;
		cin >> command;
		for (size_t j = 0; j < command; i++)
		{
		    getline(cin,line);
			line.erase(line.size()-1);
			line.erase(0,1);
			str = stringsplit(line,", ");

			if (3 == str.size())
			{
				string type = str.at(1);
				int vm_id = str2int(str.at(2));

				//if ( 需要购买 策略未定)
				/*
				{
					string type = "server type";
					int cpu = server_info.searchdata(type).at(0);
					int ram = server_info.searchdata(type).at(1);
					int buycost = server_info.searchdata(type).at(2);
					int runcost = server_info.searchdata(type).at(3);
					server_current.buyserver(type,cpu,ram,buycost,runcost);
				}
				*/

				//进行部署 
				int cpu = vm_info.searchdata(type).at(0);
				int ram = vm_info.searchdata(type).at(1);
				int nodetype = vm_info.searchdata(type).at(2);
				vector<int> temp = server_current.deployserver(cpu, ram, nodetype);
				int server_id = temp.at(0);
				int nodeplace = temp.at(1);
				server_current.deployvm(server_id, cpu, ram, nodetype, nodeplace);
				vm_current.deployvm(vm_id, server_id, nodeplace, type, cpu, ram, nodetype);

				string out ;
				if (0 == nodetype)
				{
					if (0 == nodeplace)
					{
						out = left + to_string(server_id) + middle + wordA + right;
					}
					else if (1 == nodeplace)
					{
						out = left + to_string(server_id) + middle + wordB + right;
					}
				}
				else if (1 == nodetype)
				{
					out = left + to_string(server_id) + right;
				}
				deploy_out.push(out);

			}
			else if (2 == str.size())
			{
				int vm_id = str2int(str.at(1));

				int server_id = vm_current.searchdata(vm_id).server_id_;
				int nodeplace = vm_current.searchdata(vm_id).nodeplace_;
				int cpu = vm_current.searchdata(vm_id).cpu_;
				int ram = vm_current.searchdata(vm_id).ram_;
				int nodetype = vm_current.searchdata(vm_id).nodetype_;
				server_current.deletedata(server_id, cpu, ram, nodetype, nodeplace);
				vm_current.deletevm(vm_id);
				//vector<int>().swap(temp);
			}
		}

		//3.3 输出
		while ( !deploy_out.empty() )
		{
			cout << deploy_out.front() <<endl;
			fflush(stdout);
			deploy_out.pop();
		}
				

	}

	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}