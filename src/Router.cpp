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

// まだ集約の余地がある位置名を一時的に格納する
void Router::AddtmpST(std::string locname, int router_id){
  tmpST[locname].push_back(router_id);
  tmpST[locname].sort();
  tmpST[locname].unique();
}

void Router::AddST(std::string locname, int _face){
  ST[locname].push_back(_face);
  ST[locname].sort();
  ST[locname].unique();
  for (int i = locname.size()-1; i > 0; i--) {
    ST[locname.substr(0,i)].push_back(0);
	ST[locname.substr(0,i)].sort();
	ST[locname.substr(0,i)].unique();
  }
}

// void Router::AddedgeST(std::string locname,std::string datname,int face){
//   for (int i = locname.size(); i > 0; i--) {
// 	edgeST[locname.substr(0,i)].insert(datname,face);
//   }
// }

void Router::Aggregation(){
  //位置名CDの長さ＝高さとなるような木構造を作成
  std::list<std::string> cdlist;

  for (auto itr = tmpST.begin(); itr != tmpST.end(); ++itr){
	cdlist.push_back(itr->first);
	// 自身のSTへtmpSTを入れる
	for(auto ite = (itr->second).begin() ; ite != (itr->second).end() ; ite ++){
	  AddST(itr->first,*ite);
	}
  }
  if(rank == 1) return;
  cdlist.sort();
  std::vector<std::string> CDtree[MAX_DIGIT+1];
  for (auto itr = cdlist.begin(); itr != cdlist.end(); itr++) {
	std::string cd = *itr;
	CDtree[cd.size()].push_back(*itr);
  }
  
  for(int r = MAX_DIGIT; r>0 ; r--){
	for(int i = 0; i < (int)CDtree[r].size() ; i++){
	  std::string cd = CDtree[r][i]; 
	  std::list<int> tmplist(tmpST[cd]);
	  int prefix_count=1;
	  // プレフィックスが重複している位置名をカウントする
	  for(int continue_count = 1; i+continue_count < (int)CDtree[r].size() ; continue_count++){
		std::string  cmpCD = CDtree[r][i+continue_count];
		if (LPM(cd,cmpCD) == r-1){
		  prefix_count++;
		  for (auto itr = tmpST[cmpCD].begin() ; itr != tmpST[cmpCD].end(); itr++){
			tmplist.push_back(*itr);
			tmplist.sort();
			tmplist.unique();
		  }
		}
	  }
	  // AGGREGATION_Nよりもprefix_countが大きいならさらに上位で集約できるか調べる
	  if(prefix_count >= AGGREGATION_N){
		CDtree[r-1].push_back(cd.substr(0,r-1));
		i += prefix_count-1;
		// tmpSTには入れる
		for(auto itr = tmplist.begin() ; itr != tmplist.end() ; itr ++){
		  AddtmpST(cd.substr(0,r-1),*itr);
		}
	  // これ以上集約できないものに対しての処理
	  }else{
		Network::instance().search_router(up_ID)->AddtmpST(cd,ID);
	  }
	  std::sort(CDtree[r-1].begin(),CDtree[r-1].end());
	  CDtree[r-1].erase(std::unique(CDtree[r-1].begin(),CDtree[r-1].end()),CDtree[r-1].end());
	}
  }
}

// std::tuple<int, int> Router::send_packet(std::string demand){
//   int mis=0,suc=0;
//   int r_mis,r_suc;
//   if (rank == ROUTER_RANK) {
//     for(auto itr = edgeST.begin() ; itr != edgeST.end() ; ++itr){
// 	  if(itr->first == demand){
// 		//		std::cout <<"["<<ID <<"]rank"<<rank<<"hit:"<<demand<<"\n";
// 		return std::forward_as_tuple(0,1);
// 	  }
// 	}
// 	std::cout <<"["<<ID<< "]rank"<<rank<<"mis:"<<demand<<"\n";
// 	return std::forward_as_tuple(1,0);
//   }
//   for (int loclen = MAX_DIGIT;  loclen >0;--loclen) {
// 	for(auto itr = ST.begin() ; itr != ST.end() ; itr++){
// 	  if((itr->first).size() == loclen){
// 	    if(LPM(itr->first,demand) == loclen){
// 		  for(auto ite = (itr->second).begin() ; ite != (itr->second).end() ; ++ite){
// 			if(*ite == 0) continue; // 
// 			//	std::cout <<"[" <<ID<<"]" <<"rank"<<rank<<"hit:"<<demand.substr(0,loclen) <<",send to["<<*ite <<"]\n";
// 			router_ptr router = Network::instance().search_router(*ite);
// 			std::tie(r_mis,r_suc) = router->send_packet(demand);
// 			mis += r_mis;
// 			suc += r_suc;
// 		  }
// 		  return std::forward_as_tuple(mis,suc);
// 		}
// 	  }
// 	}
//   }
//   std::cout <<"["<<ID<< "]rank"<<rank<<"mis:"<<demand<<"\n";
//   return std::forward_as_tuple(1,0);
// }

int Router::LPM(std::string a, std::string b)
{	
  for (int i = 0; i < MAX_DIGIT; i++) {
	if (i == (int)a.size() || i == (int)b.size()) return i;
	if (a[i] != b[i]) return i;
  }
  return MAX_DIGIT;
}

void Router::STprint(std::ofstream &file){
  file << "===id[" << getID() << "]===" << std::endl;
  for(int i = 0 ; i < 9 ; i++){
	for(auto itr = ST.begin() ; itr != ST.end() ; ++itr){
	  if( i == (int)(itr->first).size() ){
		file  << itr->first << " : ";
		for (auto ite = itr->second.begin(); ite != itr->second.end(); ++ite) file << *ite << ",";
		file << "\n";
	  }
	}
  }
}

void Router::STprint(){
  std::cout << "===id[" << getID() << "]===" << std::endl;
  for(int i = 0 ; i < 9 ; i++){
	for(auto itr = ST.begin() ; itr != ST.end() ; ++itr){
	  if( i == (int)(itr->first).size() ){
		std::cout  << itr->first<<":" ;
		for(auto ite = itr->second.begin(); ite != itr->second.end();ite++){
		  std::cout << Network::instance().search_router(Network::instance().search_router_position(*ite))->getRank() <<",";
		}
		std::cout << "\n";
	  }
	}
  }
}

int Router::Search_Locname_inST(std::string loc_name){
  for(auto itr = ST.begin(); itr != ST.end(); ++itr) {
	if(itr->first == loc_name) return ID;
  }
  return -1;
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

int Router::leafNode(){
  int i=0;
  for(auto node : ST){
    if(node.first.length() == 8) i++;
  }
  return i;
}

// int Router::getdataSTsize(){
//   int datanode_count=0;
//   for(auto itr = edgeST.begin(); itr != edgeST.end(); ++itr) {
// 	datanode_count += (itr->second).nodecount();
//   }
//   return datanode_count;
// }
