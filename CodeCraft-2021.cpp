#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include <fstream>//ifstream读文件，ofstream写文件，fstream读写文件
#include <string>
#include <cstring>
#include <sstream> //stringstrea

using namespace std;

//Server information
class ServerInfo
{
private:
	// 哈希表存储赋服务器类型, key：类型, value：cpu ram 购买成本 运行成本
	unordered_map< string, vector<int> > server_info_;
	// 红黑树对服务器按cpu/ram排序, key：proporiton, value：类型
	multimap<float, string> proporition_server_;

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
	// 插入服务器到哈希表及红黑树
	void insertdata(string type, int cpu, int ram, int buycost, int runcost)
	{
		server_info_.emplace(type,value(cpu, ram, buycost, runcost));

		float proportion = float(cpu)/float(ram);
		proporition_server_.emplace(proportion, type);
	}

	// 查找服务器信息
	vector<int> searchdata(string type)
	{
		auto it = server_info_.find(type);
		return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2),(*it).second.at(3));
	}

	// 选择与虚拟机比例最匹配的服务器类型
	string chooseserver(int vm_cpu, int vm_ram) {
		string server_type;
		float vm_proportion = float(vm_cpu)/float(vm_ram);

		//前向迭代器
		auto it_upper = proporition_server_.upper_bound(vm_proportion);
		if( it_upper == proporition_server_.end()){
			it_upper--;
		}
		string server_type1 = (*it_upper).second;
		float server_proporiton1 = (*it_upper).first;
		while (searchdata(server_type1).at(0) < vm_cpu || searchdata(server_type1).at(1) < vm_ram)
		{
			it_upper++;
			server_type1 = (*it_upper).second;
			server_proporiton1 = (*it_upper).first;
		}

		//反向迭代器
		auto it_lower = proporition_server_.lower_bound(vm_proportion);
		if( it_lower == proporition_server_.end()){
			it_lower--;
		}
		string server_type2 = (*it_lower).second;
		float server_proporiton2 = (*it_lower).first;
		while (searchdata(server_type2).at(0) < vm_cpu || searchdata(server_type2).at(1) < vm_ram)
		{
			it_lower--;
			server_type2 = (*it_lower).second;
			server_proporiton2 = (*it_lower).first;
		}

		if(fabs(server_proporiton1 - vm_proportion) < fabs(server_proporiton2 - vm_proportion)) {
			server_type = server_type1;
		}
		else {
			server_type = server_type2;
		}
		
		return server_type;
	}
};

//Virtual Machine information
class VMInfo
{
private:
	//哈希表存储虚拟机类型，key：类型，vlaue：cpu ram nodetype
	unordered_map< string, vector<int> > vm_info_;

	vector<int> value(int cpu, int ram, int nodetype)
	{
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		temp.push_back(nodetype);
		return temp;
	}

public:
	//插入虚拟机到哈希表
	void insertdata(string type, int cpu, int ram, int nodetype)
	{
		vm_info_.emplace(type, value(cpu, ram, nodetype));
	}

	vector<int> searchdata(string type)
	{
		auto it = vm_info_.find(type);
		return value((*it).second.at(0),(*it).second.at(1),(*it).second.at(2));
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
	int cpu_left_[2];
	int ram_left_[2];
	float proportion_[2];
	float proportion_all_;

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
		proportion_all_ = 0.5*(proportion_[0] + proportion_[1]);
	}
};


class ServerCurrent
{
private:
	// 哈希表存储当前服务器信息，key：id，value：Server对象
	unordered_map< int, Server > server_current_;
	// 红黑树对服务器节点按匹配度排序, key：proporiton, value：id
	//float proportion = 0.5*(server_proportion0 + server_proportion1)
	multimap< float, int > proporition_server_double_;
	// 红黑树对服务器节点按cpu/ram排序, key：proporiton, value：id node
	multimap< float, vector<int> > proporition_server_single_;
	// id

public:

