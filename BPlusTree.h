#ifndef BPLUSTREE_H
#define BPLUSTREE_H


#include <cstdlib>
#include <vector>
#include "bm.h"
#include <list>
#include <sstream>
#include <string>

#define keytype int
#define CannotFindKey 1006

using namespace std;

template <class T>
class TreeNode {
public:
	int TreeNodeNumber;
	string Indexname;
	bool isLeaf;
	int father;
	int degree;
	int count;
	int isChange;
	int NextNode;
	vector<int> son;
	vector<T> KeyValue;

	TreeNode(string name, int NodeNumber, int NodeDegree, bool leaf);
	TreeNode(string name, int NodeNumber, int NodeDegree);
	~TreeNode() {};
	int GetNodeNumber() { return  TreeNodeNumber; }

	int FindKeyInNode(const T Key);          // Search key in the node
	int Getdegree() { return degree; };
	int Getcount() { return count; };
	int GetIsLeaf() { return isLeaf; }
	void SetIsLeaf(int status) { isLeaf = status; }
	int GetFatherPointer() { return father; }
	int SetFatherPointer(int FatherNode) { father = FatherNode; }
	void SetNextNode(int pointer) { NextNode = pointer; }
	int GetNextNode() { return NextNode; }
	bool isfull() {
		if (isLeaf)
			return count == degree;
		else return (count - 1) == degree;

	}
	bool InsertKeyInLeaf(T Key, int offsetNumber);
	int InsertInternalNode(T Key, int offsetNumber);
	bool DeleteKeyInInternal(int position, int &rem);
	bool DeleteKeyInLeaf(int position);
	void DeleteKeyInLeafByKey(const T oldkey);
	bool WriteBackToDisk(int length, BufMan* IndexBm, int type);
	bool ReadFromDisk(int length, int type, BufMan* IndexBm);
	void ConvertToKey(T &key, int &pointer, const char* origin, int length, int type);
	int FindKeyPos(const T Key);
	string GetNodeFullName();
	void DeleteNode(BufMan* IndexBm);
};


template <class T>
class BPlusTree {
private:
	int minnumber;
	BufMan IndexBm;
	int degree;  // Degree of node
	int firstLeafNumber; // Head of leaf nodes
	int length;
	int AttributeType;

public:
	BPlusTree(string Indexname, keytype type, BufMan &bm);
	BPlusTree(string Indexname, keytype type, BufMan &bm, int root, int total);
	BPlusTree() :indexName(""), rootNumber(-1), firstLeafNumber(-1), AttributeType(0) {};
	~BPlusTree() {};
	/* Inline function */
	inline int getDegree() { return degree; }
	inline int getRoot() { return rootNumber; }
	inline int getFirstLeaf() { return firstLeafNumber; }

	bool insertKey(T newkey, int offsetNumber);    // Insert an index record
	bool deleteKey(T oldkey);                       // Delete an index record
	void dropTree();                                   // Delete the whole tree
    void ClearTree();
	bool storeTree();
	bool printleaf();
	int FindKey(T Key);
	int FindNodePosition(T newkey);
	void FindResult(T Key, int ConditionType, vector<int> &OffsetResult);

	int rootNumber;
	string indexName;
	int count;

private:
	TreeNode<T> CreateNewTreeNode(int NodeID, int IsLeaf);
	bool split(TreeNode<T> FirstNode, T newkey, int offsetNumber);
	void InsertInFather(int FirstSonID, int MyID, T NewValue, int pointer);
	int splitInternalNode(TreeNode<T> FirstNode, T newkey, int offsetNumber);
	bool ChangeKeyInInternal(int ParentNumber, T KeyChangeInParent, int SonOff);
	bool DeleteInInternal(int ParentNumber, int SonOff);

};

template <class T>
TreeNode<T>::TreeNode(string name, int NodeNumber, int NodeDegree)
{
	Indexname = name;
	TreeNodeNumber = NodeNumber;
	isLeaf = -1;
	father = -1;
	degree = NodeDegree;
	count = 0;
	isChange = 0;
	KeyValue.resize(NodeDegree + 1, T());
	son.resize(NodeDegree + 1, int());
	NextNode = -1;
}

