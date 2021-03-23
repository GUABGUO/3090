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

//server容量阈值
const float const_flag = 0.3;
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

	//选择服务器
	vector<string> chooseserver()
	{	
		int cpu = 0;
		int ram = 0;
		string type1;
		string type2;
		for(auto it = server_info_.begin(); it != server_info_.end(); it++)
		{
			if ((*it).second.at(0) > cpu)
			{
				cpu = (*it).second.at(0);
				type1 = (*it).first;
			}
			if ((*it).second.at(1) > ram)
			{
				ram = (*it).second.at(1);
				type2 = (*it).first;
			}
		} 
		vector<string> temp;
		temp.push_back(type1);
		temp.push_back(type2);
		return temp;
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
	float iffull;

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
		iffull = 0.5*(float(cpu_left_[0] + cpu_left_[1]) / float(cpu_)) + 0.5*(float(ram_left_[0] + ram_left_[1]) / float(ram_));
	}
};


class ServerCurrent
{
private:

	// 红黑树对服务器节点按匹配度排序, key：proporiton, value：id
	// 哈希表存储当前服务器信息，key：id，value：Server对象
	unordered_map< int, Server > server_current_;
	//float proportion = 0.5*(server_proportion0 + server_proportion1)
	multimap< float, int > proporition_server_double_;
	// 红黑树对服务器节点按cpu/ram排序, key：proporiton, value：id node
	multimap< float, vector<int> > proporition_server_single_;
	// 存放以接近存满的服务器
	unordered_map< int, float> server_full_;
	// id
	int id = 0;

public:
	// 查找服务器信息
	Server searchdata(int server_id)
	{
		auto it = server_current_.find(server_id);
		return (*it).second;
	}
	//求和
	vector<int> leftsum()
	{
		int cpu = 0;
		int ram = 0;
		if (!server_current_.empty())
		{
			for (auto it = server_current_.begin(); it != server_current_.end(); it++)
			{
				cpu = cpu + (*it).second.cpu_left_[0] + (*it).second.cpu_left_[1]; 
				ram = ram + (*it).second.ram_left_[0] + (*it).second.ram_left_[1]; 
			}
		}
		vector<int> temp;
		temp.push_back(cpu);
		temp.push_back(ram);
		return temp;
	}

	//买服务器
	void buyserver(string type, int cpu, int ram, int buycost, int runcost)
	{
		server_current_.emplace(id,Server(type, cpu, ram, buycost, runcost));
		//cout << "server num = "<<server_current_.size()<<endl;

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
	}

	//选择要部署的服务器
	vector<int> chooseserver(int vm_cpu, int vm_ram, int vm_nodetype)
	{
		float vm_proportion = float(vm_cpu)/float(vm_ram);
		vector<int> return_temp;
		/*
		cout<<endl<<"--------"<<endl;
		cout<<"single:"<<endl;
		for (auto ite = proporition_server_single_.begin(); ite != proporition_server_single_.end() ; ite++)
		{
			cout<<(*ite).second.at(0)<<"-"<<(*ite).first<<"  ";
		}
		cout<<endl<<"double:"<<endl;
		for (auto ite = proporition_server_double_.begin(); ite != proporition_server_double_.end() ; ite++)
		{
			cout<<(*ite).second<<"-"<<(*ite).first<<"  ";
		}
		cout<<endl<<endl<<"proportion = "<<vm_proportion<<endl;
		cout<<endl<<"--------"<<endl;
*/
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
				float server_proporiton1 = (*it_upper).first;
				server_cpu = searchdata(server_id1).cpu_left_[server_location1];
				server_ram = searchdata(server_id1).ram_left_[server_location1];
			}

