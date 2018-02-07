#include "trietree.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <typeinfo>

int TrieTreeNode::nodecount() const{
  int count = 0,i = 0;
  if(pointers[0] == NULL) return 1;
  while(pointers[i] != NULL){
	count += pointers[i]->nodecount();
	i++;
  }
  return count;
}

void TrieTreeNode::addface(int _face){
  face_num.push_back(_face);
  std::sort(face_num.begin(),face_num.end());
  face_num.erase(std::unique(face_num.begin(),face_num.end()),face_num.end()); 
}

void TrieTree::insert(std::string text,int _face){
  TrieTreeNode * temp = root;
  int i = 0;
  while(i < (int)text.length()){
	if(temp->pointers[text[i]-'a'] == NULL){
	  temp->pointers[text[i]-'a'] =  (TrieTreeNode *) calloc(1,sizeof(TrieTreeNode));
	  temp->pointers[text[i]-'a']->parent = temp;
  	  temp->pointers[text[i]-'a']->node_data = text[i];
	  temp->pointers[text[i]-'a']->addface(_face);
	}
	temp = temp->pointers[text[i]-'a'];
	i++;
  }
  dat_list[text].push_back(_face);
  std::sort(dat_list[text].begin(),dat_list[text].end());
  dat_list[text].erase(std::unique(dat_list[text].begin(),dat_list[text].end()),dat_list[text].end());
}

void TrieTree::insert(std::string text,std::vector<int> _faces){
  TrieTreeNode * temp = root;
  int i = 0;
  while(i < (int)text.size()){
	if(temp->pointers[text[i]-'a'] == NULL){
	  temp->pointers[text[i]-'a'] =  (TrieTreeNode *) calloc(1,sizeof(TrieTreeNode));
	  temp->pointers[text[i]-'a']->parent = temp;
	  for(int i = 0 ; i != (int)_faces.size() ; i++) temp->pointers[text[i]-'a']->addface(_faces[i]);
	}
	temp = temp->pointers[text[i]-'a'];
	i++;
  }
  for(int i = 0 ; i != (int)_faces.size() ; i++){
	dat_list[text].push_back(_faces[i]);
	std::sort(dat_list[text].begin(),dat_list[text].end());
	dat_list[text].erase(std::unique(dat_list[text].begin(),dat_list[text].end()),dat_list[text].end());
  }
}
  
TrieTreeNode* TrieTree::searchData(TrieTreeNode * trie_tree, std::string text){
  TrieTreeNode * temp = trie_tree;
  while(text.size() != 0){
	if(temp->pointers[text[0]-'a'] != NULL){
	  temp = temp->pointers[text[0]-'a'];
	  text.erase(text.begin());
	}else break;
  }
  if(text.size() == 0) return temp;
  else return NULL;
}
