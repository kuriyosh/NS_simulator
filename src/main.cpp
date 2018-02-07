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

std::string data_generate(int _device);

int main(){
  Router Edge(1,4,35,139,0);

  int i, j = 0;
  std::vector<std::string> line(7, "None");

  std::ifstream ifs("./devices/devices.csv");

  if(!ifs) {
	std::cout << "input error device file" << std::endl;
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
	Edge.AddST(line[3],data_generate(j), 0);
	j++;
	if (j % 10000 == 0) {
	  std::cout << j <<" completed" << "\n";
	  // break;
	}
  }
  std::cout << Edge.getSTsize()<< "\n";
  
  return 0;
}

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