template <class T>
TreeNode<T>::TreeNode(string name, int NodeNumber, int NodeDegree, bool leaf)
{
	Indexname = name;
	TreeNodeNumber = NodeNumber;
	isLeaf = leaf;
	father = -1;
	degree = NodeDegree;
	count = 0;
	isChange = 0;
	KeyValue.resize(NodeDegree + 1, T());
	son.resize(NodeDegree + 1, int());
	NextNode = -1;
}

template <class T>
bool TreeNode<T>::InsertKeyInLeaf(T Key, int offsetNumber)
{
	if (count == 0) {
		KeyValue[0] = Key;
		son[0] = offsetNumber;
		count++;
		return 0;
	}
	int pos = FindKeyInNode(Key);
	for (int i = count; i>pos; i--)
	{
		son[i] = son[i - 1];
		KeyValue[i] = KeyValue[i - 1];
	}
	KeyValue[pos] = Key;
	son[pos] = offsetNumber;
	count++;
	return 0;
}

template <class T>
int TreeNode<T>::FindKeyInNode(const T Key)
{
	int left = 0, right = count;
	if (Key < KeyValue[0]) return 0;
	while (right - left != 1)
	{
		int middle = (left + right) / 2;
		if (KeyValue[middle] < Key) left = middle;
		if (KeyValue[middle] > Key) right = middle;
	}
	return right;
}

template <class T>
int TreeNode<T>::FindKeyPos(const T Key)
{
	int left = 0, right = count;
	while (right != left)
	{
		int middle = (left + right) / 2;
		if (KeyValue[middle] == Key) return son[middle];
		if (KeyValue[middle] < Key) left = middle;
		if (KeyValue[middle] > Key) right = middle;
	}
	return CannotFindKey;
}

template <class T>
int TreeNode<T>::InsertInternalNode(T Key, int offsetNumber)
{
	if (count == 1) {
		KeyValue[0] = Key;
		son[1] = offsetNumber;
		count++;
		return 0;
	}
	count--;
	int remain = -1;
	if (count == degree)
	{
		remain = son[degree];
	}
	int pos = FindKeyInNode(Key);
	for (int i = count - 1; i >= pos; i--)
	{
		KeyValue[i + 1] = KeyValue[i];
		son[i + 2] = son[i + 1];
	}
	KeyValue[pos] = Key;
	if (pos != degree) son[pos + 1] = offsetNumber;
	count += 2;
	return remain;
}

template <class T>
bool TreeNode<T>::DeleteKeyInLeaf(int position)
{
	for (int i = position; i<count - 1; i++)
	{
		KeyValue[i] = KeyValue[i + 1];
		son[i] = son[i + 1];
	}
	KeyValue[count - 1] = -1;
	son[count - 1] = -1;
	count--;
	return 0;
}

template <class T>
bool TreeNode<T>::DeleteKeyInInternal(int position, int &rem)
{
	for (int i = position; i<count - 2; i++)
	{
		KeyValue[i] = KeyValue[i + 1];
		if (i == degree - 1) { son[i + 1] = rem; rem = -1; break; }
		else son[i + 1] = son[i + 2];
	}
	count--;
	return 0;
}

template <class T>
bool TreeNode<T>::WriteBackToDisk(int length, BufMan* IndexBm, int type)
{
	stringstream sstream;
	string output, name;

	sstream.clear();
	sstream << TreeNodeNumber;
	sstream >> output;
	name = Indexname + output;
	char* temp = new char[length];
	for (int i = 0; i<length; i++)
		temp[i] = '\0';
	IndexBm->CleanBuffer(name);
	memcpy(temp, &isLeaf, length);
	IndexBm->AddRecord(name, temp, length);
	memcpy(temp, &father, length);
	IndexBm->AddRecord(name, temp, length);
	memcpy(temp, &NextNode, length);
	IndexBm->AddRecord(name, temp, length);
	//if (!isLeaf) count--;
	for (int i = 0; i<count; i++)
	{
		//memset(temp, length, 0);
		if (type>0) {
			stringstream ss;
			ss.clear();
			ss << KeyValue[i];
			ss >> temp;
		}
		else {
			memcpy(temp, &KeyValue[i], (length - 4));
		}
		int offset = length - 4;
		memcpy(temp + offset, &son[i], sizeof(int));
		IndexBm->AddRecord(name, temp, length);
	}
	//if (!isLeaf) count++;
	delete[] temp;
	return 0;
}

