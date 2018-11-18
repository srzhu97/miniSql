#ifndef RECORD_H
#define RECORD_H
#include <iostream>
#include <string>
#include "bm.h"
#include "Macro.h"
#include "interpreter.h"

using namespace std;
class RecordManager{
private:
	BufMan bm;
	vector<string> DeleteIndex(vector<int> record,string tablename,vector<int> type);
public:
	RecordManager(){};
	~RecordManager(){};
	//API需要传给record（string tablename,vecotr<string> value,vector<int> type）
	//其中vector<string>value是指所有属性值，vector<int>type是指每一个属性对应的类型
	//调用Buffer的 【int AddRecord(string TableName,char* content)】
    void CreateIndexOnUnique(string tablename,int offset,int type,int record_length, vector<string>& s,vector<int>& r);
    int Insert(string tablename, vector<string> value, vector<int> type, vector<int>key_type, int record_length);
	vector<string> DeleteOnlyWithIndex(string tablename, vector<int>RecordOrder, int record_length,vector<int> type);
	vector<string> DeleteWithConditionNoIndex(string tablename, int record_length, vector<Condition> condition,vector<int> type);
	void DeleteNoCondition(string tablename,int record_length);
	vector<string> DeleteWithCondition(string tablename, vector<int>RecordOrder, int record_length,vector<Condition> condition,vector<int> type);
	void Create(string tablename, int record_length);
	//record调用Buffer的 【CreateTable(stirng TableName,int Record_Length)】
	//API需要传给record （string tablename）
	void Drop(string tablename);
	//record调用Buffer的 【DropTable(string TableName)】

	//API需要传给record（string tablename,vector<string> attname,vector<int>type,vector<int> Offset,vector<int>length,int record_length)）
	//其中vector<int> Offset是指被选择的属性在一条记录中的偏移量,vector<int>length是指被选择的属性的长度
	//vector<int>type是指属性的类型，record_length是指记录长度
	//如student(ID int,name char(20)),则vector<int>Offset={0,4},vector<int>length={4，20}
	void SelectNoCondition(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, int record_length);
	//record调用Buffer的 【vector<string> GetTableRecord(string TableName)】获得所有记录
	//record利用vector<int>Offset和对应的vector<int>length将需要输出的属性从vector<string>中选出

	//API在调用record前，需调用index对条件进行筛选，生成一个存储所有满足条件的记录标号，来表示满足条件的记录是第几条记录
	//void SelectWithCondition(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, vector<int>RecordOrder, int record_length);
	//record调用Buffer的vector<string> GetTableRecord(string TableName);
	void SelectWithConditionNoIndex(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, int record_length, vector<Condition>);
	void SelectWithCondition(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, vector<int>RecordOrder, int record_length, vector<Condition> condition);
	void SelectOnlyWithIndex(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int> length, vector<int>RecordOrder, int record_length);
	void UpdateFile();

};
#endif //RECORD_H

