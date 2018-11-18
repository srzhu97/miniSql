
#include <iostream>
#include <map>
#include <sstream>
//#include "bm.h"
#include "BPlusTree.h"
#include "IndexManager.h"


bool IndexManager::InitializeTree(vector<Index> TreeInfo)
{
	for (int i = 0; i<TreeInfo.size(); i++)
		insertTree(TreeInfo[i].name, TreeInfo[i].type, TreeInfo[i].root_number, TreeInfo[i].node_number);
	return 0;
}

vector<IndexChangeInfo*> IndexManager::WriteBackTreeInfo()
{
	vector<IndexChangeInfo*> ChangeInfo;
	for (auto it = IntMap.begin(); it != IntMap.end(); it++) {
		BPlusTree<int> *Tree = it->second;
		IndexChangeInfo* Info = new IndexChangeInfo;
		Info->name = Tree->indexName;
		Info->root_number = Tree->rootNumber;
		Info->node_number = Tree->count;
		ChangeInfo.push_back(Info);
		it->second->storeTree();
		delete it->second;
	}
	for (auto it = FloatMap.begin(); it != FloatMap.end(); it++) {
		BPlusTree<float> *Tree = it->second;
		IndexChangeInfo* Info = new IndexChangeInfo;
		Info->name = Tree->indexName;
		Info->root_number = Tree->rootNumber;
		Info->node_number = Tree->count;
		ChangeInfo.push_back(Info);
		it->second->storeTree();
		delete it->second;
	}
	for (auto it = StringMap.begin(); it != StringMap.end(); it++) {
		BPlusTree<string> *Tree = it->second;
		IndexChangeInfo* Info = new IndexChangeInfo;
		Info->name = Tree->indexName;
		Info->root_number = Tree->rootNumber;
		Info->node_number = Tree->count;
		ChangeInfo.push_back(Info);
		it->second->storeTree();
		delete it->second;
	}
	return ChangeInfo;
}


bool IndexManager::insertTree(string IndexName, int type, int root, int count)
{
	IndexType[IndexName] = type;
	if (type == -1)
	{
		BPlusTree<int> *Inttree = new BPlusTree<int>(IndexName, type, bm, root, count);
		IntMap.insert(pair<string, BPlusTree<int>*>(IndexName, Inttree));
	}
	else if (type == -2)
	{
		BPlusTree<float> *Floattree = new BPlusTree<float>(IndexName, type, bm, root, count);
		FloatMap.insert(pair<string, BPlusTree<float>*>(IndexName, Floattree));
	}
	else if (type > 0)
	{
		BPlusTree<string> *Stringtree = new BPlusTree<string>(IndexName, type, bm, root, count);
		StringMap.insert(pair<string, BPlusTree<string>*>(IndexName, Stringtree));
	}
	return 0;
}

bool IndexManager::createIndex(string IndexName, int type)
{
	IndexType[IndexName] = type;
	//if (type==0 || type > 256) return 1;
	if (type == -1)
	{
		BPlusTree<int> *Inttree = new BPlusTree<int>(IndexName, type, bm);
		IntMap.insert(pair<string, BPlusTree<int>*>(IndexName, Inttree));
	}
	else if (type == -2)
	{
		BPlusTree<float> *Floattree = new BPlusTree<float>(IndexName, type, bm);
		FloatMap.insert(pair<string, BPlusTree<float>*>(IndexName, Floattree));
	}
	else if (type > 0)
	{
		BPlusTree<string> *Stringtree = new BPlusTree<string>(IndexName, type, bm);
		StringMap.insert(pair<string, BPlusTree<string>*>(IndexName, Stringtree));
	}
	return 0;
}


bool IndexManager::dropIndex(string IndexName)
{
	int type = IndexType[IndexName];
	//if (type==0 || type > 256) return 1;
	switch (type) {
	case -1:
		BPlusTree<int> *IntTree;
		IntTree = IntMap[IndexName];
		IntTree->dropTree();
		IntMap.erase(IndexName);
		delete IntTree;
		break;
	case -2:
		BPlusTree<float> *FloatTree;
		FloatTree = FloatMap[IndexName];
		FloatTree->dropTree();
		FloatMap.erase(IndexName);
		delete FloatTree;
		break;
	default:
		BPlusTree<string> *StringTree;
		StringTree = StringMap[IndexName];
		StringTree->dropTree();
		StringMap.erase(IndexName);
		delete StringTree;
		break;
	}
	IndexType.erase(IndexName);
	return 0;
}

