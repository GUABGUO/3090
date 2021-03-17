#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

//Server information
class ServerInfo
{
private:
	unordered_map<string,vector<int>> server_info_;
public:
	void insertdata()
	{
		server_info_.emplace("NV603",value(92,324,53800,500));
	};
	void sortdata()
	{
		
	}

	vector<int> value(int cpu, int ram, int buycost, int runcost)
	{
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		temp.push_back(buycost);
		temp.push_back(runcost);
		return temp;
	};

	vector<int> searchdata(string type)
	{
		unordered_map<string,vector<int>>::iterator it = server_info_.find(type);
    	if (it == server_info_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2),(*it).second.at(3));
		}
	};


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

	};
};

//Virtual Machine information
class VMInfo
{
private:
	unordered_map<string,vector<int>> vm_info_;
public:
	void insertdata()
	{
		vm_info_.emplace("c3.small.1",value(1,1,0));
	};

	vector<int> value(int cpu, int ram, int nodetype)
	{
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		temp.push_back(nodetype);
		return temp;
	};

	vector<int> searchdata(string type)
	{
		unordered_map<string,vector<int>>::iterator it = vm_info_.find(type);
    	if (it == vm_info_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2));
		}
	}

	void showdata()
	{
		unordered_map<string,vector<int>>::iterator it = vm_info_.find("c3.small.1");
    	if (it == vm_info_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			cout<<(*it).first<<endl;
			cout<<(*it).second.at(0)<<endl;
			cout<<(*it).second.at(1)<<endl;
			cout<<(*it).second.at(2)<<endl;
		}

	};

};

class Server
{
private:

public:
	string type_;
	int cpu_;
	int ram_;
	int buycost_;
	int runcost_;
	float proportion_;
	int nodetype_;
	Server(string type,ServerInfo server_info)
	{
		type_=type;
		vector<int> temp = server_info.searchdata(type);
		cpu_= temp.at(0);
		ram_= temp.at(1);
		buycost_= temp.at(2);
		runcost_= temp.at(3);
		//
	};

};


class ServerCurrent
{
private:
	unordered_map<int,Server> server_current_;
	size_t id;

public:
	ServerCurrent()
	{
		id = 0;
	};
	void buyserver(ServerInfo server_info)
	{
		if (0 == id)
		{
			//初始购买
		}
		string type;
		Server server(type,server_info);
		server_current_.emplace(id,server);
		id++;
	};
	void deployvm(int vm_id, string type, int cpu, int ram, int nodetype)
	{
		
	};
	void deletevm()
	{

	};
	Server searchdata(int server_id)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
    	if (it == server_current_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			return (*it).second;
		}
		
	};

};

class VMCurrent
{
private:
	/* data */
public:
};







int main(int argc, char** argv)
{
	ServerInfo server_info;
	server_info.insertdata();
	VMInfo vm_info;
	vm_info.insertdata();

	ServerCurrent server_current;
	VMInfo vm_current;

	//初始购买 策略未定
	server_current.buyserver(server_info);
	//天数
	int day;
	for (size_t i = 0; i < day; i++)
	{
		//迁移 策略未定
		/* code */

		//当天命令数量
		int command;
		for (size_t j = 0; j < command; i++)
		{
			//if ( add )
			{
				//if ( 需要购买 策略未定)
				{
					server_current.buyserver(server_info);
				}

				vector<int> temp = vm_info.searchdata("aa");
				server_current.deployvm(1, "aa", temp.at(0),temp.at(1),temp.at(2));
			}
			

			//if ( del )
			{
				server_current.deletevm();
			}
			
		}
		
		//输出 迁移 购买 调度

	}
	

	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}
