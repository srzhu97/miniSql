#include <iostream>
#include <vector>
#include <string>
#include <memory.h>
#include <cstdio> 
#include <sstream>

//#define BUFFER_OUTPUT_DEBUG

#include "bm.h"

int BufMan::FindExactBlockNum(int dst, int &position, int length)
{
    int number = -1;
    
    while (position >= 0)
    {
        number ++;
        position -= (Blocksize-8)/length;
        if (position < 0) {
            position += (Blocksize-8)/length;
            break;
        }
        dst =buffer[dst].GetNextPtr();
    }
    return number;
}


Block::Block()
{
    initial();
}

Block::Block(string title,int order, int Line_Length, int number)
{
    initial(title, Line_Length, number);
}

void Block::initial()
{
    memset(Data, 0, 4096);
    FileName.clear();
    SetIsWritten(0);
    SetIsDirty(0);
    SetIsLock(0);
    SetNum(-1);
    num = 0;
    SetIsFull();
    CleanEmptySpace();
    SetNextPtr(-1);
}

void Block::initial(string title, int Line_Length, int number)
{
    memset(Data, 0, 4096);
    memset(Data, 48, 8);
    setStringName(title);
    SetIsWritten(1);
    SetIsDirty(0);
    SetNum(number);
    SetLen(Line_Length);
    SetIsFull();
    SetIsLock(0);
    CleanEmptySpace();
    SetNextPtr(-1);
};

void Block::CleanBlock()
{
    memset(Data, 0, 4096);
    memset(Data, 48, 8);
    FileName = "";
    SetIsWritten(0);
    SetIsDirty(0);
    SetNum(0);
    SetLen(-1);
    SetIsFull();
    SetIsLock(0);
    CleanEmptySpace();
    SetNextPtr(-1);
}

void Block::CleanBlockData()
{
//    int num = GetNumber();
    memset(Data, 0, 4096);
    SetNumber(0);
    SetIsFull();
}

int Block::StoreInBlock(FILE* fp, int num)
{
    std::fseek(fp, 0, SEEK_END);
    std::size_t filesize = std::ftell(fp);
    if (filesize < Blocksize)
    {
        std::fseek(fp, 0, SEEK_SET);
        fread(Data, sizeof(char), filesize, fp);
        //fclose(fp);
        
        int count;
        if (filesize == 0) count = 0;
        else count = GetNumber();
        count = ExamEmptySpace(count);
        SetNumber(count);
        SetIsFull();
        return 0;
    }
    else
    {
        if (num == 0)
        {
            std::fseek(fp, 0, SEEK_SET);
            fread(Data, sizeof(char), 8 + (Blocksize-8)/length * length, fp);
            
            int count;
            count = (Blocksize-8) /length ;
            count = ExamEmptySpace(count);
            SetNumber(count);
            SetIsFull();
            return 0;
        }
        else {
            int offset = 8 + (Blocksize-8)/length * length;
            for (int i=1; i<num; i++)
                offset +=(Blocksize-8)/length * length;
            std::fseek(fp, offset, SEEK_SET);
            int RemainSize = filesize - offset - (Blocksize-8)/length * length;
            int count;
            if (RemainSize > 0) /////
            {
                fread(Data+8, sizeof(char), (Blocksize-8)/length * length , fp);
                count = (Blocksize-8)/ length;
            }
            else
            {
                fread(Data+8, sizeof(char), filesize - offset , fp);
                count = (filesize - offset) / length;//Need test whether it will a half record
            }
            
            SetNumber(count);
            count = ExamEmptySpace(count);
            SetNumber(count);
            SetIsFull();
            return 0;
        }
    }
    
}

int Block::ExamEmptySpace(int count)
{
    int max =count;
    int position =8, temp = count;
    for (int i=0; i<max ; i++)
    {
        for (int j=0; j<length; j++)
        {
            if (Data[position + j] != '\0') break;
            if (j == length-1)
            {
                count--; EmptySpace.push_back(i);
            }
        }
        position+=length;
    }
    EmptySpace.push_back(temp);
    EmptySpace.erase(EmptySpace.begin());
    return count;
}

