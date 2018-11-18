#ifndef INTERPRETER_H
#define INTERPRETER_H

#include"Catalog.h"
#include"Macro.h"
#include<vector> 
#include<cstdlib>
using namespace std;
typedef struct condition Condition;
struct condition{
	//int attrOffset;			//�����ڱ��е�ƫ����
	std::string value;		//����ֵ interpreter
	int op;				//�����ϵ�� iterpreter
	string attrname; //������ interoreter
	int type;//���Ե�����
	int offset;//���Ե�ƫ��
};
struct inStream{											//wjs
	istream *iStream;
	struct inStream *pre;
};

	class interpret{
	public:
		int opcode;//�ú궨����ʾ�û��Ĳ���
		int primtype;
		string m_tabname;
		string m_indname;
		string m_filename;//execfile
		string m_colname;
		string value;//insert��ֵ
		vector<Attribute> col_info;//select ���,���Ӧ�ÿ��Բ���
		vector<string> attnames;//select ����Ӧ����������
		vector<Condition> condition;//��where �־�
		vector<string> ins_value;//�����ֵ
		vector<int> ins_type;//�����ʱ����Ҫ�жϸ������Ե�����
		vector<int> ins_stype;//���д����ʵ������
		vector<int> ins_keytype;
		vector<int> sel_val_off;//ѡ������У����ظ����Ե�ƫ����
		vector<int> sel_len;//ѡ������У�select��record �Ľӿ�

		int all;//select ����Ƿ�ȫ��ѡ��
		Attribute r_Attr;//��ʱ���
		Condition r_Con;
		int n;//char(n)���n
		Table getTableinfo;//�������ʱ���õ�
		Index getIndexinfo;//��������
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
			getTableinfo.primary = "";//��ʼ����table ��û������
			getTableinfo.isprimary = false;
			r_Attr.type = 0;//��������,-1��int,-2��float,1~255����char
			r_Attr.name = "";
			r_Attr.key_type = 0;//-1��primary, 1��unique��0�����߶�����
			getTableinfo.recordlength = 0;
			Top = (struct inStream*)malloc(sizeof(struct inStream));			//wjs
			Top->pre = NULL;
			Top->iStream = &cin;													//wjf
		}
		~interpret(){
			/*col_info.clear();//select ���,���Ӧ�ÿ��Բ���
			attnames.clear();//select ����Ӧ����������
			condition.clear();//��where �־�
			ins_value.clear();//�����ֵ
			ins_type.clear();//�����ʱ����Ҫ�жϸ������Ե�����
			sel_val_off.clear();//ѡ������У����ظ����Ե�ƫ����
			sel_len.clear();//ѡ������У�select��record �Ľӿ�
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
			getTableinfo.primary = "";//��ʼ����table ��û������
			getTableinfo.isprimary = false;
			r_Attr.type = 0;//��������,-1��int,-2��float,1~255����char
			r_Attr.name = "";
			r_Attr.key_type = 0;//-1��primary, 1��unique��0�����߶�����
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