	int id = 0;
	// 查找服务器信息
	Server searchdata(int server_id)
	{
		auto it = server_current_.find(server_id);
		return (*it).second;
	}
	//买服务器
	int buyserver(string type, int cpu, int ram, int buycost, int runcost)
	{
		server_current_.emplace(id,Server(type, cpu, ram, buycost, runcost));

		vector<int> temp0;
		temp0.push_back(id);
		temp0.push_back(0);
		vector<int> temp1;
		temp1.push_back(id);
		temp1.push_back(1);
		float proportion = float(cpu)/float(ram);
		proporition_server_single_.emplace(proportion, temp0);
		proporition_server_single_.emplace(proportion, temp1);
		proporition_server_double_.emplace(proportion, id);
		id++;
		return (id-1);
	}

	//选择要部署的服务器
	vector<int> deployserver(int vm_cpu, int vm_ram, int vm_nodetype, string buy_type, int buy_cpu, int buy_ram, int buycost, int runcost)
	{
		float vm_proportion = float(vm_cpu)/float(vm_ram);
		vector<int> return_temp;
		//初始购买
		if (id == 0){
			buyserver(buy_type, buy_cpu, buy_ram, buycost, runcost);
		}
		//单节点选择
		if ( 0 == vm_nodetype) {
			int server_cpu;
			int server_ram;
			//前向迭代器
			auto it_upper = proporition_server_single_.upper_bound(vm_proportion);
			if (it_upper == proporition_server_single_.end()){
				it_upper--;
			}
			int server_id1 = (*it_upper).second.at(0);
			int server_location1 = (*it_upper).second.at(1);
			float server_proporiton1 = (*it_upper).first;
			server_cpu = searchdata(server_id1).cpu_left_[server_location1];
			server_ram = searchdata(server_id1).ram_left_[server_location1];
			while (server_cpu < vm_cpu || server_ram <vm_ram)
			{
				it_upper++;
				if (it_upper == proporition_server_single_.end()){
					break;
				}
				server_id1 = (*it_upper).second.at(0);
				server_location1 = (*it_upper).second.at(1);
			}

			//反向迭代器
			auto it_lower = proporition_server_single_.upper_bound(vm_proportion);
			if (it_lower == proporition_server_single_.end()){
				it_lower--;
			}
			int server_id2 = (*it_lower).second.at(0);
			int server_location2 = (*it_lower).second.at(1);
			float server_proporiton2 = (*it_lower).first;
			server_cpu = searchdata(server_id2).cpu_left_[server_location2];
			server_ram = searchdata(server_id2).ram_left_[server_location2];
			while (server_cpu < vm_cpu || server_ram <vm_ram)
			{
				if (it_lower == proporition_server_single_.begin()){
					break;
				}
				it_lower--;
				server_id2 = (*it_lower).second.at(0);
				server_location2 = (*it_lower).second.at(1);

			}

			//判断购买与部署
			if (it_upper == proporition_server_single_.end()) {
				if(it_lower == proporition_server_single_.begin()){
					int id = buyserver(buy_type, buy_cpu, buy_ram, buycost, runcost);
					return_temp.push_back(id);
					return_temp.push_back(0);
					return_temp.push_back(1);
				}
				else if(it_lower != proporition_server_single_.begin()){
					return_temp.push_back(server_id2);
					return_temp.push_back(server_location2);
				}
			}
			else if(it_upper != proporition_server_single_.end()){
				if(it_lower == proporition_server_single_.begin()){
					return_temp.push_back(server_id1);
					return_temp.push_back(server_location1);
				}
				else if(it_lower != proporition_server_single_.begin()){
					if(fabs(server_proporiton1 - vm_proportion) < fabs(server_proporiton2 - vm_proportion)) {
						return_temp.push_back(server_id1);
						return_temp.push_back(server_location1);
					}
					else {
						return_temp.push_back(server_id2);
						return_temp.push_back(server_location2);
					}
				}
			}
		}
		//双节点选择
		else if (1 == vm_nodetype) {
			int server_cpu0 ;
			int server_ram0 ;
			int server_cpu1 ;
			int server_ram1 ;
			//前向迭代器
			auto it_upper = proporition_server_double_.upper_bound(vm_proportion);
			if (it_upper == proporition_server_double_.end()){
				it_upper--;
			}
			int server_id1 = (*it_upper).second;
			float server_proporiton1 = (*it_upper).first;
			server_cpu0 = searchdata(server_id1).cpu_left_[0];
			server_ram0 = searchdata(server_id1).ram_left_[0];
			server_cpu1 = searchdata(server_id1).cpu_left_[1];
			server_ram1 = searchdata(server_id1).ram_left_[1];
			while (server_cpu0 < vm_cpu/2 || server_cpu1 < vm_cpu/2 || server_ram0 < vm_ram/2 || server_ram1 < vm_ram) 
			{
				it_upper++;
				if (it_upper == proporition_server_double_.end()){
					break;
				}
				server_id1 = (*it_upper).second;
				server_proporiton1 = (*it_upper).first;
			}	

			//反向迭代器
			auto it_lower = proporition_server_double_.upper_bound(vm_proportion);
			if (it_lower == proporition_server_double_.end()){
				it_lower--;
			}
			int server_id2 = (*it_lower).second;
			float server_proporiton2 = (*it_lower).first;
			server_cpu0 = searchdata(server_id2).cpu_left_[0];
			server_ram0 = searchdata(server_id2).ram_left_[0];
			server_cpu1 = searchdata(server_id2).cpu_left_[1];
			server_ram1 = searchdata(server_id2).ram_left_[1];
			while (server_cpu0 < vm_cpu/2 || server_cpu1 < vm_cpu/2 || server_ram0 < vm_ram/2 || server_ram1 < vm_ram) 
			{
				if (it_lower == proporition_server_double_.begin()){
					break;
				}
				it_lower--;
				server_id2 = (*it_lower).second;
				server_proporiton2 = (*it_lower).first;
			}	

			//判断购买与部署
			if (it_upper == proporition_server_double_.end()) {
				if(it_lower == proporition_server_double_.begin()){
					int id = buyserver(buy_type, buy_cpu, buy_ram, buycost, runcost);
					return_temp.push_back(id);
					return_temp.push_back(0);
					return_temp.push_back(1);
				}
				else if(it_lower != proporition_server_double_.begin()){
					return_temp.push_back(server_id2);
					return_temp.push_back(0);
				}
			}
			else if(it_upper != proporition_server_double_.end()){
				if(it_lower == proporition_server_double_.begin()){
					return_temp.push_back(server_id1);
					return_temp.push_back(0);
				}
				else if(it_lower != proporition_server_double_.begin()){
					if(fabs(server_proporiton1 - vm_proportion) < fabs(server_proporiton2 - vm_proportion)) {
						return_temp.push_back(server_id1);
						return_temp.push_back(0);
					}
					else {
						return_temp.push_back(server_id2);
						return_temp.push_back(0);
					}
				}
			}
		}
		return return_temp;
	}
	
