#include <iostream>
#include <vector>
#include <unordered_map>

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
    	if (it == server_info_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2),(*it).second.at(3));
		}
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
	//float proportion_;

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
	}
};


class ServerCurrent
{
private:
	unordered_map<int,Server> server_current_;
	int id = 0;

public:
	void buyserver(string type, int cpu, int ram, int buycost, int runcost)
	{
		Server server(type, cpu, ram, buycost, runcost);
		server_current_.emplace(id,server);
		id++;
	}

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
	}

	void deployvm(int server_id,  int cpu, int ram, int nodetype, int nodeplace)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
    	if (it == server_current_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			if (0 == nodetype)
			{
				if (0 == nodeplace)
				{
					(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu;
					(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram;
				}
				else
				{
					(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu;
					(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram;
				}
			}
			else
			{
				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu/2;
				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu/2;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram/2;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram/2;
			}
		}
	}

	void deletedata(int server_id, int cpu, int ram, int nodetype, int nodeplace)
	{
		unordered_map<int,Server>::iterator it = server_current_.find(server_id);
    	if (it == server_current_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			if (0 == nodetype)
			{
				if (0 == nodeplace)
				{
					(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu;
					(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram;
				}
				else
				{
					(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu;
					(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram;
				}
			}
			else
			{
				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu/2;
				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu/2;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram/2;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram/2;
			
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
    	if (it == vm_current_.end())
		{
    	    cout<<"Element Not Present"<<endl;
		}
   		else
		{
			return (*it).second;
		}
	}

	void deployvm(int vm_id, int server_id, int nodeplace, string type, int cpu, int ram, int nodetype)
	{
		VM vm(server_id, nodeplace, type, cpu, ram, nodetype);
		vm_current_.emplace(vm_id,vm);
	}

	void deletevm(int vm_id)
	{
		vm_current_.erase(vm_id);
	}


};



/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/


int main(int argc, char** argv)
{
	ServerInfo server_info;
	VMInfo vm_info;
	ServerCurrent server_current;
	VMCurrent vm_current;

	//1. 服务器与虚拟机信息存储
	int server_num;
	for (size_t i = 0; i < server_num; i++)
	{
		string type = "server type";
		int cpu;
		int ram; 
		int buycost;
		int runcost;
		server_info.insertdata(type, cpu, ram, buycost, runcost);
	}

	int vm_num;
	for (size_t i = 0; i < vm_num; i++)
	{
		string type = "vm type";
		int cpu;
		int ram; 
		int nodetype;
		vm_info.insertdata(type, cpu, ram, nodetype);
	}
	
	//2. 购买初始服务器
	//购买策略未定
	{
		string type = "server type";
		int cpu = server_info.searchdata(type).at(0);
		int ram = server_info.searchdata(type).at(1);
		int buycost = server_info.searchdata(type).at(2);
		int runcost = server_info.searchdata(type).at(3);
		server_current.buyserver(type,cpu,ram,buycost,runcost);
	}

	//3. 每天进行处理
	int day;
	for (size_t i = 0; i < day; i++)
	{
		//3.1 迁移
		//策略未定

		//3.2 每条指令进行处理
		int command;
		for (size_t j = 0; j < command; i++)
		{
			//if ( add type id )
			{
				string type = "vm type";
				int vm_id;

				//if ( 需要购买 策略未定)
				{
					string type = "server type";
					int cpu = server_info.searchdata(type).at(0);
					int ram = server_info.searchdata(type).at(1);
					int buycost = server_info.searchdata(type).at(2);
					int runcost = server_info.searchdata(type).at(3);
					server_current.buyserver(type,cpu,ram,buycost,runcost);
				}

				//进行部署 策略未定
				int server_id;
				int nodeplace ;
				int cpu = vm_info.searchdata(type).at(0);
				int ram = vm_info.searchdata(type).at(1);
				int nodetype = vm_info.searchdata(type).at(2);
				server_current.deployvm(server_id, cpu, ram, nodetype, nodeplace);
				vm_current.deployvm(vm_id, server_id, nodeplace, type, cpu, ram, nodetype);
			}
			
			//if ( del id)
			{
				int vm_id;
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

	}

	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}