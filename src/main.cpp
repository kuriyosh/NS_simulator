#include<fstream>
#include<iostream>
#include<string>
#include<sstream> 
#include<vector>
#include<random>
#include "Router.hpp"
#include "network.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int lineout(char *filename);
int Router_Setup();
int Device_Linkup(std::string filename);
void Aggregation();
void Set_each_RNs();
void EvaluationST();
void EvaluationMiscarriage();
std::string data_generate(int _device);

int main(){
  std::cout << "AGGREGATION_N:"<< AGGREGATION_N << "\n";
  std::cout << "DEVICES_NUM:"<< DEVICES_NUM << "\n";
  std::cout << "TEST_PACKET:"<<TEST_PACKETS << "\n";
  
  std::cout << "Router Setup start" << std::endl;
  Router_Setup();
  std::cout << "Device Linkup start" << std::endl;
  Device_Linkup("./device/cars.csv");
  Device_Linkup("./device/sensor160.csv");
  std::cout << "Aggregation Start" << std::endl;
  Aggregation();
  std::cout << "Evaluation Start" << std::endl;
  std::cout << " - Evaluating ST size" << "\n";
  EvaluationST();
  //  std::cout << " - Evaluating miscarriage" << "\n";
  //  EvaluationMiscarriage();
  std::cout << "===END===" << std::endl;
  
  return 0;
}

int Router_Setup(){
  int j = 0;
  std::ifstream ifs("./data/topology.csv");

  if(!ifs) {
	std::cout << "input error <topology.csv> ";
	return 1;
  }

  std::string str;

  while(std::getline(ifs, str)) {
	boost::algorithm::trim(str);
	if(str.empty()) continue;
	std::vector<std::string> line;
	boost::algorithm::split(line, str, boost::is_any_of(","));
	router_ptr router = router_ptr(new Router(std::stoi(line[0]),
											  std::stoi(line[1]),
											  std::stof(line[3]),
											  std::stof(line[4]),
											  std::stoi(line[5])));
	Network::instance().push_router(router);
	j++;
  }

  return j;
}

int Device_Linkup(std::string filename){
  int i, j = 0;
  int routerno;
  std::vector<std::string> line(7, "None");
  std::ifstream ifs(filename);
  
  std::cout << "input : "<< filename << "\n";
  if(!ifs) {
	std::cout << "input error <" <<filename<<">" << std::endl;
	return 1;
  }
  std::string str;
  while(std::getline(ifs, str)) {
	i = 0;
	std::string token;
	std::istringstream stream(str);
	while(std::getline(stream, token, ',')) {
	  line[i] = token;
	  i++;
	}
	router_ptr router = Network::instance().search_router(std::stoi(line[6]));
	if(router.get() != nullptr) {
	  routerno = Network::instance().search_router_position(std::stoi(line[6]));
	  // router->AddedgeST(line[3], routerno);	
	  router->AddtmpST(line[3],routerno);
	  router->SetDevice(router->getDevices() + 1);
	  j++;
	}
	if(j % 10000 == 0){
	  std::cout << j << " devices completed" << std::endl;
	  return 0;
	}
  }
  return 0;
}

void Aggregation(){
  int i,j;
  int upID;
  for(i=ROUTER_RANK;i>0;i--){
	for(j=0;j<ROUTER_N;j++){
	  router_ptr router = Network::instance().router(j);
	  if(router->getRank() == i){
		router->Aggregation();
		upID = router->getUpID();
		if(upID != 0) Network::instance().search_router(upID)->AddDevice(router->getDevices());		
	  }
	}
  }
}

