1.ServerInfo
unordered_map<string,vector<int>> server_info_
type--cpu,ram,buycost,runcost

void insertdata(string type, int cpu, int ram, int buycost, int runcost)
vector<int> searchdata(string type)


2.VMInfo
unordered_map<string,vector<int>> vm_info_
type--cpu,ram,nodetype

void insertdata(string type, int cpu, int ram, int nodetype)
vector<int> searchdata(string type)


3.ServerCurrent
unordered_map<int,Server> server_current_
server_id--type,cpu,ram,buycost,runcost,cpu_left_[2],ram_left_[2]

void buyserver(string type, int cpu, int ram, int buycost, int runcost)
Server searchdata(int server_id)
void deployvm(int server_id,  int cpu, int ram, int nodetype, int nodeplace)
void deletedata(int server_id, int cpu, int ram, int nodetype, int nodeplace)

4.VMCurrent
unordered_map<int,VM> vm_current_
vm_id--server_id,nodeplace,type,cpu,ram,nodetype

VM searchdata(int vm_id
void deployvm(int vm_id, int server_id, int nodeplace, string type, int cpu, int ram, int nodetype)
void deletevm(int vm_id)