template <class T>
bool TreeNode<T>::ReadFromDisk(int length, int type, BufMan *IndexBm)
{
	stringstream sstream;
	string output, name;

	sstream.clear();
	sstream << TreeNodeNumber;
	sstream >> output;
	name = Indexname + output;
	vector<string> result = IndexBm->GetTableRecord(name, length);
	vector<const char*> ConvertResult;
	for (int i = 0; i<result.size(); i++)
	{
		ConvertResult.push_back(result[i].c_str());
	}
	memcpy(&isLeaf, ConvertResult[0], sizeof(int));
	memcpy(&father, ConvertResult[1], sizeof(int));
	memcpy(&NextNode, ConvertResult[2], sizeof(int));
	int i;
	for (i = 3; i<result.size(); i++)
		ConvertToKey(KeyValue[i - 3], son[i - 3], ConvertResult[i], length, type);
	count = i - 3;
	//    vector<T>().swap(result);
	return 0;
}

template <class T>
void TreeNode<T>::ConvertToKey(T &key, int &pointer, const char* origin, int length, int type)
{
	if (type > 0)
	{
		stringstream ss;
		ss.clear();
		ss << origin;
		ss >> key;
		memcpy(&pointer, origin + length - 4, sizeof(int));
		return;
	}
	int offset;
	if (type < 0) offset = 4;
	else offset = type;
	memcpy(&key, origin, offset);
	memcpy(&pointer, origin + offset, sizeof(int));
}

template <class T>
string TreeNode<T>::GetNodeFullName()
{
	stringstream sstream;
	string output, name;

	sstream.clear();
	sstream << TreeNodeNumber;
	sstream >> output;
	name = Indexname + output;
	return name;
}


template <class T>
BPlusTree<T>::BPlusTree(string Indexname, keytype type, BufMan &bm) :indexName(Indexname), AttributeType(type)
{
	rootNumber = -1;
	firstLeafNumber = -1;
	count = 0;
	if (type <0) length = 4;
	else length = type + 1;
	length += 4;
	degree = (Blocksize - 8 - 3 * length) / length - 1;
	minnumber = (degree - 1) / 2;
	IndexBm = bm;
}

template <class T>
BPlusTree<T>::BPlusTree(string Indexname, keytype type, BufMan &bm, int root, int total)
{
	indexName = Indexname;
	AttributeType = type;
	rootNumber = root;
	firstLeafNumber = 0;
	count = total;
	if (type <0) length = 4;
	else length = type + 1;
	length += 4;
	degree = (Blocksize - 8 - 3 * length) / length - 1;
	minnumber = (degree - 1) / 2;
	IndexBm = bm;
}



template <class T>
bool BPlusTree<T>::insertKey(T newkey, int offsetNumber)
{
	if (rootNumber == -1) {
		TreeNode<T> node = CreateNewTreeNode(0, 1);
		rootNumber = 0;
		count++;
		firstLeafNumber = 0;
		string name = node.GetNodeFullName();
		IndexBm.CreateTable(name, length, 0);
		node.InsertKeyInLeaf(newkey, offsetNumber);
		node.WriteBackToDisk(length, &IndexBm, AttributeType);
		firstLeafNumber = 0;
		node.SetNextNode(-1);
		return 0;
	}
	else {
		int NodeNumber = FindNodePosition(newkey);
		TreeNode<T> temp(indexName, NodeNumber, degree);
		temp.ReadFromDisk(length, AttributeType, &IndexBm);
		if (!temp.isfull()) {
			temp.InsertKeyInLeaf(newkey, offsetNumber);
			temp.WriteBackToDisk(length, &IndexBm, AttributeType);
		}
		else {
			split(temp, newkey, offsetNumber);
		}
	}
	return 0;
}


template <class T>
TreeNode<T> BPlusTree<T>::CreateNewTreeNode(int NodeID, int IsLeaf)
{
	TreeNode<T> node(indexName, NodeID, degree);
	node.SetIsLeaf(IsLeaf);
	return node;
}