bool IndexManager::InsertIntoIndex(string IndexName, string InsertKey, int offsetnumber)
{
	int type = IndexType[IndexName];
	//if (type==0 || type > 256) return 1;
	stringstream ss;
	switch (type) {
	case -1:
		BPlusTree<int> *IntTree;
		IntTree = IntMap[IndexName];
		ss.clear();
		ss << InsertKey;
		int InsertIntKey;
		ss >> InsertIntKey;
		IntTree->insertKey(InsertIntKey, offsetnumber);
		break;
	case -2:
		BPlusTree<float> *FloatTree;
		FloatTree = FloatMap[IndexName];
		ss.clear();
		ss << InsertKey;
		float InsertFloatKey;
		ss >> InsertFloatKey;
		FloatTree->insertKey(InsertFloatKey, offsetnumber);
		break;
	default:
		BPlusTree<string> *StringTree;
		StringTree = StringMap[IndexName];
		StringTree->insertKey(InsertKey, offsetnumber);
		break;
	}
	return 0;
}

bool IndexManager::DeleteInIndex(string IndexName, string key)
{
	int type = IndexType[IndexName];
	//if (type==0 || type > 256) return 1;
	stringstream ss;
	switch (type) {
	case -1:
		BPlusTree<int> *IntTree;
		IntTree = IntMap[IndexName];
		ss.clear();
		ss << key;
		int ConvertKey;
		ss >> ConvertKey;
		IntTree->deleteKey(ConvertKey);
		break;
	case -2:
		BPlusTree<float> *FloatTree;
		FloatTree = FloatMap[IndexName];
		ss.clear();
		ss << key;
		float ConvertFloatKey;
		ss >> ConvertFloatKey;
		FloatTree->deleteKey(ConvertFloatKey);
		break;
	default:
		BPlusTree<string> *StringTree;
		StringTree = StringMap[IndexName];
		StringTree->deleteKey(key);
		break;
	}
	return 0;
}

vector<int> IndexManager::searchIndex(string IndexName, string key, int ConditionType)
{
	int type = IndexType[IndexName];
	vector<int> OffsetResult;
	stringstream ss;
	switch (type) {
	case -1:
		BPlusTree<int> *IntTree;
		IntTree = IntMap[IndexName];
		int ConvertIntKey;
		ss.clear();
		ss << key;
		ss >> ConvertIntKey;
		IntTree->FindResult(ConvertIntKey, ConditionType, OffsetResult);
		break;
	case -2:
		BPlusTree<float> *FloatTree;
		FloatTree = FloatMap[IndexName];
		float ConvertFloatKey;
		ss.clear();
		ss << key;
		ss >> ConvertFloatKey;
		FloatTree->FindResult(ConvertFloatKey, ConditionType, OffsetResult);
		break;
	default:
		BPlusTree<string> *StringTree;
		StringTree = StringMap[IndexName];
		StringTree->FindResult(key, ConditionType, OffsetResult);
		break;
	}
	return OffsetResult;
}

bool IndexManager::DropAllTableRecord(string IndexName)
{
	int type = IndexType[IndexName];
	//if (type==0 || type > 256) return 1;
	switch (type) {
	case -1:
		BPlusTree<int> *IntTree;
		IntTree = IntMap[IndexName];
		IntTree->ClearTree();
		break;
	case -2:
		BPlusTree<float> *FloatTree;
		FloatTree = FloatMap[IndexName];
		FloatTree->ClearTree();
		break;
	default:
		BPlusTree<string> *StringTree;
		StringTree = StringMap[IndexName];
		StringTree->ClearTree();
		break;
	}
return 0;
}

