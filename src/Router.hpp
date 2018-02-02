#ifndef ROUTER_HPP_INC
#define ROUTER_HPP_INC

#include <vector>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include "trietree.hpp"
#include <memory>
#include <tuple>

#define ROUTER_RANK 4
#define ROUTER_N 129
#define MAX_DIGIT 8
#define DEVICES_NUM 10000000
#define AGGREGATION_N 5
#define DEMAND_SEED 10101
#define DEMAND_DIGIT 8
#define	TEST_PACKETS 1000000

class Router{
private:
  const  int ID;
  const float lat;
  const float lon;
  int rank;
  int devices;
  int up_ID;
  
  std::map <std::string ,  std::list<int> > tmpST;
  std::map <std::string ,  std::list<int> > ST;
  //  std::map <std::string ,  TrieTree > edgeST;
  std::map <std::string ,int> LocDevices;
  
  int Search_Router(int aID);
public:
  Router(int _ID, int _rank, float _lat, float _lon, int _up_ID)
	: ID(_ID), lat(_lat), lon(_lon), rank(_rank), devices(0), up_ID(_up_ID)
  {}
  
  int getID() const {return ID;}
  float getlat() const{return lat;}
  float getlon() const{return lon;}
  int getRank() const{return rank;}
  int getDevices() const{return devices;}
  int getUpID() const{return up_ID;}
  int getSTsize() const{return ST.size();}
  //  int getedgeSTsize() const{return edgeST.size();}
  int getLocDevices(std::string loc_name){return LocDevices[loc_name];}
  int getdataSTsize();
  
  void SetDevice(int adevice);
  void SetLocDevices(std::string loc_name, int devices);
  void AddDevice(int adddevice);
  void AddST(std::string locname, int _face);
  void AddtmpST(std::string locname, int router_id);
  void Set_RNs();
  void AddedgeST(std::string locname,int face);
  void AddedgeST(std::string locname,std::string datname,std::vector<int> faces);
  void Aggregation();
  std::tuple<int,int> send_packet(std::string _demand);
  int LPM(std::string a,std::string b);
  int Search_Locname_inST(std::string loc_name);
  
  void STprint(std::ofstream &file);
  void STprint();
  float getHop();
  int CalcDevices(std::string loc_name);
  int leafNode();
};

typedef std::shared_ptr<Router> router_ptr;

#endif
