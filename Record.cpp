

#include "Record.h"
#include <string>
#include <cstdio>
#include <cstdlib>
int RecordManager::Insert(string tablename, vector<string> value, vector<int> type, vector<int>key_type, int record_length) {
    char *p = (char*)malloc(record_length * sizeof(char));
    int a;
    float b;
    int off = 0;
    vector<string>AllRecord = bm.GetTableRecord(tablename, record_length);
    for (int i = 0; i<value.size(); i++) {
        switch (type[i]) {
            case -1:
                sscanf(value[i].c_str(), "%d", &a);
                memcpy(p + off, &a, sizeof(int));
                if (key_type[i] == -1 || key_type[i] == 1) {
                    for (int j = 0; j<AllRecord.size(); j++) {
                        int cmp = memcmp(AllRecord[j].c_str() + off, p + off, sizeof(int));
                        if (!cmp) {
                            cout << "违反unique或primarykey！" << endl;
                            return -1;
                        }
                    }
                }
                off += 4;
                break;
            case -2:
                sscanf(value[i].c_str(), "%f", &b);
                memcpy(p + off, &b, sizeof(float));
                if (key_type[i] == -1 || key_type[i] == 1) {
                    for (int j = 0; j<AllRecord.size(); j++) {
                        int cmp = memcmp(AllRecord[j].c_str() + off, p + off, sizeof(float));
                        if (!cmp) {
                            cout << "违反unique或primarykey！" << endl;
                            return -1;
                        }
                    }
                }
                off += 4;
                break;
            default:
                int len = value[i].length();
                while (len<type[i]) {
                    value[i] += " ";
                    len++;
                }
                memcpy(p + off, value[i].c_str(), len);
                if (key_type[i] == -1 || key_type[i] == 1) {
                    for (int j = 0; j<AllRecord.size(); j++) {
                        int cmp = memcmp(AllRecord[j].c_str() + off, p + off, len);
                        if (!cmp) {
                            cout << "违反unique或primarykey！" << endl;
                            return -1;
                        }
                    }
                }
                off += len;
        }
    }
    int m = bm.AddRecord(tablename, p, record_length);
    if (m<0) {
        cout << "插入失败！" << endl;
        return -1;
    }
    cout << "成功插入一条记录！" << endl;
    return m;
}

void RecordManager::DeleteNoCondition(string tablename, int record_length) {
    int i;
    int RecordNum=bm.TableRecordNumber(tablename,record_length);
    for (i = 0; i<RecordNum; i++) {
        //cout << i<<endl;
        int m = bm.DeleteRecord(tablename, i, record_length);
        //cout<<m<<endl;
        if (m==CannotFindTable)
            break;
    }
   if (i<RecordNum) {
        cout << "删除记录失败" << endl;
        return;
    }
    cout << "成功删除 " << i << " 条记录！" << endl;
}

