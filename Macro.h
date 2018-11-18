#pragma once
#include <string>
#include<string.h>
#include<vector>
#ifndef MACRO_H
#define MACRO_H
#define Unknown 11
#define Empty 10
#define Quit 12
#define CreateTab 13
#define CreateTabErr 39
#define CreateInd 14

#define CreateIndErr 1//建立重复的index
#define TableExisted 15
#define InvalidPrimary 16//输入的主键名不存在
#define VoidUni 17//键入的unique不存在
#define TableExistedErr 18 //插入index的时候，这个表不存在
#define AttrExistErr 19//创建索引的时候，属性名不存在
#define SELERR 20//选择语句
#define SELECT_NOWHERE_CAULSE 21//选择语句没有where
#define COLUMNERROR 22//选择语句的时候没有这个列
#define SELECT_WHERE_CAULSE 23
#define DeleErr 24//删除语句
#define Del 25//删除操作
#define Del_Nowhere 27
#define ColErr 26
#define DropTabErr 28
#define DropTab 35
#define DropIndErr 34
#define DrpInd 29
#define InstErr 30
#define InsNumErr 31//插入的值不符合表的要求
#define InserDuplicateValue 32
#define Ins 33
#define Sel_No_Attr 40
#define LackBrack 41
#define ExecFile 42
#define ExecFileErr 43
#define Blocksize 4096
#define Blocknum  200

typedef struct indexinfo IndexChangeInfo;
struct indexinfo {
    std::string name;
    int root_number;
    int node_number;
};
typedef struct index Index;
struct index {
    std::string name;				//索引名字
    std::string tablename;			//索引对应的表名
    std::string attrname;			//索引对应的关键属性名
    int type;					//对应属性的类型,属性类型,-1是int,-2是float,1~255就是char
    bool isprimary;				//是不是primary
    int root_number;			//索引的根节点信息
    int node_number;			//索引的节点数量
    int AttrNameOffset;
};
typedef struct attribute Attribute;
struct attribute {
    std::string name;			//属性名
    //unsigned int length;	//属性长度
    unsigned int offset;	//属性在表中的偏移量
    int key_type;			//-1是primary, 1是unique，0是两者都不是
    int type;			//属性类型,-1是int,-2是float,1~255就是char
};
typedef struct table Table;
struct table {
    std::string name;						//表名
    std::vector<Attribute> attr;				//属性列表
    std::string primary;						//主键名
    bool isprimary;						//用来判断这张表是否存在主键
    //unsigned int recordnum;				//记录的数量
    unsigned int recordlength;			//记录的长度
};


#endif