			//反向迭代器
			auto it_lower = proporition_server_single_.lower_bound(vm_proportion);
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
				server_proporiton2 = (*it_lower).first;
				server_cpu = searchdata(server_id2).cpu_left_[server_location2];
				server_ram = searchdata(server_id2).ram_left_[server_location2];
			}

			//部署
			if (it_upper == proporition_server_single_.end()) {
				if(it_lower == proporition_server_single_.begin()){
		cout<<endl<<"--------"<<endl;
		cout<<"single:"<<endl;
		for (auto ite = proporition_server_single_.begin(); ite != proporition_server_single_.end() ; ite++)
		{
			cout<<(*ite).second.at(0)<<"-"<<(*ite).first<<"  ";
		}
		cout<<endl<<"double:"<<endl;
		for (auto ite = proporition_server_double_.begin(); ite != proporition_server_double_.end() ; ite++)
		{
			cout<<(*ite).second<<"-"<<(*ite).first<<"  ";
		}
		cout<<endl<<endl<<"proportion = "<<vm_proportion<<endl;
		cout<<endl<<"--------"<<endl;
					cout <<endl<<"error"<<endl;
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
				server_cpu0 = searchdata(server_id1).cpu_left_[0];
				server_ram0 = searchdata(server_id1).ram_left_[0];
				server_cpu1 = searchdata(server_id1).cpu_left_[1];
				server_ram1 = searchdata(server_id1).ram_left_[1];
			}	

			//反向迭代器
			auto it_lower = proporition_server_double_.lower_bound(vm_proportion);
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
				server_cpu0 = searchdata(server_id2).cpu_left_[0];
				server_ram0 = searchdata(server_id2).ram_left_[0];
				server_cpu1 = searchdata(server_id2).cpu_left_[1];
				server_ram1 = searchdata(server_id2).ram_left_[1];
			}	

			//判断购买与部署
			if (it_upper == proporition_server_double_.end()) {
				if(it_lower == proporition_server_double_.begin()){
					cout << "error"<<endl;
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
	int deployvm(int server_id, int nodeplace, int cpu, int ram, int nodetype)
	{
		int isfull = 0;
		auto it = server_current_.find(server_id);
		//单节点虚拟机
		if (0 == nodetype) {
			//deldete
			auto iter1 = proporition_server_single_.find((*it).second.proportion_[nodeplace]);
			for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[nodeplace]); k++, iter1++) {
				if (server_id == (*iter1).second.at(0) && nodeplace == (*iter1).second.at(1)){
					proporition_server_single_.erase(iter1);
					break;
				}
			}
			auto iter2 = proporition_server_double_.find((*it).second.proportion_all_);
			for (size_t k = 0; k < proporition_server_double_.count((*it).second.proportion_all_); k++, iter2++) {
				if (server_id == (*iter2).second){
					proporition_server_double_.erase(iter2);
					break;
				}
			}

			//add
			(*it).second.cpu_left_[nodeplace] = (*it).second.cpu_left_[nodeplace] - cpu;
			(*it).second.ram_left_[nodeplace] = (*it).second.ram_left_[nodeplace] - ram;
			if (0 == (*it).second.ram_left_[nodeplace] || 0 == (*it).second.cpu_left_[nodeplace]) {
				(*it).second.proportion_[nodeplace] = -1;
				(*it).second.proportion_all_ = -1;
			}
			else {
				(*it).second.proportion_[nodeplace] = float((*it).second.cpu_left_[nodeplace])/float((*it).second.ram_left_[nodeplace]);
				(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);
			}

			float iffull = 0.5*(float((*it).second.cpu_left_[0] + (*it).second.cpu_left_[1]) / float((*it).second.cpu_)) 
					   	+ 0.5*(float((*it).second.ram_left_[0] + (*it).second.ram_left_[1]) / float((*it).second.ram_));
			if ( iffull < const_flag){
				server_full_.emplace(server_id, iffull);
				isfull = 1;
			}
			else{
				vector<int> temp;
				temp.push_back(server_id);
				temp.push_back(nodeplace);
				proporition_server_single_.emplace((*it).second.proportion_[nodeplace], temp);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}


	
		}
		//双节点虚拟机
		else if (1 == nodetype) {
			//delete
			auto iter1 = proporition_server_single_.find((*it).second.proportion_[0]);
			for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[0]); k++, iter1++) {
				if (server_id == (*iter1).second.at(0) && 0 == (*iter1).second.at(1)){
					iter1 = proporition_server_single_.erase(iter1);
					break;
				}
			}
			auto iter2 = proporition_server_single_.find((*it).second.proportion_[1]);
			for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[1]); k++, iter2++) {
				if (server_id == (*iter2).second.at(0) && 1 == (*iter2).second.at(1)){
					iter2 = proporition_server_single_.erase(iter2);
					break;
				}
			}
			auto iter3 = proporition_server_double_.find((*it).second.proportion_all_);
			for (size_t k = 0; k < proporition_server_double_.count((*it).second.proportion_all_); k++, iter3++) {
				if (server_id == (*iter3).second){
					iter3 = proporition_server_double_.erase(iter3);
					break;
				}
			}
			//add
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

			float iffull = 0.5*(float((*it).second.cpu_left_[0] + (*it).second.cpu_left_[1]) / float((*it).second.cpu_)) 
					   	+ 0.5*(float((*it).second.ram_left_[0] + (*it).second.ram_left_[1]) / float((*it).second.ram_));
			if ( iffull < const_flag){
				server_full_.emplace(server_id, iffull);
				isfull = 1;
			}
			else
			{
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
		return isfull;
	}

	//删除虚拟机 更新当前服务器资源 维护两棵树
	void deletedata(int server_id, int cpu, int ram, int nodetype, int nodeplace, int isfull)
	{
		auto it = server_current_.find(server_id);
		//单节点虚拟机
		if (0 == nodetype) {
			if (0 == isfull){
				//delete
				auto iter1 = proporition_server_single_.find((*it).second.proportion_[nodeplace]);
				for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[nodeplace]); k++, iter1++) {
					if (server_id == (*iter1).second.at(0) && nodeplace == (*iter1).second.at(1)){
						proporition_server_single_.erase(iter1);
						break;
					}
				}
				auto iter2 = proporition_server_double_.find((*it).second.proportion_all_);
				for (size_t k = 0; k < proporition_server_double_.count((*it).second.proportion_all_); k++, iter2++) {
					if (server_id == (*iter2).second){
						proporition_server_double_.erase(iter2);
						break;
					}
				}
			}
			(*it).second.cpu_left_[nodeplace] = (*it).second.cpu_left_[nodeplace] + cpu;
			(*it).second.ram_left_[nodeplace] = (*it).second.ram_left_[nodeplace] + ram;
			(*it).second.proportion_[nodeplace] = float((*it).second.cpu_left_[nodeplace])/float((*it).second.ram_left_[nodeplace]);
			(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);

			float iffull = 0.5*(float((*it).second.cpu_left_[0] + (*it).second.cpu_left_[1]) / float((*it).second.cpu_)) 
					   	+ 0.5*(float((*it).second.ram_left_[0] + (*it).second.ram_left_[1]) / float((*it).second.ram_));
			if ( iffull < const_flag){
				server_full_[server_id] = iffull;
			}
			else
			{
				vector<int> temp0;
				temp0.push_back(server_id);
				temp0.push_back(nodeplace);
				proporition_server_single_.emplace((*it).second.proportion_[nodeplace], temp0);
				proporition_server_double_.emplace((*it).second.proportion_all_, server_id);
			}
		}
		//双节点虚拟机
		else if (1 == nodetype) {
			if (0 == isfull){
				//delete
				auto iter1 = proporition_server_single_.find((*it).second.proportion_[0]);
				for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[0]); k++, iter1++) {
					if (server_id == (*iter1).second.at(0) && 0 == (*iter1).second.at(1)){
						proporition_server_single_.erase(iter1);
						break;
					}
				}
				auto iter2 = proporition_server_single_.find((*it).second.proportion_[1]);
				for (size_t k = 0; k < proporition_server_single_.count((*it).second.proportion_[1]); k++, iter2++) {
					if (server_id == (*iter2).second.at(0) && 1 == (*iter2).second.at(1)){
						proporition_server_single_.erase(iter2);
						break;
					}
				}
				auto iter3 = proporition_server_double_.find((*it).second.proportion_all_);
				for (size_t k = 0; k < proporition_server_double_.count((*it).second.proportion_all_); k++, iter3++) {
					if (server_id == (*iter3).second){
						proporition_server_double_.erase(iter3);
						break;
					}
				}
			}
			//add
			(*it).second.cpu_left_[0] = (*it).second.cpu_left_[0] + cpu/2;
			(*it).second.cpu_left_[1] = (*it).second.cpu_left_[1] + cpu/2;
			(*it).second.ram_left_[0] = (*it).second.ram_left_[0] + ram/2;
			(*it).second.ram_left_[1] = (*it).second.ram_left_[1] + ram/2;
			(*it).second.proportion_[0] = float((*it).second.cpu_left_[0])/float((*it).second.ram_left_[0]);
			(*it).second.proportion_[1] = float((*it).second.cpu_left_[1])/float((*it).second.ram_left_[1]);
			(*it).second.proportion_all_ = 0.5*((*it).second.proportion_[0] + (*it).second.proportion_[1]);

			float iffull = 0.5*(float((*it).second.cpu_left_[0] + (*it).second.cpu_left_[1]) / float((*it).second.cpu_)) 
					   	+ 0.5*(float((*it).second.ram_left_[0] + (*it).second.ram_left_[1]) / float((*it).second.ram_));
			if ( iffull < const_flag){
				server_full_[server_id] = iffull;
			}
			else
			{
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
	int isfull_;

	VM(int server_id, int nodeplace, string type, int cpu, int ram, int nodetype, int isfull)
	{
		server_id_ = server_id;
		nodeplace_ = nodetype;
		type_ = type;
		cpu_ = cpu;
		ram_ = ram;
		nodetype_ = nodetype;
		isfull_ = isfull;
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

	void deployvm(int vm_id, int server_id, int nodeplace, string type, int cpu, int ram, int nodetype, int isfull)
	{
		vm_current_.emplace(vm_id,VM(server_id, nodeplace, type, cpu, ram, nodetype, isfull));
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
	queue<string> buy_out1;
	queue<string> buy_out2;
	queue<string> migration_out;
	queue<string> deploy_out;
	queue<string> deploy_in;

  // 	ofstream ccout;
  //  ccout.open ("output.txt");

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
	
	vector<string> temp_type = server_info.chooseserver();
	string type1 = temp_type.at(0);
	int cpu1 = server_info.searchdata(type1).at(0);
	int ram1 = server_info.searchdata(type1).at(1);
	cout<< cpu1<<endl;
	cout<< ram1<<endl;
	cout<< float(cpu1)/float(ram1)<<endl;
	int buycost1 = server_info.searchdata(type1).at(2);
	int runcost1 = server_info.searchdata(type1).at(3);

	string type2 = temp_type.at(1);
	int cpu2 = server_info.searchdata(type2).at(0);
	int ram2 = server_info.searchdata(type2).at(1);
	cout<< cpu2<<endl;
	cout<< ram2<<endl;
	cout<< float(cpu2)/float(ram2)<<endl;
	int buycost2 = server_info.searchdata(type2).at(2);
	int runcost2 = server_info.searchdata(type2).at(3);
	//3. 每天进行处理
	getline(ccin, line);
	int day = str2int(line);
	for (size_t i = 0; i < day; i++)
	{
		//3.1 扩容
		int cpu_all = 0;
		int ram_all = 0;
		getline(ccin, line);
		int command = str2int(line);
		for (size_t j = 0; j < command; j++)
		{
		    getline(ccin,line);
			deploy_in.push(line);

			line.erase(line.size()-1);
			line.erase(0,1);
			str = stringsplit(line,", ");
			if (3 == str.size())
			{		
				string vm_type = str.at(1);
				int vm_cpu = vm_info.searchdata(vm_type).at(0);
				int vm_ram = vm_info.searchdata(vm_type).at(1);
				cpu_all = cpu_all + vm_cpu;
				ram_all = ram_all + vm_ram;
			}
		}
		vector<int> temp_sum = server_current.leftsum();
		cout<<"-------------"<<endl;
		cout<<"cpu_add= "<<cpu_all<<endl;
		cout<<"ram_add= "<<ram_all<<endl;
		cout<<"cpu_left= "<<temp_sum.at(0)<<endl;
		cout<<"ram_left= "<<temp_sum.at(1)<<endl;

		cpu_all = cpu_all - temp_sum.at(0);
		ram_all = ram_all - temp_sum.at(1);		
		
		int buy_num1 = 10 * ceil(float(cpu_all)/float(cpu1));
		int buy_num2 = 10 * ceil(float(ram_all)/float(ram2));

		cout<<"num1 = "<<buy_num1<<endl;
		cout<<"num2 = "<<buy_num2<<endl;

		if (buy_num1 < 3){
			buy_num1 = 3;
		}
		if (buy_num2 < 3){
			buy_num2 = 3;
		}

		for (size_t j = 0; j < buy_num1; j++)
		{
			server_current.buyserver(type1,cpu1,ram1,buycost1,runcost1);
			buy_out1.push(type1);
		}
		for (size_t j = 0; j < buy_num2; j++)
		{
			server_current.buyserver(type2,cpu2,ram2,buycost2,runcost2);
			buy_out2.push(type2);
		}
		
		//3.2 迁移

		//3.3 部署 
		for (size_t j = 0; j < command; j++)
		{
			line = deploy_in.front();
			deploy_in.pop();
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
				vector<int> temp = server_current.chooseserver(vm_cpu, vm_ram, vm_nodetype);
				int server_id = temp.at(0);
				int nodeplace = temp.at(1);
				//cout<<"id = "<<server_id<<endl;
				//cout<<"flag = "<<nodeplace<<endl;
				//cout<<"type = "<<vm_nodetype<<endl;
				int isfull = server_current.deployvm(server_id, nodeplace, vm_cpu, vm_ram, vm_nodetype);
				vm_current.deployvm(vm_id, server_id, nodeplace, vm_type, vm_cpu, vm_ram, vm_nodetype, isfull);

				string out ;
				if (0 == vm_nodetype) {
					if (0 == nodeplace) {
						out = string("(") + to_string(server_id) + string(", A)");
					}
					else if (1 == nodeplace) {
						out = string("(") + to_string(server_id) + string(", B)");
					}
				}
				else if (1 == vm_nodetype) {
					out = string("(") + to_string(server_id) + string(")");
				}
				deploy_out.push(out);
				}

			else if (2 == str.size())
			{
				int vm_id = str2int(str.at(1));

				int server_id = vm_current.searchdata(vm_id).server_id_;
				int nodeplace = vm_current.searchdata(vm_id).nodeplace_;
				int vm_cpu = vm_current.searchdata(vm_id).cpu_;
				int vm_ram = vm_current.searchdata(vm_id).ram_;
				int vm_nodetype = vm_current.searchdata(vm_id).nodetype_;
				int vm_isfull = vm_current.searchdata(vm_id).isfull_;

				//进行删除
				server_current.deletedata(server_id, vm_cpu, vm_ram, vm_nodetype, nodeplace ,vm_isfull);
				vm_current.deletevm(vm_id);
				//vector<int>().swap(temp);
			}
		}
		/*
		//3.3 输出
		// 购买信息
		cout << string("(purchase, ") + to_string(2) + string(")")<<endl;
		fflush(stdout);
		if (!buy_out1.empty()){
			cout << string("(") + buy_out1.front() + string(", ") + to_string(buy_out1.size()) + string(")")<<endl;
			fflush(stdout);
		}
		while ( !buy_out1.empty()){
			buy_out1.pop();
		}
		if (!buy_out2.empty()){
			cout << string("(") + buy_out2.front() + string(", ") + to_string(buy_out2.size()) + string(")")<<endl;
			fflush(stdout);
		}
		while ( !buy_out2.empty()){
			buy_out2.pop();
		}

		// 迁移信息
		cout << string("(migration, 0)")<<endl;
		fflush(stdout);

		// 部署信息
		while ( !deploy_out.empty() )
		{
			cout << deploy_out.front() <<endl;
			fflush(stdout);
			deploy_out.pop();
		}
		*/
	}
	//ccout.close();

	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}