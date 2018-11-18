#pragma once
//
//  indexManager.h
//  bplustree
//
//  Created by Ellie Maxinyin on 2017/6/20.
//  Copyright © 2017年 horseee. All rights reserved.
//

#ifndef indexManager_h
#define indexManager_h

#include "BPlusTree.h"
#include <map>
#include "Macro.h"
/*class indexManager {
public:
	bool createIndex(string IndexName, int type);//create table/create index int float n 
	bool dropIndex(string IndexName);//drop index
	bool InsertIntoIndex(string IndexName, string InsertKey, int offsetnumber);//插入语句 ，所有有index 属性偏移量，
	bool DeleteInIndex(string IndexName, string key);//多次调用，所有有index的属性都调用
	vector<int> searchIndex(string IndexName, string key, int ConditionType);//select语句，key是找的属性 ，select age, name key=age, name 多次调用
	bool InitializeTree(vector<Index> TreeInfo);//打开命令行就调用
	vector<IndexChangeInfo*> WriteBackTreeInfo();//quit的时候
	bool insertTree(string IndexName, int type, int root, int count);//我不用管
private:
	map<string, int>IndexType;
	map<string, BPlusTree<int>*> IntMap;
	map<string, BPlusTree<float>*>  FloatMap;
	map<string, BPlusTree<string>*> StringMap;
	BufMan bm;
};*/
class IndexManager {
public:
	bool createIndex(string IndexName, int type);//create table/create index int float n 
	bool dropIndex(string IndexName);//drop index
	bool InsertIntoIndex(string IndexName, string InsertKey, int offsetnumber);//插入语句 ，所有有index 属性偏移量，
	bool DeleteInIndex(string IndexName, string key);//多次调用，所有有index的属性都调用
	vector<int> searchIndex(string IndexName, string key, int ConditionType);//select语句，key是找的属性 ，select age, name key=age, name 多次调用
	bool InitializeTree(vector<Index> TreeInfo);//打开命令行就调用
	vector<IndexChangeInfo*> WriteBackTreeInfo();//quit的时候
	bool insertTree(string IndexName, int type, int root, int count);//我不用管
    bool DropAllTableRecord(string IndexName);

private:
	map<string,int>IndexType;
	map<string, BPlusTree<int>*> IntMap;
	map<string, BPlusTree<float>*>  FloatMap;
	map<string, BPlusTree<string>*> StringMap;
	BufMan bm;
};


#endif /* indexManager_h */
