#ifndef INTERPRETER_H
#define INTERPRETER_H

#include"Catalog.h"
#include"Macro.h"
#include<vector> 
#include<cstdlib>
using namespace std;
typedef struct condition Condition;
struct condition{
	//int attrOffset;			//属性在表中的偏移量
	std::string value;		//常数值 interpreter
	int op;				//代表关系符 iterpreter
	string attrname; //属性名 interoreter
	int type;//属性的类型
	int offset;//属性的偏移
};
struct inStream{											//wjs
	istream *iStream;
	struct inStream *pre;
};

	class interpret{
	public:
		int opcode;//用宏定义显示用户的操作
		int primtype;
		string m_tabname;
		string m_indname;
		string m_filename;//execfile
		string m_colname;
		string value;//insert的值
		vector<Attribute> col_info;//select 语句,这个应该可以不用
		vector<string> attnames;//select 语句对应的属性名字
		vector<Condition> condition;//存where 字句
		vector<string> ins_value;//插入的值
		vector<int> ins_type;//插入的时候需要判断各个属性的类型
		vector<int> ins_stype;//表中存的真实的类型
		vector<int> ins_keytype;
		vector<int> sel_val_off;//选择语句中，返回给属性的偏移量
		vector<int> sel_len;//选择语句中，select给record 的接口

		int all;//select 语句是否全部选择
		Attribute r_Attr;//临时存放
		Condition r_Con;
		int n;//char(n)里的n
		Table getTableinfo;//创建表的时候用到
		Index getIndexinfo;//创建索引
		char stack[100];
		struct inStream *Top;													//wj
		int s_p;
		int totaloff;
		interpret(){
			all = 0;
			s_p = 0;
			opcode = Unknown;
			m_tabname = "";
			m_indname = "";
			m_colname = "";
			findPrimary = 0;
			findUnique = 0;
			getTableinfo.name = "";
			getTableinfo.primary = "";//初始化，table 里没有主键
			getTableinfo.isprimary = false;
			r_Attr.type = 0;//属性类型,-1是int,-2是float,1~255就是char
			r_Attr.name = "";
			r_Attr.key_type = 0;//-1是primary, 1是unique，0是两者都不是
			getTableinfo.recordlength = 0;
			Top = (struct inStream*)malloc(sizeof(struct inStream));			//wjs
			Top->pre = NULL;
			Top->iStream = &cin;													//wjf
		}
		~interpret(){
			/*col_info.clear();//select 语句,这个应该可以不用
			attnames.clear();//select 语句对应的属性名字
			condition.clear();//存where 字句
			ins_value.clear();//插入的值
			ins_type.clear();//插入的时候需要判断各个属性的类型
			sel_val_off.clear();//选择语句中，返回给属性的偏移量
			sel_len.clear();//选择语句中，select给record 的接口
			getTableinfo.attr.clear();
			all = 0;
			s_p = 0;
			opcode = Unknown;
			m_tabname = "";
			m_indname = "";
			m_colname = "";
			findPrimary = 0;
			findUnique = 0;
			getTableinfo.name = "";
			getTableinfo.primary = "";//初始化，table 里没有主键
			getTableinfo.isprimary = false;
			r_Attr.type = 0;//属性类型,-1是int,-2是float,1~255就是char
			r_Attr.name = "";
			r_Attr.key_type = 0;//-1是primary, 1是unique，0是两者都不是
			getTableinfo.recordlength = 0;*/
		}
		void Parse();
		bool NewFile();									//wj
		bool QuitFile();
		short int IsInt(const char *);
		short int IsFloat(char * input);
		short int findPrimary;
		short int findUnique;
		short int judtype(char *input);
        
		
	};

#endif

