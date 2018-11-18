
#include "interpreter.h"
#include<iostream>
#include "Macro.h"
#include<fstream>
#include <string>
using namespace std;

void interpret::Parse(){
	//CatalogManager catalog;
	vector<string> des;
	string temp;
	string temp2;
	int end = 1;
	if (this->Top->pre == NULL)	std::cout << "mysql>";
	*(this->Top->iStream) >> temp;
	ins_stype.clear();
	ins_keytype.clear();
	col_info.clear();//select 语句,这个应该可以不用
	attnames.clear();//select 语句对应的属性名字
	condition.clear();//存where 字句
	ins_value.clear();//插入的值
	ins_type.clear();//插入的时候需要判断各个属性的类型
	sel_val_off.clear();//选择语句中，返回给属性的偏移量
	sel_len.clear();//选择语句中，select给record 的接口
	getTableinfo.isprimary = false;
	getTableinfo.attr.clear();
	totaloff = 0;
	all = 0;
    findPrimary=0;
	int a;
	////getTableinfo.name.clear();
	//getTableinfo.primary.clear();
while (temp != ";")
	{
		char c;
		char c2;
		int i = 0;
		while (temp.size() != 0 && i < temp.size()){
			while ((temp[i] != '(') && (temp[i] != ',') && (temp[i] != ')') && (i < temp.size()) && (temp[i] != ';') && (temp[i] != '<') && (temp[i] != '=') && (temp[i] != '>'))
				i++;
			if (i == temp.size()){
				if ((c = temp.find(";")) != temp.npos)
					end = 0;
				if (((c = temp.find("'")) != temp.npos) || ((c = temp.find("\"")) != temp.npos)){
					temp.erase(c, 1);

					while (((c = temp.find("'")) == temp.npos) && ((c = temp.find("\"")) == temp.npos)){
						a = temp.find('\"');
						c2 = cin.get();
						temp += c2;
					}
					temp.erase(c, 1);

				}
				des.push_back(temp);
				break;
			}
			if (temp[i] == '('){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
					stack[s_p++] = '(';
				}
				temp.erase(0, i + 1);
			}
			else if (temp[i] == ','){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
				}
				temp.erase(0, i + 1);
			}
			else if (temp[i] == ')'){
				if ((c = temp.find(";")) != temp.npos){
					end = 0;
					temp.erase(c, 1);
				}
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
				}
				temp.erase(0, i + 1);
				stack[s_p++] = ')';
			}
			else if (temp[i] == '='){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
				}
				des.push_back("=");
				temp.erase(0, i + 1);
			}
			else if (temp[i] == '<'){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
				}
				if ((i + 1) < temp.size() && temp[i + 1] == '='){
					des.push_back("<=");
					temp.erase(0, i + 2);
				}
				else if ((i + 1) < temp.size() && temp[i + 1] == '>'){
					des.push_back("<>");
					temp.erase(0, i + 2);
				}
				else{
					des.push_back("<");
					temp.erase(0, i + 1);
				}
			}
			else if (temp[i] == '>'){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
				}
				if ((i + 1) < temp.size() && temp[i + 1] == '='){
					des.push_back(">=");
					temp.erase(0, i + 2);
				}
				else{
					des.push_back(">");
					temp.erase(0, i + 1);
				}
			}
			else if (i<temp.size() && temp[i] == ';'){
				temp2 = temp.substr(0, i);
				if (temp2.size() != 0){
					if (((c = temp2.find("'")) != temp2.npos) || ((c = temp2.find("\"")) != temp2.npos)){
						temp2.erase(c, 1);

						while (((c = temp2.find("'")) == temp2.npos) && ((c = temp2.find("\"")) == temp2.npos)){
							c2 = cin.get();
							temp += c2;
						}
						temp2.erase(c, 1);

					}
					des.push_back(temp2);
					temp.erase(0, i + 1);
				}
				else{
					temp.erase(0, i + 1);
				}
				end = 0;
			}
			i = 0;

		}
		if (end) {
			//cout << "mysql>";
			*(this->Top->iStream) >> temp;									//wj
		}
		else break;
	}
	if (des.size() == 0) {
		opcode = Empty;
		return;
	}
	ofstream out("input.txt", ios::app);
	for (int m = 0; m < des.size(); m++)
		out << des[m] << endl;
	out.close();
	int size = des.size();
	int st = 0;
	vector<Attribute> ::iterator iter_a;
	int it;
	if (st < size&&des[st++] == "create"){
		if (st < size&&des[st++] == "table"){
			opcode = CreateTabErr;
			if (st >= size) return;
			m_tabname = des[st++];
			getTableinfo.name = m_tabname;
			opcode = CreateTabErr;
			while (st<size){
				if (des[st] == "primary"){
					st++;
					if (st >= des.size()) return;
					if (des[st++] != "key")
						return;
					if (st >= size)
						return;
					if (st >= des.size()) return;
					temp = des[st++];
					if (!getTableinfo.isprimary)
						getTableinfo.isprimary = true;
					else{
						opcode = InvalidPrimary;
						return;
					}

					//查看是否有这个属性
					it = 0, iter_a = getTableinfo.attr.begin();
					//cout << getTableinfo.attr.size() << endl;
					//cout << getTableinfo.attr.begin()->name << endl;
					//cout << iter_a->name << endl;
					for (; it<getTableinfo.attr.size(); it++, iter_a++){
						if (iter_a->name == temp){
							findPrimary = 1;
							getTableinfo.primary = temp;
							iter_a->key_type = -1;
							primtype = iter_a->type;
							getTableinfo.isprimary = true;
							break;
						}
					}
					/*if (!findPrimary)
					{
						opcode = InvalidPrimary;
						return;
					}*/
					//findPrimary = 0;
					break;
				}
				if (st >= des.size()) return;
				r_Attr.name = des[st++];
				r_Attr.key_type = 0;
				if (st >= des.size()) return;
				temp = des[st++];
				if (temp == "char") {
					if (st >= des.size()) return;
					int n = atoi((char *)des[st++].c_str());
					if ((n<0) || (n>255)) return;
					r_Attr.type = n;
					r_Attr.offset = totaloff;
					totaloff += n;
				}
				else if (temp == "int"){
					r_Attr.type = -1;
					r_Attr.offset = totaloff;
					totaloff += 4;
				}

				else{
					r_Attr.type = -2;
					r_Attr.offset = totaloff;
					totaloff += 4;
				}
				if (st >= des.size()) return;
				if (des[st] == "unique"){
					st++;
					r_Attr.key_type = 1;
				}
				getTableinfo.attr.push_back(r_Attr);
				r_Attr.key_type = 0;
			}
			opcode = CreateTab;
			return;
		}
		else if (--st < size&&des[st++] == "index"){
			opcode = CreateIndErr;
			if (st >= des.size()) return;
			getIndexinfo.name = des[st++];
			if (st >= des.size()) return;
			if (des[st++] != "on") return;
			if (st >= des.size()) return;
			m_tabname = des[st++];
			getIndexinfo.tablename = m_tabname;
			if (st >= des.size()) return;
			getIndexinfo.attrname = des[st++];
			opcode = CreateInd;


			return;
		}

	}
	else if (--st < size&&des[st++] == "delete")
	{
		temp = des[st++];
		if (temp != "from"){
			opcode = DeleErr;
			return;
		}
		m_tabname = des[st++];
		if (st == size){
			
			opcode=Del_Nowhere;
			return;
		}
		else{
			r_Attr.offset = 1;
			st++;    
			//cout<<"st size"<<st<<endl;
			m_colname = des[st++];
			//<<"col_name "<<m_colname<<endl;
			temp = des[st++];
			
			if (temp == "<")
				r_Con.op = 0;
			else if (temp == "<=")
				r_Con.op = 1;
			else if (temp == ">")
				r_Con.op = 2;
			else if (temp == ">=")
				r_Con.op = 3;
			else if (temp == "=")
				r_Con.op = 4;
			else if (temp == "<>")
				r_Con.op = 5;
			else return;
			r_Con.attrname = m_colname;
			//cout<<des[st]<<endl;
			r_Con.value = des[st++];
			//cout<<"delete value: "<<r_Con.attrname<<" "<<r_Con.op<<" "<<r_Con.value<<endl;
			condition.push_back(r_Con);//插入条件
			while (st<des.size() && (des[st++] == "and"))
			{  
				//r_Attr.offset = 1;
				m_colname = des[st++];
				temp = des[st++];
				if (temp == "<")
					r_Con.op = 0;
				else if (temp == "<=")
					r_Con.op = 1;
				else if (temp == ">")
					r_Con.op = 2;
				else if (temp == ">=")
					r_Con.op = 3;
				else if (temp == "=")
					r_Con.op = 4;
				else if (temp == "<>")
					r_Con.op = 5;
				else return;
				r_Con.attrname = m_colname;
				r_Con.value = des[st++];
				//cout<<"insert value: "<<r_Con.attrname<<" "<<r_Con.op<<" "<<r_Con.value<<endl;
				condition.push_back(r_Con);//插入条件
			}
			opcode = Del;
			return;
		}

	}

	else if (--st < size&&des[st++] == "select")
	{
		temp = des[st++];
		if (temp == "*") all = 1;
		else {
			r_Attr.name = temp;
			attnames.push_back(temp);
			col_info.push_back(r_Attr);//循环push要返回的列
			while (des[st] != "from"){
				temp = des[st++];
				r_Attr.name = temp;
				r_Con.attrname = temp;
			
				attnames.push_back(temp);
				col_info.push_back(r_Attr);//循环push要返回的列
			}
		}
		st++;
		m_tabname = des[st++];
		//cout<<"tablename "<<m_tabname<<endl;
		if (st >= size){
			opcode = SELECT_NOWHERE_CAULSE;
			return;
		}
		else{
			r_Attr.offset = 1;
			st++;    
			//cout<<"st size"<<st<<endl;
			m_colname = des[st++];
			//<<"col_name "<<m_colname<<endl;
			temp = des[st++];
			
			if (temp == "<")
				r_Con.op = 0;
			else if (temp == "<=")
				r_Con.op = 1;
			else if (temp == ">")
				r_Con.op = 2;
			else if (temp == ">=")
				r_Con.op = 3;
			else if (temp == "=")
				r_Con.op = 4;
			else if (temp == "<>")
				r_Con.op = 5;
			else return;
			r_Con.attrname = m_colname;
			//cout<<des[st]<<endl;
			r_Con.value = des[st++];
			//cout<<"insert value: "<<r_Con.attrname<<" "<<r_Con.op<<" "<<r_Con.value<<endl;
			condition.push_back(r_Con);//插入条件
			while (st<des.size() && (des[st++] == "and"))
			{  
				//r_Attr.offset = 1;
				m_colname = des[st++];
				temp = des[st++];
				if (temp == "<")
					r_Con.op = 0;
				else if (temp == "<=")
					r_Con.op = 1;
				else if (temp == ">")
					r_Con.op = 2;
				else if (temp == ">=")
					r_Con.op = 3;
				else if (temp == "=")
					r_Con.op = 4;
				else if (temp == "<>")
					r_Con.op = 5;
				else return;
				r_Con.attrname = m_colname;
				r_Con.value = des[st++];
				//cout<<"insert value: "<<r_Con.attrname<<" "<<r_Con.op<<" "<<r_Con.value<<endl;
				condition.push_back(r_Con);//插入条件
			}
			opcode = SELECT_WHERE_CAULSE;
			return;
		}

	}
	else if (--st < size&&des[st++] == "insert")
	{
		temp = des[st++];
		if (temp != "into"){
			opcode = InstErr;
			return;
		}
		m_tabname = des[st++];
		temp = des[st++];
		if (temp != "values")
			return;

		temp = des[st++];
		ins_value.push_back(temp);
		while (st<des.size())
		{
			temp = des[st++];
			ins_value.push_back(temp);
		}
		opcode = Ins;
		return;
	}
	else if (--st < size&&des[st++] == "drop")
	{
		opcode = DropTabErr;
		if (st >= size) return;
		temp = des[st++];
		if (temp == "table")
		{
			m_tabname = des[st++];
			opcode = DropTab;
			return;
		}
		else if (temp == "index")
		{
			opcode = DropIndErr;
			if (st >= size) return;
			m_indname = des[st++];
			opcode = DrpInd;
			return;
		}
	}
	else if (--st < size&&des[st++] == "quit")
	{
		opcode = Quit;

		return;
	}
	else if (--st < size&&des[st++] == "execfile")		//wjs
	{
		opcode = ExecFileErr;
		if (st >= size) return;
		m_filename = des[st++];
		opcode = ExecFile;
		return;
	}													//wjf
}
//This function is used to check is the string 'input' represented an int variable
//return 1 if is, 0 otherwise
short int interpret::IsInt(const char *input)//支持-号
{
	int i;
	int length = strlen(input);
	for (i = 0; i < length; i++){
		if (!isdigit(input[i])) return 0;
	}
	return 1;
}

//This function is used to check is the string 'input' represented an float variable
//return 1 if is, 0 otherwise
short int interpret::IsFloat(char *input)
{
	int i;
	int length = strlen(input);
	for (i = 0; i < length; i++){
		bool a = !isdigit(input[i]);
		bool b = input[i] != '.';
		bool c = a&&b;
		if (!isdigit(input[i]) && input[i] != '.')
			return 0;
	}
	return 1;
}
short int interpret::judtype(char *input){
	int i = 0;
	if (IsInt(input)) return -1;
	else if (IsFloat(input)) return -2;
	else{
		return strlen(input);
	}
}

bool interpret::NewFile()							//wjs
{
	struct inStream *temp;
	ifstream *tempF;

	tempF = new ifstream(m_filename.c_str());
	if (!tempF) return false;

	temp = (struct inStream*)malloc(sizeof(struct inStream));
	temp->pre = this->Top;
	temp->iStream = tempF;
	this->Top = temp;
	return true;
}
bool interpret::QuitFile()
{
	struct inStream *temp;
	temp = this->Top->pre;
	if (temp == NULL) return false;

	delete this->Top->iStream;
	free(this->Top);
	this->Top = temp;
	return true;
}													//wjf