template <class T>
int BPlusTree<T>::FindNodePosition(T newkey)
{
	int head = rootNumber;
	if (head == 0) return 0;
	while (1)
	{
		TreeNode<T> parent(indexName, head, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		if (parent.GetIsLeaf())break;
		int i;
		for (i = 0; i<parent.count - 1; i++)
		{
			if (newkey < parent.KeyValue[i])
				break;
		}
		head = parent.son[i];
	}
	return head;
}


template <class T>
bool BPlusTree<T>::split(TreeNode<T> FirstNode, T newkey, int offsetNumber)
{
	TreeNode<T> SecondNode = CreateNewTreeNode(count, 1);
	count++;
	SecondNode.SetIsLeaf(1);
	string name = SecondNode.GetNodeFullName();
	IndexBm.CreateTable(name, length, 0);

	FirstNode.InsertKeyInLeaf(newkey, offsetNumber);
	int i = (degree + 3) / 2;
	for (; i <= degree; i++)
		SecondNode.InsertKeyInLeaf(FirstNode.KeyValue[i], FirstNode.son[i]);
	int cal = (degree + 3) / 2;
	for (i = degree; i >= cal; i--)
		FirstNode.DeleteKeyInLeaf(i);
	SecondNode.SetNextNode(FirstNode.NextNode);
	FirstNode.SetNextNode(count - 1);
	if (FirstNode.father == -1)  SecondNode.father = FirstNode.father = count;
	else SecondNode.father = FirstNode.father;
	FirstNode.WriteBackToDisk(length, &IndexBm, AttributeType);
	SecondNode.WriteBackToDisk(length, &IndexBm, AttributeType);
	if (FirstNode.father == count)
		InsertInFather(FirstNode.TreeNodeNumber, -1, SecondNode.KeyValue[0], count - 1);
	else
		InsertInFather(FirstNode.TreeNodeNumber, FirstNode.father, SecondNode.KeyValue[0], count - 1);
	return 1;
}


template <class T>
void BPlusTree<T>::InsertInFather(int FirstSonID, int MyID, T NewValue, int pointer)
{
	if (MyID == -1) {
		TreeNode<T> Root = CreateNewTreeNode(count, 0);
		rootNumber = count;
		count++;
		//        cout << count;
		string name = Root.GetNodeFullName();
		IndexBm.CreateTable(name, length, 0);
		Root.son[0] = FirstSonID;
		Root.count++;
		Root.InsertInternalNode(NewValue, pointer);
		Root.WriteBackToDisk(length, &IndexBm, AttributeType);
		return;
	}
	TreeNode<T> node(indexName, MyID, degree);
	node.ReadFromDisk(length, AttributeType, &IndexBm);
	if (node.isfull() == 0) {
		node.InsertInternalNode(NewValue, pointer);
		node.WriteBackToDisk(length, &IndexBm, AttributeType);
	}
	else {
		if (node.GetFatherPointer() == -1)
		{
			TreeNode<T> RootNode = CreateNewTreeNode(count, 0);
			rootNumber = count;
			node.father = count;
			count++;
			//   cout << count;
			string name = RootNode.GetNodeFullName();
			IndexBm.CreateTable(name, length, 0);
			RootNode.son[0] = MyID;
			RootNode.count++;
			RootNode.WriteBackToDisk(length, &IndexBm, AttributeType);
			splitInternalNode(node, NewValue, pointer);
		}
		else
			splitInternalNode(node, NewValue, pointer);
	}
}


template <class T>
int BPlusTree<T>::splitInternalNode(TreeNode<T> FirstNode, T newkey, int offsetNumber)
{
	TreeNode<T> SecondNode = CreateNewTreeNode(count, 1);
	SecondNode.SetIsLeaf(0);
	count++;
	SecondNode.father = FirstNode.father;
	string name = SecondNode.GetNodeFullName();
	IndexBm.CreateTable(name, length, 0);


	int rem = FirstNode.InsertInternalNode(newkey, offsetNumber);
	if (newkey == FirstNode.KeyValue[degree]) rem = offsetNumber;
	int i = (degree + 2) / 2;
	T temp = FirstNode.KeyValue[i];

	SecondNode.son[0] = FirstNode.son[i + 1];
	SecondNode.count++;
	FirstNode.DeleteKeyInInternal(i, rem);

	int sig = i;
	for (; i < degree; i++)
	{
		SecondNode.InsertInternalNode(FirstNode.KeyValue[sig], FirstNode.son[sig + 1]);
		FirstNode.DeleteKeyInInternal(sig, rem);
	}

	for (i = 0; i<SecondNode.count; i++)
	{
		TreeNode<T> SonNode(indexName, SecondNode.son[i], degree);
		SonNode.ReadFromDisk(length, AttributeType, &IndexBm);
		SonNode.father = SecondNode.TreeNodeNumber;
		SonNode.WriteBackToDisk(length, &IndexBm, AttributeType);
	}

	FirstNode.WriteBackToDisk(length, &IndexBm, AttributeType);
	SecondNode.WriteBackToDisk(length, &IndexBm, AttributeType);

	InsertInFather(FirstNode.TreeNodeNumber, SecondNode.father, temp, count - 1);  //////
	return count - 1;
}


template <class T>
bool BPlusTree<T>::storeTree()
{
	TreeNode<T> Son(indexName, 0, degree);
	Son.ReadFromDisk(length, AttributeType, &IndexBm);
	IndexBm.StoreBuffer();
	return 0;
}


template <class T>
void BPlusTree<T>::dropTree()
{
	stringstream ss;
	for (int i = 0; i<count; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		string name = indexName + temp;
		IndexBm.DropTable(name);
	}
}

template <class T>
void BPlusTree<T>::ClearTree()
{
	stringstream ss;
	for (int i = 0; i<count; i++)
	{
		string temp;
		ss.clear();
		ss << i;
		ss >> temp;
		string name = indexName + temp;
		IndexBm.DropTable(name);
	}
	firstLeafNumber = -1;
	rootNumber = -1;
	count = 0;
}

template <class T>
bool BPlusTree<T>::printleaf()
{
	/*int first = firstLeafNumber;
	while (first != -1)
	{
		TreeNode<T> parent(indexName, first, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		for (int i = 0; i<parent.count; i++)
			cout << parent.KeyValue[i] << endl;
		cout << "This Node:" << parent.TreeNodeNumber << endl;
		cout << "Next Node:" << parent.NextNode << endl;
		cout << "Father Node:" << parent.father << endl;
		cout << endl;

		first = parent.NextNode;
	}

	return 0;*/
}


template <class T>
int BPlusTree<T>::FindKey(T Key)
{
	int pos = FindNodePosition(Key);
	TreeNode<T> node(indexName, pos, degree);
	node.ReadFromDisk(length, AttributeType, &IndexBm);
	return node.FindKeyPos(Key);
}

template <class T>
bool BPlusTree<T>::deleteKey(T oldkey)                       // Delete an index record
{

	int pos = rootNumber;
	if (pos == 0) {
		TreeNode<T> leaf(indexName, 0, degree);
		leaf.ReadFromDisk(length, AttributeType, &IndexBm);
		leaf.DeleteKeyInLeafByKey(oldkey);
		if (leaf.count == 0) {
			rootNumber = -1;
			firstLeafNumber = -1;
			leaf.DeleteNode(&IndexBm);
			return 0;
		}
		leaf.WriteBackToDisk(length, &IndexBm, AttributeType);
		return 0;
	}

	/*----------  find key in leaf  ----------*/
	int ParentNumber = rootNumber, SiblingNumber = 0;
	while (1)
	{
		TreeNode<T> parent(indexName, pos, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		if (parent.GetIsLeaf())break;
		int i;
		for (i = 0; i<parent.count - 1; i++)
		{
			if (oldkey < parent.KeyValue[i])
			{
				if (i != 0) SiblingNumber = parent.son[i - 1];
				else SiblingNumber = parent.son[1];
				break;
			}
		}
		ParentNumber = pos;
		pos = parent.son[i];
		if (i == parent.count - 1) SiblingNumber = parent.son[i - 1];
	}
	TreeNode<T> node(indexName, pos, degree);
	node.ReadFromDisk(length, AttributeType, &IndexBm);
	node.DeleteKeyInLeafByKey(oldkey);
	if (node.count < (degree + 1) / 2)
	{
		TreeNode<T> sibling(indexName, SiblingNumber, degree);
		sibling.ReadFromDisk(length, AttributeType, &IndexBm);
		T KeyChangeInParent;
		int SonOff;
		if (node.count + sibling.count <= degree)
		{
			if (node.KeyValue[0] < sibling.KeyValue[0])
			{
				int OriginNumber = node.count;
				for (int i = 0; i<sibling.count; i++)
				{
					node.KeyValue[OriginNumber + i] = sibling.KeyValue[i];
					node.son[OriginNumber + i] = sibling.son[i];
				}
				SonOff = sibling.TreeNodeNumber;
				node.NextNode = sibling.NextNode;
				node.count += sibling.count;
				sibling.DeleteNode(&IndexBm);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			else
			{
				int OriginNumber = sibling.count;
				for (int i = 0; i<node.count; i++)
				{
					sibling.KeyValue[OriginNumber + i] = node.KeyValue[i];
					sibling.son[OriginNumber + i] = node.son[i];
				}
				SonOff = node.TreeNodeNumber;
				sibling.NextNode = node.NextNode;
				sibling.count += node.count;
				node.DeleteNode(&IndexBm);
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			DeleteInInternal(ParentNumber, SonOff);
			return 0;
		}
		else {
			if (node.KeyValue[0] < sibling.KeyValue[0]) {
				SonOff = node.TreeNodeNumber;
				node.KeyValue[node.count] = sibling.KeyValue[0];
				node.son[node.count] = sibling.son[0];
				node.count++;
				sibling.DeleteKeyInLeaf(0);
				KeyChangeInParent = sibling.KeyValue[0];
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			else {
				int FinalPos = sibling.count - 1;
				SonOff = sibling.TreeNodeNumber;
				for (int i = node.count; i>0; i--)
				{
					node.KeyValue[i] = node.KeyValue[i - 1];
					node.son[i] = node.son[i - 1];
				}
				node.KeyValue[0] = sibling.KeyValue[FinalPos];
				node.son[0] = sibling.son[FinalPos];
				sibling.count--;
				node.count++;
				KeyChangeInParent = node.KeyValue[0];
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			ChangeKeyInInternal(ParentNumber, KeyChangeInParent, SonOff);
			return 0;
		}
	}
	node.WriteBackToDisk(length, &IndexBm, AttributeType);
	return 0;
}


template <class T>
bool BPlusTree<T>::DeleteInInternal(int ParentNumber, int SonOff)
{
	TreeNode<T> node(indexName, ParentNumber, degree);
	node.ReadFromDisk(length, AttributeType, &IndexBm);
	for (int i = 1; i<node.count; i++)
	{
		if (SonOff == node.son[i]) {
			int rm = -1;
			node.DeleteKeyInInternal(i - 1, rm);
			break;
		}
	}
	if (node.count == 1 && node.TreeNodeNumber == rootNumber)
	{
		rootNumber = node.son[0];
		node.DeleteNode(&IndexBm);
		return 0;
	}
	if (node.count < (degree + 1) / 2 && node.TreeNodeNumber != rootNumber)
	{

		int SiblingNumber;
		TreeNode<T> parent(indexName, node.father, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		T MiddleKeyValue;
		if (parent.son[0] == ParentNumber)
		{
			SiblingNumber = parent.son[1];
			MiddleKeyValue = parent.KeyValue[0];
		}
		else
		{
			for (int i = 1; i<count; i++)
				if (parent.son[i] == ParentNumber)
				{
					MiddleKeyValue = parent.KeyValue[i - 1];
					SiblingNumber = parent.son[i - 1]; break;
				}
		}
		TreeNode<T> sibling(indexName, SiblingNumber, degree);
		sibling.ReadFromDisk(length, AttributeType, &IndexBm);

		if (node.count + sibling.count <= degree + 1)
		{
			int SonOff;
			if (node.KeyValue[0] < sibling.KeyValue[0])
			{
				int OriginNumber = node.count, i;
				for (i = 0; i<sibling.count - 1; i++)
				{
					node.KeyValue[OriginNumber + i] = sibling.KeyValue[i];
					node.son[OriginNumber + i] = sibling.son[i];
				}
				node.son[OriginNumber + i] = sibling.son[i];
				node.KeyValue[OriginNumber - 1] = MiddleKeyValue;
				SonOff = sibling.TreeNodeNumber;
				node.NextNode = sibling.NextNode;
				node.count += sibling.count;
				for (int i = 0; i<sibling.count; i++)
				{
					TreeNode<T> ChangeSon(indexName, sibling.son[i], degree);
					ChangeSon.ReadFromDisk(length, AttributeType, &IndexBm);
					ChangeSon.father = node.TreeNodeNumber;
					ChangeSon.WriteBackToDisk(length, &IndexBm, AttributeType);
				}
				sibling.DeleteNode(&IndexBm);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			else
			{
				int OriginNumber = sibling.count, i;
				for (i = 0; i<node.count - 1; i++)
				{
					sibling.KeyValue[OriginNumber + i] = node.KeyValue[i];
					sibling.son[OriginNumber + i] = node.son[i];
				}
				sibling.KeyValue[OriginNumber - 1] = MiddleKeyValue;
				sibling.son[OriginNumber + i] = node.son[i];
				SonOff = node.TreeNodeNumber;
				sibling.NextNode = node.NextNode;
				sibling.count += node.count;
				for (int i = 0; i<node.count; i++)
				{
					TreeNode<T> ChangeSon(indexName, node.son[i], degree);
					ChangeSon.ReadFromDisk(length, AttributeType, &IndexBm);
					ChangeSon.father = sibling.TreeNodeNumber;
					ChangeSon.WriteBackToDisk(length, &IndexBm, AttributeType);
				}
				node.DeleteNode(&IndexBm);
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			DeleteInInternal(node.father, SonOff);
			return 0;
		}
		else {
			T KeyChangeInParent;
			if (node.KeyValue[0] < sibling.KeyValue[0]) {
				SonOff = node.TreeNodeNumber;

				node.KeyValue[node.count - 1] = MiddleKeyValue;
				node.son[node.count] = sibling.son[0];
				node.count++;
				KeyChangeInParent = sibling.KeyValue[0];

				TreeNode<T> ChangeSon(indexName, sibling.son[0], degree);
				ChangeSon.ReadFromDisk(length, AttributeType, &IndexBm);
				ChangeSon.father = node.TreeNodeNumber;
				ChangeSon.WriteBackToDisk(length, &IndexBm, AttributeType);

				sibling.son[0] = sibling.son[1];
				int rm = -1;
				sibling.DeleteKeyInInternal(0, rm);
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			else {
				SonOff = sibling.TreeNodeNumber;

				for (int i = node.count - 1; i>1; i--)
				{
					node.son[i] = node.son[i - 1];
					node.KeyValue[i - 1] = node.KeyValue[i - 2];
				}
				node.son[1] = node.son[0];
				node.KeyValue[0] = MiddleKeyValue;
				int FinalPos = sibling.count - 1;
				node.son[0] = sibling.son[FinalPos];
				node.count++;

				TreeNode<T> ChangeSon(indexName, node.son[0], degree);
				ChangeSon.ReadFromDisk(length, AttributeType, &IndexBm);
				ChangeSon.father = node.TreeNodeNumber;
				ChangeSon.WriteBackToDisk(length, &IndexBm, AttributeType);

				sibling.count--;

				KeyChangeInParent = sibling.KeyValue[FinalPos - 1];
				sibling.WriteBackToDisk(length, &IndexBm, AttributeType);
				node.WriteBackToDisk(length, &IndexBm, AttributeType);
			}
			ChangeKeyInInternal(node.father, KeyChangeInParent, SonOff);
			return 0;
		}
	}
	node.WriteBackToDisk(length, &IndexBm, AttributeType);
	return 0;
}

template <class T>
bool BPlusTree<T>::ChangeKeyInInternal(int ParentNumber, T KeyChangeInParent, int SonOff)
{
	TreeNode<T> node(indexName, ParentNumber, degree);
	node.ReadFromDisk(length, AttributeType, &IndexBm);
	for (int i = 0; i<node.count - 1; i++)
		if (SonOff == node.son[i]) {
			node.KeyValue[i] = KeyChangeInParent;
			break;
		}
	node.WriteBackToDisk(length, &IndexBm, AttributeType);
	return 0;
}

template <class T>
void TreeNode<T>::DeleteKeyInLeafByKey(const T oldkey)
{
	int i;
	for (i = 0; i<count; i++)
	{
		if (KeyValue[i] == oldkey) break;
	}
	count--;
	for (; i<count; i++)
	{
		KeyValue[i] = KeyValue[i + 1];
		son[i] = son[i + 1];
	}
}

template <class T>
void TreeNode<T>::DeleteNode(BufMan* IndexBm)
{
	string ID;
	stringstream ss;
	ss << TreeNodeNumber;
	ss >> ID;
	string name = Indexname + ID;

	IndexBm->DropTable(name);
}


template <class T>
void BPlusTree<T>::FindResult(T Key, int ConditionType, vector<int> &OffsetResult)
{
	if (ConditionType == 0) {
		/*----------  <  ----------*/
		int first = firstLeafNumber;
		while (first != -1)
		{
			TreeNode<T> parent(indexName, first, degree);
			parent.ReadFromDisk(length, AttributeType, &IndexBm);
			for (int i = 0; i<parent.count; i++)
			{
				if (parent.KeyValue[i] < Key)
					OffsetResult.push_back(parent.son[i]);
				if (parent.KeyValue[i] >= Key)
					return;
			}
			first = parent.NextNode;
		}
	}
	else if (ConditionType == 1)
	{
		/*----------  <=  ----------*/
		int first = firstLeafNumber;
		while (first != -1)
		{
			TreeNode<T> parent(indexName, first, degree);
			parent.ReadFromDisk(length, AttributeType, &IndexBm);
			for (int i = 0; i<parent.count; i++)
			{
				if (parent.KeyValue[i] <= Key)
					OffsetResult.push_back(parent.son[i]);
				if (parent.KeyValue[i] > Key)
					return;
			}
			first = parent.NextNode;
		}
	}
	else if (ConditionType == 2)
	{
		/*----------  >  ----------*/
		int block = FindNodePosition(Key), i;
		TreeNode<T> parent(indexName, block, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		for (i = 0; i<parent.count; i++)
		{
			if (parent.KeyValue[i] == Key)
				break;
            if (parent.KeyValue[i] > Key)
            {
                i--;
                break;
            }
		}
		i = i + 1;
		for (; i<parent.count; i++)
			OffsetResult.push_back(parent.son[i]);
		block = parent.NextNode;
		while (block != -1)
		{
			TreeNode<T> parent(indexName, block, degree);
			parent.ReadFromDisk(length, AttributeType, &IndexBm);
			for (int i = 0; i<parent.count; i++)
				OffsetResult.push_back(parent.son[i]);
			block = parent.NextNode;
		}
	}
	else if (ConditionType == 3)
	{
		/*----------  >=  ----------*/
		int block = FindNodePosition(Key), i;
		TreeNode<T> parent(indexName, block, degree);
		parent.ReadFromDisk(length, AttributeType, &IndexBm);
		for (i = 0; i<parent.count; i++)
		{
            if (parent.KeyValue[i] >= Key)
                break;
		}
		for (; i<parent.count; i++)
			OffsetResult.push_back(parent.son[i]);
		block = parent.NextNode;
		while (block != -1)
		{
			TreeNode<T> parent(indexName, block, degree);
			parent.ReadFromDisk(length, AttributeType, &IndexBm);
			for (int i = 0; i<parent.count; i++)
				OffsetResult.push_back(parent.son[i]);
			block = parent.NextNode;
		}
	}
	else if (ConditionType == 4)
	{
		/*----------  ==  ----------*/
		int offset = FindKey(Key);
		OffsetResult.push_back(offset);
	}
	else if (ConditionType == 5)
	{
		/*----------  <>  ----------*/
		int first = firstLeafNumber;
		while (first != -1)
		{
			TreeNode<T> parent(indexName, first, degree);
			parent.ReadFromDisk(length, AttributeType, &IndexBm);
			for (int i = 0; i<parent.count; i++)
				if (parent.KeyValue[i] != Key)
					OffsetResult.push_back(parent.son[i]);
			first = parent.NextNode;
		}
	}
	return;
}

#endif // !BPLUSTREE_H

