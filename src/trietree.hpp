#ifndef TRIETREE_HPP_INC
#define TRIETREE_HPP_INC

#include<map>
#include<cstdlib>
#include<string>
#include<vector>
#include<iostream>

#define ALPHABETS 26
#define MAX_WORD_SIZE 20

class TrieTreeNode
{
public:
  TrieTreeNode * pointers[ALPHABETS];
  TrieTreeNode * parent;
  std::vector<int> face_num;
  std::string node_data;
  int child=0;
  void addface(int _face);
  int nodecount();
};

class TrieTree{
public:
  TrieTreeNode * root;
  std::map <std::string , std::vector<int>> dat_list;
  TrieTree() {
	root = (TrieTreeNode *) calloc(1, sizeof(TrieTreeNode));
  }
  void insert(std::string text,int _face);
  void insert(std::string text,std::vector<int> _faces);
  int nodecount();
  TrieTreeNode * searchData(TrieTreeNode * trie_tree, std::string text);
  
};
#endif  