int Block::StoreInFile(FILE *fp)
{
    string ObjectFile = "./DBfile/"+ FileName + ".txt";
    
    /*    if (GetIsDirty() == 0)
     {
     CleanBlock();
     return 0;
     } still remain some problem*/
    
    char tempRecord[Blocksize];
    memset(tempRecord, 0, Blocksize-8);
    int count = 1, ptr = 0;
    int position = 8, NowPlace = 0;
    int RecordLength = GetLen();
    while (count <= GetNumber())
    {
        if (EmptySpace[ptr] == count-1) {
            ptr++; count--;
        }
        for(int i=0; i<RecordLength; i++)
        {
            //           if (Data[position] == '\0') Data[position] = ' ';
            tempRecord[NowPlace++] = Data[position++];
            
        }
        
        count++;
        //更改index中位置
    }
    int mem = GetNextPtr();
    fwrite(tempRecord, sizeof(char), NowPlace, fp);
    
    CleanBlock();
    return mem;
}

string Block::ReadRecord(int position)
{
    string temp;
    int pos =8 + length * position;
    for (int i=0; i<length; i++)
        temp.append(1, Data[pos + i]);
    return temp;
}

int Block::Delete(int position)
{
    int place = 8 + (position)*length;
    for (int i=0; i<length; i++)
        *(Data+ place + i) = '\0';
    IsDirty = 1;
    for (vector<int>::iterator iter = EmptySpace.begin(); iter <EmptySpace.end(); iter++)
    {
        if (*iter > position) {
            EmptySpace.insert(iter, position);
            break;
        }
    }
    int count = GetNumber()-1;
    SetNumber(count);
    SetIsFull();
    return 0;
}

int Block::Add(char* content)
{
    int temp = GetFirstEmptySpace();
    if (EmptySpace.size() == 1)
        EmptySpace[0]++;
    else EmptySpace.erase(EmptySpace.begin());
    int position = 8 + length * temp;
    //memcpy(Data+position, content, length);
    for (int i=0; i<length; i++)
        *(Data + position + i) = content[i];
    IsDirty = 1;
    int count = GetNumber()+1;
    SetNumber(count);
    SetIsFull();
    return temp;
}

int Block::FindPosition(string record)
{
    int number = GetNumber();
    for (int i=0; i<number; i++)
    {
        if (record[0] == *(Data + 8 + i * length))
        {
            int k = 8 + i * length + 1, j;
            int m = 1;
            for (j=0; j<length; j++)
                if (Data[k++] != record[m++]) break;
            if (j==length) return i;
            
        }
    }
    return CannotFindRecord;
}

void Block::MoveRecord(int dst, int src)
{
    int length = GetLen();
    int position = 8;
    dst--; src--;
    for (int i=0; i<length; i++)
        Data[position + dst*length + i] = Data[position + dst*length + i];
}

BufMan::BufMan()
{
    for (int i=0; i<Blocknum; i++)
    {
        buffer[i].initial();
        RecentUsed.push_back(i);
    }
    
}


int BufMan::ResortBlock(string TableName,int Record_Length)
{
    int dst = FindBlock(TableName);   ///////////
    
    if (dst == CannotFindBlock)
    {
        string temp ="./DBfile/"+TableName+".txt";
        FILE* fp = fopen(temp.c_str(), "rb");
        if (fp == NULL) return CannotFindTable;
        std::fseek(fp, 0, SEEK_END);
        int filesize = (int)(ftell(fp));
        filesize -= 8 + (Blocksize-8) / Record_Length * Record_Length;
        int NeedBlock = 0;
        while (filesize>0)
        {
            NeedBlock++;
            filesize-= (Blocksize-8) / Record_Length * Record_Length;
        }
        int former = -1;
        for (int i=0; i<=NeedBlock; i++)
        {
            int dstt = IsEmptyBlock();
            if (dstt == -1)
            {
                dstt = ArrangeEmptyBlock();
            }
            buffer[dstt].initial(TableName, Record_Length, i);
            buffer[dstt].StoreInBlock(fp, i);
            if (former != -1) buffer[former].SetNextPtr(dstt);
            else dst = dstt;
            former = dstt;
        }
        fclose(fp);
    }
    return dst;
}