#ifdef test
int main()
{
	IndexManager im;
	stringstream ss;
	//im.insertTree("first", 500, 27, 47);
	/*for (int i = 0; i <= 2000; i++)
	{
	string temp;
	ss.clear();
	ss << i;
	ss >> temp;
	remove(("first" + temp + ".txt").c_str());
	}*/
	im.createIndex("first", 500);
	im.InsertIntoIndex("first", "maxinyin", 1);
	im.InsertIntoIndex("first", "hudanchu", 2);
	im.InsertIntoIndex("first", "zhusirou", 3);
	for (int i = 0; i <= 100; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		cout << i << endl;
		ss >> temp;
		im.InsertIntoIndex("first", temp, i);
	}

	for (int i = 0; i <= 10; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		im.DeleteInIndex("first", temp);
	}
	/*
	im.insertTree("first", -1, 0, 1);
	im.insertTree("second", -2, 0, 1);
	im.insertTree("third", 250, 15, 17);

	for (int i=0; i<=100; i++)
	{
	string temp;
	ss.clear();
	ss<<i;
	ss>>temp;
	im.InsertIntoIndex("third",temp, i);
	}*/
	vector<int> result = im.searchIndex("first", "40", 0);

	for (int i = 0; i<result.size(); i++)
		cout << result[i] << endl;

	vector<IndexChangeInfo*> TreeInfo;
	TreeInfo = im.WriteBackTreeInfo();
	for (int i = 0; i<TreeInfo.size(); i++)
	{
		cout << "name:" << TreeInfo[i]->name << endl;
		cout << "root:" << TreeInfo[i]->root_number << endl;
		cout << "count:" << TreeInfo[i]->node_number << endl;
	}
}
#endif 
#ifdef TestBPlusTree
int main()
{
	BufMan bm;
	stringstream ss;

#ifdef ExamIntTree

	for (int i = 0; i <= 4552; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		remove(("first" + temp + ".txt").c_str());
	}
	BPlusTree<int> FirstTree("first", -1, bm);
	for (int i = 1; i <= 100; i++)
		FirstTree.insertKey(i, i);
	for (int i = 100; i>3; i--)
		FirstTree.deleteKey(i);
	//for (int i=1000; i>747; i--)
	// FirstTree.deleteKey(i);
	//FirstTree.printleaf();
	FirstTree.deleteKey(1);
	FirstTree.printleaf();
	FirstTree.storeTree();
#endif



#ifdef DropAllTable
	for (int i = 0; i <= 381; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		bm.DropTable("first" + temp);
	}
#endif


#ifdef ExamFloatTree
	BPlusTree<float> FirstTree("first", -2, bm);

	for (float i = 1.01; i <= 1000.01; i++)
		FirstTree.insertKey(i, 1);
	for (float i = 3000.01; i <= 3999.01; i++)
		FirstTree.insertKey(i, 1);
	for (float i = 8000.01; i <= 8999.01; i++)
		FirstTree.insertKey(i, 1);
	for (float i = 5000.01; i <= 6999.01; i++)
		FirstTree.insertKey(i, 1);
	for (float i = 10000.01; i >= 9001.01; i--)
		FirstTree.insertKey(i, 1);
	for (float i = 7999.01; i >= 7001.01; i--)
		FirstTree.insertKey(i, 1);
	for (float i = 4999.01; i >= 4001.01; i--)
		FirstTree.insertKey(i, 1);
	for (float i = 2999.01; i>1001.01; i--)
		FirstTree.insertKey(i, 1);
	for (float i = 3000.01; i >= 2000.01; i--)
		FirstTree.deleteKey(i);

	FirstTree.printleaf();
	FirstTree.storeTree();
#endif


#define ExamStringTree
#ifdef ExamStringTree
	BPlusTree<string> FirstTree("first", 500, bm);
	FirstTree.insertKey("maxinyin", 1);
	FirstTree.insertKey("hudanchu", 2);
	FirstTree.insertKey("zhusirou", 3);
	for (int i = 0; i <= 100; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		FirstTree.insertKey(temp, i);
	}

	for (int i = 0; i <= 10; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		FirstTree.deleteKey(temp);
	}

	FirstTree.printleaf();
	FirstTree.storeTree();
#endif
}
#endif