vector<string> RecordManager::DeleteWithConditionNoIndex(string tablename, int record_length, vector<Condition> condition,vector<int> type){
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int RecordNum = bm.TableRecordNumber(tablename,record_length);
    vector<int> result;
    for(int i=0;i<RecordNum;i++){
        result.push_back(i);
    }
    for(int i=0;i<condition.size();i++){
        vector<int> newr;
        switch(condition[i].type){
            case -1://int
                for(int j=0;j<result.size();j++){
                    int a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    int b = atoi(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            case -2://float
                for(int j=0;j<result.size();j++){
                    float a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    float b = atof(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            default://char
                for(int j=0;j<result.size();j++){
                    char *a=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(a, p+condition[i].offset,condition[i].type);
                    *(a+condition[i].type)='\0';
                    char *b=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *q=condition[i].value.c_str();
                    memcpy(b,q,condition[i].type);
                    *(b+condition[i].type)='\0';
                    switch(condition[i].op){
                        case 0://<
                            if(strcmp(a,b)<0) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(strcmp(a,b)<=0) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(strcmp(a,b)>0) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(strcmp(a,b)>=0) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(strcmp(a,b)==0) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(strcmp(a,b)!=0) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
        }
    }
    int i;
    vector<string> DeleteRecord;
    for(i=0;i<result.size();i++){
        string str;
        const char*p=AllRecord[result[i]].c_str();
        int len = 0;
        stringstream ss;
        for (int j = 0; j<type.size(); j++) {
            if(type[j]==-1) {
                string s1;
                int a;
                memcpy(&a, p+len,4);
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else if (type[j]==-2){
                string s1;
                float a;
                memcpy(&a, p+len,4);
                stringstream ss;
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else{
                char *a=(char*)malloc(sizeof(char)*(type[j]+1));
                memcpy(a,p+len,type[j]);
                *(a+type[j])='\0';
                DeleteRecord.push_back(string(a));
                len+=type[j];
            }
        }
        int m = bm.DeleteRecord(tablename, result[i], record_length);
        if (m==CannotFindTable)
            break;
    }
    if (i<result.size()) {
        cout << "删除失败" << endl;
        return DeleteRecord;
    }
    cout << "成功删除 " << i << " 条记录!" << endl;
    return DeleteRecord;
}

vector<string> RecordManager::DeleteOnlyWithIndex(string tablename, vector<int>RecordOrder, int record_length,vector<int> type){
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int i;
    vector<string> DeleteRecord;
    for(i=0;i<RecordOrder.size();i++){
        string str;
        const char*p=AllRecord[RecordOrder[i]].c_str();
        int len = 0;
        stringstream ss;
        for (int j = 0; j<type.size(); j++) {
            if(type[j]==-1) {
                string s1;
                int a;
                memcpy(&a, p+len,4);
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else if (type[j]==-2){
                string s1;
                float a;
                memcpy(&a, p+len,4);
                stringstream ss;
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else{
                char *a=(char*)malloc(sizeof(char)*(type[j]+1));
                memcpy(a,p+len,type[j]);
                *(a+type[j])='\0';
                DeleteRecord.push_back(string(a));
                len+=type[j];
            }
        }
        cout<<RecordOrder[i]<<endl;
        int m = bm.DeleteRecord(tablename, RecordOrder[i], record_length);
        if (m==CannotFindTable)
            break;
    }
    if (i<RecordOrder.size()) {
        cout << "删除失败" << endl;
        return DeleteRecord;
    }
    cout << "成功删除 " << i << " 条记录!" << endl;
    return DeleteRecord;

}

vector<string> RecordManager::DeleteWithCondition(string tablename, vector<int>RecordOrder, int record_length,vector<Condition> condition,vector<int> type){
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    vector<int> result(RecordOrder);
    for(int i=0;i<condition.size();i++){
        vector<int> newr;
        switch(condition[i].type){
            case -1://int
                for(int j=0;j<result.size();j++){
                    int a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    int b = atoi(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            case -2://float
                for(int j=0;j<result.size();j++){
                    float a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    float b = atof(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            default://char
                for(int j=0;j<result.size();j++){
                    char *a=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(a, p+condition[i].offset,condition[i].type);
                    *(a+condition[i].type)='\0';
                    char *b=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *q=condition[i].value.c_str();
                    memcpy(b,q,condition[i].type);
                    *(b+condition[i].type)='\0';
                    switch(condition[i].op){
                        case 0://<
                            if(strcmp(a,b)<0) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(strcmp(a,b)<=0) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(strcmp(a,b)>0) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(strcmp(a,b)>=0) newr.push_back(result[j]);
                            break;
                        case 4://=
                            cout<<strcmp(a,b)<<endl;
                            if(strcmp(a,b)==0) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(strcmp(a,b)!=0) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
        }
    }
    int i;
    vector<string> DeleteRecord;
    for(i=0;i<result.size();i++){
        string str;
        const char*p=AllRecord[RecordOrder[i]].c_str();
        int len = 0;
        stringstream ss;
        for (int j = 0; j<type.size(); j++) {
            if(type[j]==-1) {
                string s1;
                int a;
                memcpy(&a, p+len,4);
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else if (type[j]==-2){
                string s1;
                float a;
                memcpy(&a, p+len,4);
                stringstream ss;
                ss.clear();
                ss << a;
                ss >> s1;
                DeleteRecord.push_back(s1);
                len+=4;
            }
            else{
                char *a=(char*)malloc(sizeof(char)*(type[j]+1));
                memcpy(a,p+len,type[j]);
                *(a+type[j])='\0';
                DeleteRecord.push_back(string(a));
                len+=type[j];
            }
        }
        int m = bm.DeleteRecord(tablename, RecordOrder[i], record_length);
        if (m==CannotFindTable)
            break;
    }
    if (i<result.size()) {
        cout << "删除失败" << endl;
        return DeleteRecord;
    }
    cout << "成功删除 " << i << " 条记录!" << endl;
    return DeleteRecord;
}


void RecordManager::Create(string tablename, int record_length) {
    int m = bm.CreateTable(tablename, record_length, 0);
    if (m<0) {
        cout << "无法创建表\'" << tablename << "\'的块" << endl;
        return;
    }
    cout << "成功创建表\'" << tablename << "\'的块" << endl;
}

void RecordManager::Drop(string tablename) {
    int m = bm.DropTable(tablename);
    if (m == CannotFindTable) {
        cout << "无法成功删除\'" << tablename << "\'的块" << endl;
        return;
    }
    cout << "成功删除\'" << tablename << "\'的块" << endl;
}

void RecordManager::SelectNoCondition(string tablename, vector<string> attname, vector<int>type, vector<int>Offset, vector<int>length, int record_length) {
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int i;
    for (i = 0; i<attname.size(); i++) {
        if(type[i]>0){
            int len=attname[i].length();
            while(len<type[i]){
                attname[i]+=" ";
                len++;
            }
        }
        cout << attname[i] << "\t";
    }
    cout << endl;
    int num=0;
    for (i = 0; i<AllRecord.size(); i++) {
        const char *p = AllRecord[i].c_str();
        if(*p=='\0') continue;
        for (int j = 0; j<attname.size(); j++) {
            switch (type[j]) {
                case -1:
                    int a;
                    memcpy(&a, p + Offset[j], 4);
                    cout << a << "\t";
                    break;
                case -2:
                    float b;
                    memcpy(&b, p + Offset[j], 4);
                    cout << b << "\t";
                    break;
                default:
                    char *s = (char*)malloc(sizeof(char)*(type[j]+1));
                    memcpy(s, p + Offset[j], length[j]);
                    *(s + length[j]) = '\0';
                    string str(s);
                    cout << str << "\t";
            }
        }
        num++;
        cout << endl;
    }
    if (i<AllRecord.size()) {
        cout << "选择失败！" << endl;
        return;
    }
    cout << "一共输出 " << num << " 条记录!" << endl;
}
//只有普通项
void RecordManager::SelectWithConditionNoIndex(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, int record_length, vector<Condition> condition){
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int RecordNum = bm.TableRecordNumber(tablename,record_length);
    vector<int> result;
    for(int i=0;i<RecordNum;i++){
        if(AllRecord[i]!="")
            result.push_back(i);
    }
    for(int i=0;i<condition.size();i++){
      //  if(*p=='\0') continue;
        vector<int> newr;
        switch(condition[i].type){
            case -1://int
                for(int j=0;j<result.size();j++){
                    int a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    int b = atoi(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            case -2://float
                for(int j=0;j<result.size();j++){
                    float a;
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    float b = atof(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            default://char
                for(int j=0;j<result.size();j++){
                    char *a=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *p = AllRecord[result[j]].c_str();
                    memcpy(a, p+condition[i].offset,condition[i].type);
                    *(a+condition[i].type)='\0';
                    char *b=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *q=condition[i].value.c_str();
                    memcpy(b,q,condition[i].type);
                    *(b+condition[i].type)='\0';
                    switch(condition[i].op){
                        case 0://<
                            if(strcmp(a,b)<0) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(strcmp(a,b)<=0) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(strcmp(a,b)>0) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(strcmp(a,b)>=0) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(strcmp(a,b)==0) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(strcmp(a,b)!=0) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
        }
    }
    int i;
    for (i = 0; i<attname.size(); i++) {
        if(type[i]>0){
            int len=attname[i].length();
            while(len<type[i]){
                attname[i]+=" ";
                len++;
            }
        }
        cout << attname[i] << "\t";
    }
    cout << endl;
    //cout<<"鑳″崟鏄?<<endl; 
    //cout<<record.size()<<endl;
    for(i=0;i<result.size();i++){

        const char* p=AllRecord[result[i]].c_str();
        //if(*p=='\0') continue;
        for (int j = 0; j<attname.size(); j++) {
            switch (type[j]) {
                case -1:
                    int a;
                    memcpy(&a, p + Offset[j], 4);
                    cout << a << "\t";
                    break;
                case -2:
                    float b;
                    memcpy(&b, p + Offset[j], 4);
                    cout << b << "\t";
                    break;
                default:
                    char *s = (char*)malloc(sizeof(char)*(type[j]+1));
                    memcpy(s, p + Offset[j], length[j]);
                    *(s + length[j]) = '\0';
                    string str(s);
                    cout << str << "\t";
            }
        }
        cout<<endl;
    }
    if (i<result.size()) {
        cout << "选择失败！" << endl;
        return;
    }
    cout << "一共输出 " << i << " 条记录!" << endl;
}
//既有索引项，又有普通项
void RecordManager::SelectWithCondition(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, vector<int>RecordOrder, int record_length,vector<Condition> condition){
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    vector<int> result(RecordOrder);
    for(int i=0;i<condition.size();i++){
        vector<int> newr;
        switch(condition[i].type){
            case -1://int
                for(int j=0;j<result.size();j++){
                    int a;
                    const char *p = AllRecord[result[j]].c_str();
                    if(*p=='\0') break;
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    int b = atoi(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            case -2://float
                for(int j=0;j<result.size();j++){
                    float a;
                    const char *p = AllRecord[result[j]].c_str();
                    if(*p=='\0') break;
                    memcpy(&a, p+condition[i].offset,4);
                    const char *q=condition[i].value.c_str();
                    float b = atof(q);
                    switch(condition[i].op){
                        case 0://<
                            if(a<b) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(a<=b) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(a>b) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(a>=b) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(a==b) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(a!=b) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
            default://char
                for(int j=0;j<result.size();j++){
                    const char *p = AllRecord[result[j]].c_str();
                    if(*p=='\0') break;
                    char *a=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    memcpy(a, p+condition[i].offset,condition[i].type);
                    *(a+condition[i].type)='\0';
                    char *b=(char*)malloc(sizeof(char)*(condition[i].type+1));
                    const char *q=condition[i].value.c_str();
                    memcpy(b,q,condition[i].type);
                    *(b+condition[i].type)='\0';
                    switch(condition[i].op){
                        case 0://<
                            if(strcmp(a,b)<0) newr.push_back(result[j]);
                            break;
                        case 1://<=
                            if(strcmp(a,b)<=0) newr.push_back(result[j]);
                            break;
                        case 2://>
                            if(strcmp(a,b)>0) newr.push_back(result[j]);
                            break;
                        case 3://>=
                            if(strcmp(a,b)>=0) newr.push_back(result[j]);
                            break;
                        case 4://=
                            if(strcmp(a,b)==0) newr.push_back(result[j]);
                            break;
                        case 5://<>
                            if(strcmp(a,b)!=0) newr.push_back(result[j]);
                            break;
                    }
                }
                result.clear();
                result.assign(newr.begin(),newr.end());
                newr.clear();
                break;
        }
    }
    int i;
    for (i = 0; i<attname.size(); i++) {
        if(type[i]>0){
            int len=attname[i].length();
            while(len<type[i]){
                attname[i]+=" ";
                len++;
            }
        }
        cout << attname[i] << "\t";
    }
    cout << endl;
    for(i=0;i<result.size();i++){
        const char* p=AllRecord[result[i]].c_str();
        for (int j = 0; j<attname.size(); j++) {
            switch (type[j]) {
                case -1:
                    int a;
                    memcpy(&a, p + Offset[j], 4);
                    cout << a << "\t";
                    break;
                case -2:
                    float b;
                    memcpy(&b, p + Offset[j], 4);
                    cout << b << "\t";
                    break;
                default:
                    char *s = (char*)malloc(sizeof(char)*(type[j]+1));
                    memcpy(s, p + Offset[j], length[j]);
                    *(s + length[j]) = '\0';
                    string str(s);
                    cout << str << "\t";
            }
        }
        cout<<endl;
    }
    if (i<result.size()) {
        cout << "选择失败！" << endl;
        return;
    }
    cout << "一共输出 " << i << " 条记录!" << endl;
    
}
void RecordManager::SelectOnlyWithIndex(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int> length, vector<int>RecordOrder, int record_length) {
    vector<string> AllRecord;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int i;
    for (i = 0; i<attname.size(); i++){
        if(type[i]>0){
            int len=attname[i].length();
            while(len<type[i]){
                attname[i]+=" ";
                len++;
            }
        }
        cout << attname[i] << "\t";
    }
    cout << endl;
    for (i = 0; i<RecordOrder.size(); i++) {
        const char *p = AllRecord[RecordOrder[i]].c_str();
        if(*p=='\0') continue;
        for (int j = 0; j<Offset.size(); j++) {
            switch (type[j]) {
                case -1:
                    int a;
                    memcpy(&a, p + Offset[j], 4);
                    cout << a << "\t";
                    break;
                case -2:
                    float b;
                    memcpy(&b, p + Offset[j], 4);
                    cout << b << "\t";
                    break;
                default:
                    char *s = (char*)malloc(sizeof(char)*(type[j]+1));
                    memcpy(s, p + Offset[j], length[j]);
                    *(s + length[j]) = '\0';
                    string str(s);
                    cout << str << "\t";
            }
        }
        cout << endl;
    }
    if (i<RecordOrder.size()) {
        cout << "条件选择失败！" << endl;
        return;
    }
    cout << "一共输出符合条件的 " << i << " 条记录！" << endl;
}

void RecordManager::UpdateFile() {
    cout << "Update" << endl;
    bm.StoreBuffer();
}
void RecordManager::CreateIndexOnUnique(string tablename,int offset,int type,int record_length, vector<string>& s,vector<int>& r){
    vector<string> AllRecord;
    stringstream ss;
    AllRecord = bm.GetTableRecord(tablename, record_length);
    int RecordNum = bm.TableRecordNumber(tablename,record_length);
    for(int i=0;i<RecordNum;i++){
        r.push_back(i);
    }
    for (int i = 0; i<AllRecord.size(); i++) {
        //int len=0;
        const char *p = AllRecord[i].c_str();
        if(*p=='\0') continue;
            if(type==-1) {
                string s1;
                int a;
                memcpy(&a, p+offset,4);
                ss.clear();
                ss << a;
                ss >> s1;
                s.push_back(s1);
            }
            else if (type==-2){
                string s1;
                float a;
                memcpy(&a, p+offset,4);
                stringstream ss;
                ss.clear();
                ss << a;
                ss >> s1;
                s.push_back(s1);
            }
            else{
                char *a=(char*)malloc(sizeof(char)*(type+1));
                memcpy(a,p+offset,type);
                *(a+type)='\0';
                s.push_back(string(a));
            }
    }
}
