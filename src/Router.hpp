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

#define ROUTER_RANK 4
#define ROUTER_N 129
#define MAX_DIGIT 8
#define DEVICES_FILE 10
#define RN_ROUTER_RANK 3
#define RN_LOCATION_LENGTH 7
#define AGGREGATION_LENGTH 7
#define AGGREGATION_N 2


class Router{
private:
  const  int ID;
  const float lat;
  const float lon;
  int rank;
  int devices;
  int up_ID;

  std::map <std::string ,  TrieTree > ST;
  std::map <std::string , std::list<int> > RNST;
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
  int getlocSTsize() const{return ST.size();}
  int getRNSTsize() const{return RNST.size();}
  int getLocDevices(std::string loc_name) {return LocDevices[loc_name];}
  int getSTsize();
  void SetDevice(int adevice);
  void SetLocDevices(std::string loc_name, int devices);
  void AddDevice(int adddevice);
  void AddST(std::string locname,std::string datname,int face);
  void AddST(std::string locname,std::string datname,std::vector<int> faces);
  void AddRNST(std::string loc_name, int addID);
  void Aggregation();
  int LPM(std::string a,std::string b);
  int Search_Locname_inST(std::string loc_name);
  
  void Set_RNs();
  float getHop();
  int CalcDevices(std::string loc_name);
};

typedef std::shared_ptr<Router> router_ptr;

#endif
