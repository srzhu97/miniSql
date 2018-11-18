#include <iostream>
#include <string>
#include <fstream>

#include "Catalog.h"
#include <vector>
CatalogManager::CatalogManager() {
	int flag = 0;
	string s1, s;
	int count = -1;
	ifstream in("tables.txt");
	if (!in) {
		//cout<<"Fail to open tables.txt when read dictionary information!"<<endl;
		ofstream out("tables.txt");
		out.close();
		flag = 1;
	}
	in >> s1;
	while (in >> s&&in.peek() != EOF) {
		Table t;
		//cout<<count<<endl;
		if (s1 == "TableName" || s == "TableName") {
			Attribute a;
			int len = 0;
			if (s == "TableName")
				in >> s;
			t.name = s;
			in >> s;
			in >> s;
			t.isprimary = 0;
			if (s == "TRUE") {
				t.isprimary = 1;
				in >> s; in >> s;
				t.primary = s;
				in >> s; in >> s;
			}
			else {
				in >> s; in >> s; in >> s;
			}
			a.key_type = 0;
			while (s != "ColumnNumber:") {
				if (a.key_type == 1) {
					in >> s;
				}
				if (s == "ColumnNumber:")
					break;
				a.name = s;
				//cout<<s;
				a.key_type = 0;
				if (s == t.primary)
					a.key_type = -1;
				in >> s;
				a.offset = len;
				if (s[0] == 'i') { a.type = -1; len += 4; }
				if (s[0] == 'f') { a.type = -2; len += 4; }
				if (s[0] == 'c') {
					char z[12];
					for (int i = 5; s[i] != ')'; i++)
						z[i - 5] = s[i];
					sscanf(z, "%d", &a.type);
					//cout<<a.type;
					len += a.type;
				}
				in >> s;
				if (s == "unique") {
					a.key_type = 1;
				}
				t.attr.push_back(a);
			}
			in >> s; in >> s; in >> s;
			sscanf(s.c_str(), "%u", &t.recordlength);
			ctable.push_back(t);
		}
	}
	in.close();
	ifstream in2("indexes.txt");
	if (!in2) {
		//cout<<"Fail to open indexes.txt when read dictionary information!"<<endl;
		ofstream out2("indexes.txt");
		out2.close();
		flag = 1;
	}
	if (flag) {
		return;
	}
	int count2 = -1;
	while (in2 >> s&&in2.peek() != EOF) {
		Index i;
		in2 >> s;
		i.name = s;
		in2 >> s; in2 >> s; in2 >> s; in2 >> s;
		i.tablename = s;
		in2 >> s;
		char *aname = new char[s.length() - 1];
		int ri;
		for (ri = 0; s[ri + 1] != ')'; ri++) {
			aname[ri] = s[ri + 1];
		}
		aname[ri] = '\0';
		i.attrname = (string)aname;
		in2 >> s; in2 >> s; in2 >> s;
		if (s[0] == 'i') { i.type = -1; }
		if (s[0] == 'f') { i.type = -2; }
		if (s[0] == 'c') {
			char z[12];
			for (int i = 5; s[i] != '\0'; i++)
				z[i - 5] = s[i];
			sscanf(z, "%d", &i.type);
		}
		in2 >> s; in2 >> s;
		if (s == "TRUE") i.isprimary = true;
		else i.isprimary = false;
		in2 >> s; in2 >>s;
		sscanf(s.c_str(),"%d",&i.AttrNameOffset);
		in2 >> s; in2 >> s;
		sscanf(s.c_str(), "%d", &i.root_number);
		in2 >> s; in2 >> s;
		sscanf(s.c_str(), "%d", &i.node_number);
		cindex.push_back(i);
	}
	in2.close();
}
//判断表是否存在，若存在则返回表的number,-1代表不存在
int CatalogManager::IsTableExist(string tablename) {
	vector<Table>::iterator it;
	int count = 0;
	if (ctable.size() == 0) {
		return -1;
	}
	for (it = ctable.begin(); it != ctable.end(); it++, count++) {
		if ((*it).name == tablename) {
			return count;
		}
	}
	return -1;
}
//判断索引是否存在,若存在则返回索引的number,-1代表不存在名为indexname的索引
int CatalogManager::IsIndexExist(string indexname) {
	vector<Index>::iterator it;
	int count = 0;
	for (it = cindex.begin(); it != cindex.end(); it++, count++) {
		if ((*it).name == indexname) {
			return count;
		}
	}
	return -1;
}
//判断表上的属性是否存在,返回-1代表表不存在，返回-2代表属性不存在
int CatalogManager::IsAttrExist(string tablename, string attrname) {
	int tablenum = IsTableExist(tablename);
	if (tablenum >= 0) {
		vector<Attribute>::iterator it;
		for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
			if ((*it).name == attrname) {
				return tablenum;
			}
		}
	}
	else {
		cout << "There is no table named " << tablename << endl;
		return -1;
	}
	cout << "There is no attribute named " << attrname << " on table " << "\"" << tablename << "\"" << endl;
	return -2;
}
//判断是否成功创建表,-1表示表已经存在，num>=0表示主键在记录中的偏移量
int CatalogManager::CreateTable(string tablename, vector<Attribute> attrs,short int HasPrimary) {
	int tableexist = IsTableExist(tablename);
	if (tableexist != -1) {
		cout << "There is already a table named " << tablename << "!" << endl;
		cout << "You can't redefine an existed table!" << endl;
		return -1;
	}
    cout<<HasPrimary<<endl;
	Table t;
	int num, PrimaryOff;   //+ primaryoff
	string primary;
	t.name = tablename;
	//t.recordnum=0;
	unsigned int length = 0;
	vector<Attribute>::iterator it;
	for (it = attrs.begin(); it != attrs.end(); it++) {
		if ((*it).key_type == -1) {
			t.primary = (*it).name;
            PrimaryOff = (int)(it - attrs.begin());
			primary = t.primary;
			t.isprimary = true;
			//offset=length;
		}
		int type = (*it).type;
		if (type == -1 || type == -2)
			length += 4;
		else
			length += type;
		t.attr.push_back(*it);
	}
	t.recordlength = length;
	ctable.push_back(t);
	//UpdateTable();
	if(HasPrimary)
		num = CreateIndex(tablename, primary, tablename + "_primary_index", PrimaryOff);
	cout << "Create table \"" << tablename << "\" successfully!" << endl;
	return num;
}
void CatalogManager::UpdateTable() {
	ofstream out("tables.txt");
	vector<Attribute>::iterator it;
	vector<Table>::iterator it2;
	for (it2 = ctable.begin(); it2 != ctable.end(); it2++) {
		out << "TableName" << "\t" << (*it2).name << endl;
		if ((*it2).isprimary == true)
			out << "HasPrimary: " << "TRUE" << "\n";
		else out << "HasPrimary: " << "FALSE" << "\n";
		out << "PrimaryKey" << "\t" << (*it2).primary << endl;
		out << "AttributeList:" << endl;
		for (it = (*it2).attr.begin(); it != (*it2).attr.end(); it++) {
			out << "\t" << (*it).name << "\t";
			int type = (*it).type;
			if (type == -1)
				out << "int";
			if (type == -2)
				out << "float";
			if (type >= 1 && type <= 255)
				out << "char(" << type << ")";
			if ((*it).key_type == 1)
				out << "\t" << "unique";
			out << endl;
		}
		out << "ColumnNumber: " << (*it2).attr.size() << endl;
		//out<<"RecordNumber: "<<(*it2).recordnum<<endl;
		out << "RecordLength: " << (*it2).recordlength << endl;
	}
	out.close();
}
//判断是否成功删除表
bool CatalogManager::DropTable(string tablename) {
	int tableexist = IsTableExist(tablename);
	if (tableexist == -1) {
		cout << "There is no table named " << tablename << "!" << endl;
		cout << "You can't delete a table not existing!" << endl;
		return false;
	}
	ctable.erase(ctable.begin() + tableexist);
	//UpdateTable();
	int indexnum = DropTableIndex(tablename);
	cout << "Drop table \"" << tablename << "\" successfully!" << endl;
	return true;
}
//判断是否成功创建索引,成功则返回此索引对应的属性的偏移量,失败则返回-1
int CatalogManager::CreateIndex(string tablename, string attrname, string indexname, int AttributeOff) {
	int indexexist = IsIndexExist(indexname);
	if (indexexist >= 0) {
		cout << "You can't redefine existed index!" << endl;
		return -1;
	}
	int tablenum = IsTableExist(tablename);
	int type = IsAttrUnique_Primary(tablename, attrname);
	if (type == 3) {
		cout << "Create index fails! For you don't create table named " << tablename << "!" << endl;
		return -1;
	}
	else if (type == 2) {
		cout << "Create index fails! For you don't have the attribute " << attrname << "!" << endl;
		return -1;
	}
	else if (type == 0) {
		cout << "The attribute is not a primarykey or unique key!" << endl;
		return -1;
	}
	Index newindex;
	newindex.name = indexname;
	newindex.tablename = tablename;
	newindex.attrname = attrname;
	newindex.root_number = -1;
	newindex.node_number = 0;
    newindex.AttrNameOffset = AttributeOff;
	if (ctable[tablenum].primary == attrname)
		newindex.isprimary = true;
	else
		newindex.isprimary = false;
	vector<Attribute>::iterator it;
	int offset = 0, length = 0;
	for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
		if ((*it).name == attrname) {
			newindex.type = (*it).type;
			offset = length;
			break;
		}
		int type = (*it).type;
		if (type == -1 || type == -2)
			length += 4;
		else
			length += type;
	}
	cindex.push_back(newindex);
	//UpdateIndex();
	cout << "Create index \"" << indexname << "\" successfully!" << endl;
	//cout<<offset<<endl;
	return offset;
}
void CatalogManager::UpdateIndex(vector<IndexChangeInfo*> ChangeIndexInfo) {
	ofstream out("Indexes.txt");
	vector<Index>::iterator it2;
	for (it2 = cindex.begin(); it2 != cindex.end(); it2++) {
		for (int i = 0; i<ChangeIndexInfo.size(); i++) {
			if ((*it2).name == ChangeIndexInfo[i]->name) {
				(*it2).root_number = ChangeIndexInfo[i]->root_number;
				(*it2).node_number = ChangeIndexInfo[i]->node_number;
				ChangeIndexInfo.erase(ChangeIndexInfo.begin() + i);
			}
		}
		out << "IndexName" << "\t" << (*it2).name << endl;
		out << "Where:\t\t" << "On Table " << (*it2).tablename << " (" << (*it2).attrname << ")" << endl;
		out << "Attribute Type:\t";
		int type = (*it2).type;
		if (type == -1)
			out << "int" << endl;
		else if (type == -2)
			out << "float" << endl;
		else if (type >= 1 && type <= 255)
			out << "char(" << type << ")" << endl;
		out << "IsPrimary: \t";
		if ((*it2).isprimary == 1)
			out << "TRUE" << endl;
		else
			out << "FALSE" << endl;
		out << "AttrNameOffset: "<<(*it2).AttrNameOffset << endl;
		out << "Root_Number: " << (*it2).root_number << endl;
		out << "Node_Number: " << (*it2).node_number << endl;
	}
	out.close();
}/*
void CatalogManager::UpdateIndex() {
	ofstream out("Indexes.txt");
	vector<Index>::iterator it2;
	for (it2 = cindex.begin(); it2 != cindex.end(); it2++) {
		for (int i = 0; i<ChangeIndexInfo.size(); i++) {
			if ((*it2).name == ChangeIndexInfo[i]->name) {
				(*it2).root_number = ChangeIndexInfo[i]->root_number;
				(*it2).node_number = ChangeIndexInfo[i]->node_number;
				ChangeIndexInfo.erase(ChangeIndexInfo.begin() + i);
			}
		}
		out << "IndexName" << "\t" << (*it2).name << endl;
		out << "Where:\t\t" << "On Table " << (*it2).tablename << " (" << (*it2).attrname << ")" << endl;
		out << "Attribute Type:\t";
		int type = (*it2).type;
		if (type == -1)
			out << "int" << endl;
		else if (type == -2)
			out << "float" << endl;
		else if (type >= 1 && type <= 255)
			out << "char(" << type << ")" << endl;
		out << "IsPrimary: \t";
		if ((*it2).isprimary == 1)
			out << "TRUE" << endl;
		else
			out << "FALSE" << endl;
		out << "Root_Number: " << (*it2).root_number << endl;
		out << "Node_Number: " << (*it2).node_number << endl;
	}
	out.close();
}*/
//判断是否成功删除索引
bool CatalogManager::DropIndex(string indexname) {
	int indexnum = IsIndexExist(indexname);
	if (indexnum >= 0) {
		if (cindex[indexnum].isprimary == true) {
			cout << "The index to be deleted is primarykey!" << endl;
			return false;
		}
		cindex.erase(cindex.begin() + indexnum);
		//UpdateIndex();
		cout << "Delete index \"" << indexname << "\" successfully!" << endl;
		return true;
	}
	cout << "Delete index \"" << indexname << "\" fails!" << endl;
	cout << "There is no such index named " << "\"" << indexname << "\"" << endl;
	return false;
}
bool CatalogManager::DropTableIndex(string tablename) {
	vector<Index>::iterator it;
	vector<int> offset;
	int count = 0;
	for (it = cindex.begin(); it != cindex.end(); it++, count++) {
		if ((*it).tablename == tablename)
			offset.push_back(count);
	}
	if (offset.size() == 0) {
		cout << "There is no index on table named " << "\"" << tablename << "\"" << endl;
		return false;
	}
	vector<int>::iterator it2;
	for (it2 = offset.begin(); it2 != offset.end(); it2++) {
		cindex.erase(cindex.begin() + (*it2));
	}
	//UpdateIndex();
	cout << "Delete index(es) on table named " << "\"" << tablename << "\" successfully!" << endl;
	return true;
}
//判断属性是否unique或是primary，1是unique,-1是primary，0则两者都不是，2是存在表但属性不存在，3是不存在表
int CatalogManager::IsAttrUnique_Primary(string tablename, string attrname) {
	int tablenum = IsTableExist(tablename);
	if (tablenum >= 0) {
		vector<Attribute>::iterator it;
		for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
			if ((*it).name != attrname)
				continue;
			return (*it).key_type;
		}
		return 2;
	}
	else return 3;
}
/*void CatalogManager::InsertValue(string tablename){
int tablenum = IsTableExist(tablename);
if(tablenum==-1){
cout<<"There is no such table named \""<<tablename<<"\"!"<<endl;
return;
}
//ctable[tablenum].recordnum+=1;
//UpdateTable();
}
void CatalogManager::DeleteValue(string tablename,int deletenum){
int tablenum = IsTableExist(tablename);
if(tablenum==-1){
cout<<"There is no such table named \""<<tablename<<"\"!"<<endl;
return;
}
//ctable[tablenum].recordnum-=deletenum;
//UpdateTable();
}
//判断该属性上是否有索引建立
bool CatalogManager::IsAttrIndex(string tablename,string attrname){
int tablenum = IsTableExist()
}*/
int CatalogManager::GetAttrNameOffset(string tablename,string attrname){
	int tablenum = IsTableExist(tablename);
	vector<Attribute>::iterator it;
	for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
		if ((*it).name == attrname)
            return (it-ctable[tablenum].attr.begin());
	}
    return -1;
}
int CatalogManager::GetAttrType(string tablename,string attrname){
	int tablenum = IsTableExist(tablename);
	vector<Attribute>::iterator it;
	for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
		if ((*it).name == attrname)
			return (*it).type;
	}
    return 0;
}
//获取索引类型，-1是int，-2是float，1~255是char
int CatalogManager::GetIndexType(string indexname) {
	int indexnum = IsIndexExist(indexname);
	return cindex[indexnum].type;
}
//string GetIndexName(string tablename,string attrname);				//获取索引名称
//获取表的记录长度
unsigned int CatalogManager::GetRecordLength(string tablename) {
	int tablenum = IsTableExist(tablename);
	return ctable[tablenum].recordlength;
}
//获取表的记录数目
/*unsigned int CatalogManager::GetRecordNum(string tablename){
int tablenum = IsTableExist(tablename);
return ctable[tablenum].recordnum;
}*/
//-1是没有找到这个属性，0-其他是属性的偏移量
int CatalogManager::GetOffset(string tablename, string attrname) {
	int tablenum = IsTableExist(tablename);
	if (tablenum == -1 || tablenum == -2)
		return -1;
	vector<Attribute>::iterator it;
	for (it = ctable[tablenum].attr.begin(); it != ctable[tablenum].attr.end(); it++) {
		if ((*it).name == attrname)
			return (*it).offset;
	}
    return 0;
}
Table CatalogManager::getTableInformation(string tablename) {
	vector<Table>::iterator it;
	int tablenum = 0;
	for (it = ctable.begin(); it != ctable.end(); it++, tablenum++) {
		if ((*it).name == tablename)
			return ctable[tablenum];
	}
    return ctable[tablenum];
}
vector<Index> CatalogManager::getTableIndex(string tablename) {
	vector<Index> in;
	for (int i = 0; i<cindex.size(); i++) {
		if (cindex[i].tablename == tablename)
			in.push_back(cindex[i]);
	}
	return in;
}
vector<int> CatalogManager::GetTableAttributeType(string tablename) {
	vector<int> type;
	int tablenum = IsTableExist(tablename);
	for (int i = 0; i<ctable[tablenum].attr.size(); i++) {
		type.push_back(ctable[tablenum].attr[i].type);
	}
	return type;
}
vector<string> CatalogManager::getAllAttrName(string tablename) {
	vector<string> n;
	int tablenum = IsTableExist(tablename);
	for (int i = 0; i<ctable[tablenum].attr.size(); i++) {
		n.push_back(ctable[tablenum].attr[i].name);
	}
	return n;
}