void EvaluationST(){
  std::string filenameA,filenameB,filenameC;
  int devices;
  int STsize;
  int counta,countb,countc;
  int dataSTsize=0;
  int leaf_node = 0;
   
  for(int i = 1; i<ROUTER_RANK+1 ; i++){
	counta = countb = countc = 0;
	devices = 0;
	STsize = 0;
	filenameA = "./result"+std::to_string(AGGREGATION_N)+"/rank" + std::to_string(i) + "ST.txt";
	filenameB = "./result"+std::to_string(AGGREGATION_N)+"/rank" + std::to_string(i) + "device.csv";
	filenameC = "./result"+std::to_string(AGGREGATION_N)+"/rank" + std::to_string(i) + "STsize.csv";
	std::ofstream ofsA(filenameA);
	std::ofstream ofsB(filenameB);

	std::ofstream ofsC(filenameC);

	for(int j = 0; j<ROUTER_N ; j++){
	  router_ptr router = Network::instance().router(j);
	  if(router->getRank() == i){
		router->STprint(ofsA);
		int tmpSTsize;
		// エッジルータにはedgeSTを出力させる
		// if (router->getRank() == ROUTER_RANK) {
		//   if((tmpSTsize = router->getedgeSTsize()) != 0){
		// 	STsize+= tmpSTsize;
		// 	dataSTsize += router->getdataSTsize();
		// 	ofsC << router->getID() << "," << tmpSTsize << std::endl;
		// 	countb++;
		//   }
		//		}else {
		if((tmpSTsize = router->getSTsize()) != 0){
		  STsize += tmpSTsize;
		  ofsC << router->getID() << "," << tmpSTsize << std::endl;
		  countb++;
		}
		//		}
		if(router->getDevices() !=0){
		  devices += router->getDevices();
		  ofsB << router->getID() << "," << router->getDevices() << std::endl;
		  counta++;
		}
	  }
	  if(router->getRank() == 4){
		countc++;
		leaf_node += router->leafNode();
	  }
	}
	//std::cout << "device_rank" << i << ":" << devices/counta << std::endl;
	std::cout << "STsize_rank" << i << ":" << 	STsize/countb << std::endl;
  }
  std::cout << "leafNode:" <<  leaf_node/countc;
  std::cout << "dataSTsize:" << dataSTsize/countb << "\n";
}
  
// void EvaluationMiscarriage(){
//   std::mt19937 mt(DEMAND_SEED);
//   std::uniform_int_distribution<> rand(0,3);
//   float sum_miscarriage=0;
//   int packet_count = 0;
//   router_ptr root_router;
//   int r_mis,r_suc;
  
//   for (int i = 0; i < ROUTER_N; ++i) {
// 	root_router = Network::instance().router(i);
//     if(root_router->getRank() == 1) break;
//   }
  
//   for (int packet = 0; packet < TEST_PACKETS; ++packet) {
// 	if(packet == TEST_PACKETS *0.25) std::cout << "misscarriagge 25 % completed" << "\n";
// 	else if (packet == TEST_PACKETS *0.5) std::cout << "misscarriagge 50 % completed" << "\n";
// 	else if (packet == TEST_PACKETS *0.75) std::cout << "misscarriagge 75 % completed" << "\n";
// 	std::string demand="";
// 	for (int i = 0;  i < DEMAND_DIGIT; ++ i) {
// 	  demand = demand + std::to_string(rand(mt));
// 	}
// 	std::tie(r_mis,r_suc) = root_router->send_packet(demand);
// 	if(r_suc != 0){
// 	  packet_count++;
// 	  sum_miscarriage += r_mis/r_suc;
// 	}
//   }
//   std::cout << "packet count:"<<packet_count << "\n";
//   std::cout << "sum_mis_ratio:"<< sum_miscarriage<< "\n";
//   std::cout << "mis_ratio:"<< sum_miscarriage/packet_count<< "\n";
// }


std::string data_generate(int _device){
  std::mt19937 mt(_device);
  std::uniform_int_distribution<> rand(0,99);
  std::ifstream ifs("./data/dataname");
  int line_target = rand(mt);
  if(!ifs) {
	std::cout << "input error device file" << std::endl;
	return NULL;
  }
  std::string str;
  int line = 0;
  while(std::getline(ifs, str)) {
	if(line == line_target) return str;
	line++;
  }
  return str;
}