	//部署虚拟机 更新当前服务器资源 维护两颗树
	void deployvm(int server_id, int cpu, int ram, int nodetype, int nodeplace)
	{
		auto it = server_current_.find(server_id);
		if (0 == nodetype) {
			if (0 == nodeplace) {
				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram;
				if (0 == (*it).second.ram_left_[0] || 0 == (*it).second.cpu_left_[0]) {
					(*it).second.proportion_[0] = -1;
					(*it).second.proportion_all_ = -1;
				}
				else {
					(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
					(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
				}
				vector<int> temp;
				temp.push_back(server_id);
				temp.push_back(0);
				proporition_server_single_.emplace((*it).second.proportion_[0], temp);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}
			else if (1 == nodeplace) {
				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram;
				if (0 == (*it).second.ram_left_[1] || 0 == (*it).second.cpu_left_[1]) {
					(*it).second.proportion_[1] = -1;
					(*it).second.proportion_all_ = -1;
				}
				else {
					(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
					(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
				}
				vector<int> temp;
				temp.push_back(server_id);
				temp.push_back(1);
				proporition_server_single_.emplace((*it).second.proportion_[1], temp);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}
		}
		else if (1 == nodetype) {
			(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] - cpu/2;
			(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] - cpu/2;
			(*it).second.ram_left_[0] = (*it).second.ram_left_[0] - ram/2;
			(*it).second.ram_left_[1] = (*it).second.ram_left_[1] - ram/2;
			if (0 == (*it).second.ram_left_[0] || 0 == (*it).second.cpu_left_[0]) {
				(*it).second.proportion_[0] = -1;
				(*it).second.proportion_all_ = -1;
			}
			else {
				(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
				(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
			}

			if (0 == (*it).second.ram_left_[1] || 0 == (*it).second.cpu_left_[1]) {
				(*it).second.proportion_[1] = -1;
				(*it).second.proportion_all_ = -1;
			}
			else {
				(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
				(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
			}
				vector<int> temp0;
				temp0.push_back(server_id);
				temp0.push_back(0);
				proporition_server_single_.emplace((*it).second.proportion_[0], temp0);
				vector<int> temp1;
				temp1.push_back(server_id);
				temp1.push_back(1);
				proporition_server_single_.emplace((*it).second.proportion_[1], temp1);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
		}
	}

	//删除虚拟机 更新当前服务器资源 维护两棵树
	void deletedata(int server_id, int cpu, int ram, int nodetype, int nodeplace)
	{
		auto it = server_current_.find(server_id);
		if (0 == nodetype) {
			if (0 == nodeplace) {
				proporition_server_single_.erase(proporition_server_single_.find((*it).second.proportion_[0]));
				proporition_server_double_.erase(proporition_server_double_.find((*it).second.proportion_all_));

				(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu;
				(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram;
				(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
				(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);

				/*
  			  	for(auto iter = proporition_server_single_.begin(); iter!=proporition_server_single_.end(); iter++)
				{
					if (server_id == (*iter).second.at(0) && 0 == (*iter).second.at(1)){
						proporition_server_single_.erase(iter);
						break;
					}
				}
  			  	for(auto iter = proporition_server_double_.begin(); iter!=proporition_server_double_.end(); iter++)
				{
					if (server_id == (*iter).second){
						proporition_server_double_.erase(iter);
						break;
					}
				}
				*/
				vector<int> temp0;
				temp0.push_back(server_id);
				temp0.push_back(0);
				proporition_server_single_.emplace((*it).second.proportion_[0], temp0);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}
			else if (1 == nodeplace) {
				proporition_server_single_.erase(proporition_server_single_.find((*it).second.proportion_[1]));
				proporition_server_double_.erase(proporition_server_double_.find((*it).second.proportion_all_));

				(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu;
				(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram;
				(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
				(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);

				/*
  			  	for(auto iter = proporition_server_single_.begin(); iter!=proporition_server_single_.end(); iter++)
				{
					if (server_id == (*iter).second.at(0) && 1 == (*iter).second.at(1)){
						proporition_server_single_.erase(iter);
						break;
					}
				}
  			  	for(auto iter = proporition_server_double_.begin(); iter!=proporition_server_double_.end(); iter++)
				{
					if (server_id == (*iter).second){
						proporition_server_double_.erase(iter);
						break;
					}
				}
				*/
				vector<int> temp1;
				temp1.push_back(server_id);
				temp1.push_back(1);
				proporition_server_single_.emplace((*it).second.proportion_[1], temp1);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}
		}
		else if (1 == nodetype) {
			proporition_server_single_.erase(proporition_server_single_.find((*it).second.proportion_[0]));
			proporition_server_single_.erase(proporition_server_single_.find((*it).second.proportion_[1]));
			proporition_server_double_.erase(proporition_server_double_.find((*it).second.proportion_all_));

			(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu/2;
			(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu/2;
			(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram/2;
			(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram/2;
			(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
			(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
			(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
			
			/*
  		  	for(auto iter = proporition_server_single_.begin(); iter!=proporition_server_single_.end(); iter++)
			{
				if (server_id == (*iter).second.at(0) && 0 == (*iter).second.at(1)){
					proporition_server_single_.erase(iter);
					break;
				}
			}  		  
			for(auto iter = proporition_server_single_.begin(); iter!=proporition_server_single_.end(); iter++)
			{
				if (server_id == (*iter).second.at(0) && 1 == (*iter).second.at(1)){
					proporition_server_single_.erase(iter);
					break;
				}
			}
  		  	for(auto iter = proporition_server_double_.begin(); iter!=proporition_server_double_.end(); iter++)
			{
				if (server_id == (*iter).second){
					proporition_server_double_.erase(iter);
					break;
				}
			}
			*/
			vector<int> temp0;
			temp0.push_back(server_id);
			temp0.push_back(0);
			proporition_server_single_.emplace((*it).second.proportion_[0], temp0);
			vector<int> temp1;
			temp1.push_back(server_id);
			temp1.push_back(1);
			proporition_server_single_.emplace((*it).second.proportion_[1], temp1);
			proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
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


/*---------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
	ServerInfo server_info;
	VMInfo vm_info;
	ServerCurrent server_current;
	VMCurrent vm_current;

	string line;
	vector<string> str;
	queue<string> buy_out;
	queue<string> migration_out;
	queue<string> deploy_out;

   	ofstream ccout;
    ccout.open ("output.txt");


	ifstream ccin("training-1.txt");

	//1. 服务器与虚拟机信息存储
	getline(ccin, line);
	int server_num = str2int(line);
	for (size_t i = 0; i < server_num; i++)
	{
		getline(ccin,line);
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

	getline(ccin, line);
	int vm_num = str2int(line);
	for (size_t i = 0; i < vm_num; i++)
	{
		getline(ccin,line);
		line.erase(line.size()-1);
		line.erase(0,1);
		str = stringsplit(line,", ");

		string type = str.at(0);
		int cpu = str2int(str.at(1));
		int ram = str2int(str.at(2)); 
		int nodetype = str2int(str.at(3));
		vm_info.insertdata(type, cpu, ram, nodetype);
	}
	
	/*
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
	*/

	//3. 每天进行处理
	getline(ccin, line);
	int day = str2int(line);
	for (size_t i = 0; i < day; i++)
	{
		//3.1 迁移
		//策略未定

		//3.2 每条指令进行处理
		getline(ccin, line);
		int command = str2int(line);
		for (size_t j = 0; j < command; j++)
		{
		    getline(ccin,line);
			line.erase(line.size()-1);
			line.erase(0,1);
			str = stringsplit(line,", ");
			if (3 == str.size())
			{
				string vm_type = str.at(1);
				int vm_id = str2int(str.at(2));
				//进行部署 
				int vm_cpu = vm_info.searchdata(vm_type).at(0);
				int vm_ram = vm_info.searchdata(vm_type).at(1);
				int vm_nodetype = vm_info.searchdata(vm_type).at(2);
				string buy_type = server_info.chooseserver(vm_cpu, vm_ram);

				int buy_cpu = server_info.searchdata(buy_type).at(0);
				int buy_ram = server_info.searchdata(buy_type).at(1);
				int buycost = server_info.searchdata(buy_type).at(2);
				int runcost = server_info.searchdata(buy_type).at(3);


				vector<int> temp = server_current.deployserver(vm_cpu, vm_ram, vm_nodetype, buy_type, buy_cpu, buy_ram, buycost, runcost);
				if (1 == server_current.id) {
					buy_out.push(buy_type);
				}

				if( 3 == temp.size()) {
					buy_out.push(buy_type);
				}
				int server_id = temp.at(0);
				int nodeplace = temp.at(1);
				server_current.deployvm(server_id, vm_cpu, vm_ram, vm_nodetype, nodeplace);
				vm_current.deployvm(vm_id, server_id, nodeplace, vm_type, vm_cpu, vm_ram, vm_nodetype);

				string out ;
				if (0 == vm_nodetype)
				{
					if (0 == nodeplace)
					{
						out = string("(") + to_string(server_id) + string(", A)");
					}
					else if (1 == nodeplace)
					{
						out = string("(") + to_string(server_id) + string(", B)");
					}
				}
				else if (1 == vm_nodetype)
				{
					out = string("(") + to_string(server_id) + string(")");
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
		// 购买信息
		ccout << string("(purchase, ") + to_string(buy_out.size()) + string(")")<<endl;
		fflush(stdout);
		while ( !buy_out.empty())
		{
			ccout << string("(") + buy_out.front() + string(", 1)")<<endl;
			fflush(stdout);
			buy_out.pop();
		}

		// 迁移信息
		ccout << string("(migration, 0)")<<endl;
		fflush(stdout);

		// 部署信息
		while ( !deploy_out.empty() )
		{
			ccout << deploy_out.front() <<endl;
			fflush(stdout);
			deploy_out.pop();
		}
	}
	ccout.close();

	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}