int BufMan::AddRecord(string TableName, char* content, int Record_Length)
{
    int dst = ResortBlock(TableName, Record_Length);
    int offset = 0;
    
    while (buffer[dst].GetIsFull())
    {
        if (buffer[dst].GetNextPtr() == -1)
        {
            int NewBlock = CreateTable(TableName, Record_Length, buffer[dst].GetNum()+1);
            buffer[dst].SetNextPtr(NewBlock);
            offset += buffer[NewBlock].Add(content);
            return offset;
        }
        else
        {
            offset += buffer[dst].GetNumber();
            dst = buffer[dst].GetNextPtr();
        }
        
    }
    offset =  buffer[dst].Add(content);
    buffer[dst].SetIsFull();
    return offset;
}

int BufMan::DeleteRecord(string TableName, int position, int Record_Length)
{
    int pos = position;
    int dst = ResortBlock(TableName, Record_Length);
    int fileblocknum = FindExactBlockNum(dst, pos,Record_Length);
    while (dst != -1)
    {
        if (buffer[dst].GetNum() == fileblocknum) break;
        dst = buffer[dst].GetNextPtr();
    }
    if (dst!=-1) buffer[dst].Delete(position);
    else return CannotFindBlock;
    return 0;
}

int BufMan::ArrangeEmptyBlock()
{
    int ptr = -1;
    ptr = *RecentUsed.begin();
    RecentUsed.pop_front();
    RecentUsed.push_back(ptr);
    if (ptr == -1 )
    {
        cout << "error";return 0;
    }
    PutInFile(buffer[ptr].GetStringName(),ptr);
    
    return ptr;
}

int BufMan::PutInFile(string filename, int firstnum)
{
    int nextptr = firstnum;
    string tempName= "./DBfile/"+filename + ".txt";
    FILE *fp;
    if ((fp = fopen(tempName.c_str(),"wb"))==NULL	){
        return CannotFindFile;
    }
    int count = 0;
    for (int i=0; i<200; i++)
        if (buffer[i].GetStringName() == filename) count += buffer[i].GetNumber();  ///////change
    fseek(fp, 0, SEEK_SET);
    char recordnum[8];
    int comp = 10000000;
    for (int i=0; i<8; i++)
    {
        recordnum[i] = count / comp + '0';
        count = count % comp;
        comp = comp / 10;
    }
    fwrite(recordnum, sizeof(char), 8, fp);
    
    while (nextptr != -1)
        nextptr = buffer[nextptr].StoreInFile(fp);
    fclose(fp);
    
    return 0;
}

int BufMan::CreateTable(string TableName, int Record_Length, int number)
{
    int NewBlock = IsEmptyBlock();
    if (NewBlock == -1) NewBlock = ArrangeEmptyBlock();
    buffer[NewBlock].initial(TableName, Record_Length, number);
    buffer[NewBlock].SetIsDirty(1);
    return NewBlock;
}

int BufMan::DropTable(string TableName)
{
    int dst =FindBlock(TableName);
    
    if (dst != CannotFindBlock) {
        while (dst != -1)
        {
            int nextptr = buffer[dst].GetNextPtr();
            buffer[dst].CleanBlock();
            RecentUsed.pop_back();
            RecentUsed.push_front(dst);
            dst = nextptr;
        }
    }
    else {
        FILE* fp = fopen(("./DBfile/"+TableName+".txt").c_str(), "rb");
        if (fp == NULL) return CannotFindTable;
        fclose(fp);
    };
    remove(("./DBfile/"+TableName+".txt").c_str());
    return 0;
}

