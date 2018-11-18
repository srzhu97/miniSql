#pragma once
#ifndef CATALOG_H
#define CATALOG_H
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "Macro.h"

using namespace std;


class CatalogManager {
public:
	vector<Table>	ctable;
	vector<Index> 	cindex;
	CatalogManager();
	~CatalogManager() {}

	//void ReadCatalog();
	//void UpdateCatalog();

	int IsTableExist(string tablename);			//判断表是否存在
	int IsIndexExist(string indexname);			//判断索引是否存在
	int IsAttrExist(string tablename, string attrname);//判断表上的属性是否存在
	int CreateTable(string tablename, vector<Attribute> attrs,short int HasPrimary);//判断是否成功创建表
	bool DropTable(string tablename);					//判断是否成功删除表
	int CreateIndex(string tablename, string attrname, string indexname, int AttributeOff);//判断是否成功创建索引,并返回此索引对应的属性的偏移量
	bool DropIndex(string indexname);	//判断是否成功删除索引	
	bool DropTableIndex(string tablename);
	int IsAttrUnique_Primary(string tablename, string atrrname);				//判断属性是否unique或是primary
	void UpdateTable();						//更新表信息文件
	void UpdateIndex(vector<IndexChangeInfo*> ChangeIndexInfo);						//更新索引信息文件
	//void InsertValue(string tablename); 		//插入一条记录，增加记录数量
	//void DeleteValue(string tablename,int deletenum);	//删除deletenum条记录
	unsigned int GetRecordLength(string tablename);						//获取表的记录长度
	//unsigned int GetRecordNum(string tablename);						//获取表的记录数目
	int GetAttrType(string tablename,string attrname);
	int GetAttrNameOffset(string tablename,string attrname);
	int GetIndexType(string indexname);				//获取索引的类型
	int GetOffset(string tablename, string attrname);//-1是没有找到这个属性，0-其他是属性的偏移量
	vector<int> GetTableAttributeType(string tablename); //在表存在的前提下获取表的所有属性类型
	Table getTableInformation(string tablename);	//在表存在的前提下获取表的信息
	vector<Index> getTableIndex(string tablename);	//在表存在的前提下获取表的所有索引信息
	vector<string> getAllAttrName(string tablename);
};
#endif
