#include "Router.hpp"
#include "trietree.hpp"
#include "network.hpp"
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

void Router::SetDevice(int adevice){
  devices = adevice;
}

int Router::getSTsize(){
  int sum = 0;
  for (auto& i: ST) {
    sum++;
	sum += (i.second).nodecount();
  }
  return sum;
}

void Router::SetLocDevices(std::string loc_name, int devices){
  auto itr = LocDevices.find(loc_name);
  if( itr != LocDevices.end() ) {
	LocDevices[loc_name] += devices;
  } else {
	LocDevices[loc_name] = devices;
  }
}

void Router::AddDevice(int adddevice){
  devices = devices + adddevice;
}

void Router::AddST(std::string locname,std::string datname,int face){
  for (int i = 8;i > 0; i--) {
    ST[locname.substr(0,i)].insert(datname,face);
  }
}

void Router::AddST(std::string locname,std::string datname,std::vector<int> faces){
  ST[locname].insert(datname,faces);
}

void Router::AddRNST(std::string loc_name, int addID){
  auto itr = RNST.find(loc_name);
  if( itr != RNST.end() ) {
	auto ite = find( itr->second.begin(), itr->second.end(), addID );
	if( ite == itr->second.end() ) itr->second.push_back(addID);
  } else {
	RNST[loc_name].push_back(addID);
	RNST[loc_name].sort();
	RNST[loc_name].unique();
  }
}

void Router::Aggregation(){
  // STを集約
  for(unsigned int r = MAX_DIGIT ; r > 0 ; r--){
	for(auto itr = ST.begin(); itr != ST.end(); ++itr) {
	  if((itr->first).size() == r){
		for(auto ite = ((itr->second).dat_list).begin() ; ite != ((itr->second).dat_list).end() ; ++ite){
		  ST[(itr->first).substr(0,r-1)].insert(ite->first,ite->second);
		}
	  }
	}
  }

  // 上位ルータへSTを渡す
  for(auto itr = ST.begin() ; itr != ST.end() ; itr++){
	// 集約できるかどうか判定
	if((itr->first).length() == AGGREGATION_LENGTH+1){
	  int same_prefix = 0;
	  for(auto ag_itr = ST.begin() ; ag_itr != ST.end() ; ag_itr++){
		if((ag_itr->first).length() == AGGREGATION_LENGTH+1){
		  if((ag_itr->first).substr(0,AGGREGATION_LENGTH) == (itr->first).substr(0,AGGREGATION_LENGTH)) same_prefix++;
		}
	  }
	  // 同じプレフィックスを持つものがAGGREGATION_N以上ならコピーしない
	  if(same_prefix >= AGGREGATION_N) continue;
	}
	for(auto ite = ((itr->second).dat_list).begin() ; ite != ((itr->second).dat_list).end() ; ++ite){
	  Network::instance().router(up_ID)->AddST(itr->first,ite->first,ite->second);
	}
  }
}

int Router::LPM(std::string a, std::string b)
{	
	for (int i = 0; i < MAX_DIGIT; i++) {
	  if (i == (int)a.size() || i == (int)b.size()) return i;
		if (a[i] != b[i]) return i;
	}
	return MAX_DIGIT;
}

int Router::Search_Locname_inST(std::string loc_name){
  for(auto itr = ST.begin(); itr != ST.end(); ++itr) {
	if(itr->first == loc_name) return ID;
  }
  return -1;
}

void Router::Set_RNs(){
  for(auto itr = ST.begin(); itr != ST.end(); ++itr) {
	if(itr->first.length() == RN_LOCATION_LENGTH){
	  for(int i=0 ; i<ROUTER_N ; i++){
		if(Network::instance().router(i)->getRank() == RN_ROUTER_RANK ){
		  int search_routerID = Network::instance().router(i)->Search_Locname_inST(itr->first);
		  if(search_routerID != -1) AddRNST(itr->first,search_routerID);
		}
	  }
	}
  }
}

float Router::getHop(){
  int count_hop = 0;
  int count_link = 0;
  for(auto itr = ST.begin(); itr != ST.end(); ++itr) {
	if(itr->first.length() == RN_LOCATION_LENGTH){
	  int maxcount2_hop = 0; int maxcount4_hop = 0; int maxcount6_hop = 0;
	  int tmp_hop = 0; int tmp_link =0;
	  tmp_hop += CalcDevices(itr->first)*2;
	  tmp_link += CalcDevices(itr->first);
	  maxcount2_hop = CalcDevices(itr->first);
	  auto ite = RNST.find(itr->first);
	  for(auto it = ite->second.begin() ; it != ite->second.end() ; ++it){
		if(*it == ID) continue;
		int near_router = Search_Router(*it);
		int near_router_upID = Network::instance().router(near_router)->getUpID();
		if(near_router_upID == up_ID){
		  tmp_hop += Network::instance().router(near_router)->CalcDevices(itr->first)*4;
		  tmp_link += Network::instance().router(near_router)->CalcDevices(itr->first);
		  maxcount4_hop = std::max(Network::instance().router(near_router)->CalcDevices(itr->first),maxcount4_hop);
		}else{
		  tmp_hop += Network::instance().router(near_router)->CalcDevices(itr->first)*6;
		  tmp_link += Network::instance().router(near_router)->CalcDevices(itr->first);
		  maxcount6_hop = std::max(Network::instance().router(near_router)->CalcDevices(itr->first),maxcount6_hop);
		}
	  }
	  if(maxcount2_hop >=  maxcount4_hop && maxcount2_hop >= maxcount6_hop){
		count_hop += tmp_hop;
		count_link += tmp_link;
	  }  
	}
  }
  return (float) count_hop / count_link;
}

int Router::Search_Router(int aID){
  int i;
  for(i=0;i<ROUTER_N;i++){
	if(Network::instance().router(i)->getID() == aID) return i;
  }
  return -1;
}

int Router::CalcDevices(std::string loc_name){
  int devices_sum = 0;
  if(rank ==4) return LocDevices[loc_name];
  for(int i=0 ; i<ROUTER_N ; i++){
	if(Network::instance().router(i)->
	   getUpID() == ID) devices_sum += Network::instance().router(i)->CalcDevices(loc_name);
  }
  return devices_sum;
}