int BufMan::CleanBuffer(string TableName)
{
    int dst =FindBlock(TableName);
    if (dst != CannotFindBlock)
        buffer[dst].CleanBlockData();
    buffer[dst].CleanEmptySpace();
    return 0;
}

int BufMan::TableRecordNumber(string TableName, int length)
{
    int dst = ResortBlock(TableName, length);
    int RecordCount = 0;
    while (dst!=-1)
    {
        RecordCount +=buffer[dst].GetNumber();
        dst = buffer[dst].GetNextPtr();
    }
    return RecordCount;
}

vector<string> BufMan::GetTableRecord(string TableName, int Record_Length) ///
{
    vector<string> ResultRecord;
    
    int dst = ResortBlock(TableName, Record_Length);
    
    int count= buffer[dst].GetNumber();
    int nextdst = buffer[dst].GetNextPtr();
    while (nextdst!=-1)
    {
        count += buffer[nextdst].GetNumber();
        nextdst = buffer[nextdst].GetNextPtr();
    }
    int BlockOrder = 1; int ptr = 0;int head=0; int cal = 0;
    for (int i=0; i<count ; i++)
    {
        if (ptr == buffer[dst].GetEmptyList(head))
        {
            head++; i--;
        }
        string temp = buffer[dst].ReadRecord(ptr);
        ResultRecord.push_back(temp);
        if(BlockOrder * (Blocksize - 8) >= Record_Length * (cal+BlockOrder) && BlockOrder * (Blocksize - 8) < Record_Length * (cal+BlockOrder+1))
        {
            BlockOrder++; dst = buffer[dst].GetNextPtr(); ptr =-1;head = 0;
        }
        ptr++; cal++;
        
        //if (buffer[dst].)
    }
    return ResultRecord;
}


int BufMan::FindBlock(string TableName)
{
    for (int i=0; i<Blocknum; i++)
    {
        if (buffer[i].GetStringName() == TableName && buffer[i].GetNum() == 0)
        {
            list<int>::iterator iter = RecentUsed.end();
            int iterr = 200;
            int ptr = i;
            while (ptr!=-1)
            {
                do {
                    iterr--; iter--;
                    if (*iter == ptr) {
                        RecentUsed.erase(iter);
                        RecentUsed.push_back(ptr);
                        break;
                    }
                }while (iter!=RecentUsed.begin());
                ptr = buffer[ptr].GetNextPtr();
            }
            return i;
        }
        
    }
    return CannotFindBlock;
}

int BufMan::IsEmptyBlock()
{
    int pos =*RecentUsed.begin();
    RecentUsed.pop_front();
    RecentUsed.push_back(pos);
    if (buffer[pos].GetIsWritten() == 0)
        return pos;
    return -1;
}

int BufMan::StoreBuffer()
{
    for (int i=0; i<Blocknum; i++)
    {
        if (buffer[i].GetIsWritten() == 1 && buffer[i].GetNum() == 0)
            PutInFile(buffer[i].GetStringName(), i);
    }
    return 0;
}

void Block::CleanEmptySpace()
{
    EmptySpace.clear();
    EmptySpace.push_back(0);
}

int Block::GetFirstEmptySpace()
{
    return EmptySpace[0];
}

void Block::SetIsFull()
{
    int x =GetNumber(), y = GetLen();
    if (x * y <= Blocksize - 8 && (x+1) * y > Blocksize - 8)
        IsFull = 1;
    else IsFull = 0;
}


int Block::GetIsFull()
{
    return IsFull;
}


void Block::setStringName(string title)
{
    FileName = title;
}

string Block::GetStringName()
{
    return FileName;
}

void Block::SetNumber(int number)
{
    int comp = 10000000;
    for (int i=0; i<8; i++)
    {
        Data[i] = number / comp + '0';
        number = number % comp;
        comp = comp / 10;
    }
}

int Block::GetNumber()
{
    int comp = 0;
    for (int i=0; i<8; i++)
    {
        comp *= 10;
        comp += Data[i] - '0';
    }
    return comp;
}

