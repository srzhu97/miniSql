 
#ifndef bm_h
#define bm_h
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#define Blocksize 4096
#define Blocknum  200

#define CannotFindFile 1000
#define CannotFindRecord 1001
#define CannotFindBlock 1002
#define TableHasExist 1003
#define CannotFindTable 1004

using namespace std;

class Block{
private:
    string FileName;
    int IsWrite;
    int IsDirty;
    int IsFull;
    int IsLock;
    int num;
    int NextPtr;
    int length;
    vector<int> EmptySpace;
    
public:
    char Data[Blocksize];
    Block();
    ~Block(){};
    Block(string title, int order, int Line_Length, int number);
    void initial(string title, int Line_Length,int number);
    void initial();
    void CleanBlock();
    void CleanBlockData();
    
    void SetNum(int count) { num = count; }
    int GetNum() { return num; }
    void SetLen(int Line_Length) { length = Line_Length; }
    int  GetLen() { return length; }
    void SetIsWritten(int status) { IsWrite = status; }
    int GetIsWritten() { return IsWrite; }
    void SetIsDirty(int status) { IsDirty = status; }
    int GetIsDirty() { return IsDirty; }
    void SetIsLock(int status) { IsLock = status; }  // 0 is unlock
    int GetIsLock() {return IsLock; }
    void SetIsFull();//
    int GetIsFull();//
    void SetNextPtr(int pointer) { NextPtr = pointer;  }
    int GetNextPtr() { return NextPtr; }
    void setStringName(string title);
    string GetStringName();
    int GetEmptyList(int head) {return EmptySpace[head]; }
    
    void CleanEmptySpace();
    int GetFirstEmptySpace();
    
    string ReadRecord(int position);
    int Add(char* content);
    int Delete(int position);
    int FindPosition(string record);
    int FindEmptyPosition();
    void SetNumber(int number);
    int GetNumber();
    void MoveRecord(int dst, int src);
    int ExamEmptySpace(int count);
    
    int StoreInBlock(FILE* fp, int num);
    int StoreInFile(FILE* fp);
    
};


class BufMan{
private:
    list<int> RecentUsed;
    
public:
    BufMan();
    ~BufMan(){};
    Block buffer[Blocknum];
    
    int AddRecord(string TableName, char* content, int Record_Length);
    int DeleteRecord(string TableName, int position, int Record_Length);
    
    int CreateTable(string TableName, int Record_Length, int number);
    int DropTable(string TableName);
    vector<string> GetTableRecord(string TableName, int Record_Length);
    int PutInFile(string filename, int firstnum);
    int ResortBlock(string TableName,int Record_Length);
    int TableRecordNumber(string TableName, int length);

    
    int FindExactBlockNum(int dst, int &position, int length);
    int FindBlock(string TableName);
    int ArrangeEmptyBlock();
    int IsEmptyBlock();
    int StoreBuffer();
    int CleanBuffer(string TableName);
};

#endif /* bm_hpp */
