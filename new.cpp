

#include "interpreter.h"
#include<iostream>
#include <fstream>
#include "Record.h"
#include "IndexManager.h"
#include <cmath>
#include "Catalog.h"
#include<algorithm>

int main()
{
    interpret parse;
    string line;
    CatalogManager catalog;
    RecordManager record;
    IndexManager ind;
    //IndexManager INDEX;
    vector<string>::iterator it;
    vector<int>::iterator it2;
    vector<int>::iterator it3;
    vector<string>::iterator it4;
    vector<int> ::iterator ins_it_type;
    vector<Attribute> ::iterator ins_st_it;
    vector<Attribute>::iterator it_at;
    vector<Attribute>::iterator it_a2;
    int len;
    string ind_name;
    int t_attr_size;
    ofstream out;
    int wr_type = 0;
    int not_find = 0;
    ind.InitializeTree(catalog.cindex);
    int RecordOffset;
    string debug;
    
    //new added
    
    vector<Index>::iterator sel_ind_it;
    vector<Index> sel_ind;
    vector<Condition> ::iterator con_it;
    vector<Condition> sel_no;
    vector<int> re_ord;//Ã–Â»Ã“Ãindex
    vector<int> re_off;
    vector<int> re_tmp;
    vector<int> sel_con;
    vector<Index> TableIndexInfo;
    vector<string> Del_Index;
    int del_off;
    //vector<int> re_res;
    int del_to;
    
    int sel_flag;
    while (1) {
        sel_ind.clear();
        sel_flag=0;
        sel_no.clear();
        re_ord.clear();
        re_off.clear();
        re_tmp.clear();
        sel_con.clear();
        parse.Parse();
        vector<string> s;
        vector<int> r;
        int s_count;
        TableIndexInfo.clear();
        switch (parse.opcode)
        {
            case CreateTabErr: {
                cout << "Invalid Input" << endl;
                break;
            }
            case DropTab:
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Not Existed" << endl;
                    break;
                }
                catalog.DropTable(parse.m_tabname);
                record.Drop(parse.m_tabname);
                TableIndexInfo=catalog.getTableIndex(parse.m_tabname);
                for (sel_ind_it =TableIndexInfo.begin();sel_ind_it <TableIndexInfo.end(); sel_ind_it++)
                    ind.dropIndex(sel_ind_it->name);
                break;
            case DropTabErr:
            case DropIndErr:
            case CreateIndErr:
                cout << "Invalid Input" << endl;
                break;
            case TableExistedErr:
                cout << "Table Not Existed" << endl;
                break;
            case Sel_No_Attr:
                cout << "The attribute doesn't exsit in the table" << endl;
            case TableExisted:
                cout << "There is already a table named " << parse.m_tabname << "!" << endl;
                cout << "You can't redefine an existed table!" << endl;
                break;
            case DrpInd:
                catalog.DropIndex(parse.m_indname);
                ind.dropIndex(parse.m_indname);
                //cout << "DrpInd succeed" << endl;
                break;
            case CreateTab:
                if((catalog.CreateTable(parse.m_tabname, parse.getTableinfo.attr,parse.findPrimary))==-1) break;                record.Create(parse.m_tabname, catalog.GetRecordLength(parse.m_tabname));
                ind_name = parse.getTableinfo.name + "_primary_index";
                ind.createIndex(ind_name, parse.primtype);//create table/create index int float n
                //parse.~parse();
                break;
            case CreateInd:
                if(catalog.IsTableExist(parse.m_tabname)==-1) break;
                catalog.CreateIndex(parse.m_tabname, parse.getIndexinfo.attrname, parse.getIndexinfo.name,catalog.GetAttrNameOffset(parse.m_tabname,parse.getIndexinfo.attrname));
                record.CreateIndexOnUnique(parse.m_tabname,catalog.GetOffset(parse.m_tabname, parse.getIndexinfo.attrname),catalog.GetAttrType(parse.m_tabname, parse.getIndexinfo.attrname),catalog.GetRecordLength(parse.m_tabname),s, r);
                ind.createIndex(parse.getIndexinfo.name, catalog.GetAttrType(parse.m_tabname, parse.getIndexinfo.attrname));
                for (s_count = 0; s_count < s.size(); s_count++)
                    ind.InsertIntoIndex(parse.getIndexinfo.name,s[s_count], r[s_count]);
                
                //cout << "CreateInd" << endl;
                break;
            case SELECT_NOWHERE_CAULSE:
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Do Not Exist" << endl;
                    break;
                }
                parse.getTableinfo = catalog.getTableInformation(parse.m_tabname);
                not_find = 0;
                if (parse.all) {
                    parse.col_info = parse.getTableinfo.attr;
                    parse.ins_value = catalog.getAllAttrName(parse.m_tabname);
                }
                for (it_at = parse.col_info.begin(); it_at != parse.col_info.end(); it_at++) {
                    
                    for (it_a2 = parse.getTableinfo.attr.begin(); it_a2 != parse.getTableinfo.attr.end(); it_a2++) {
                        if (it_a2->name == it_at->name) break;
                    }
                    if (it_a2 == parse.getTableinfo.attr.end()) not_find = 1;
                    
                    
                    if (!not_find) {
                        parse.sel_val_off.push_back(it_a2->offset);
                        parse.ins_type.push_back(it_a2->type);
                        if (it_a2->type == -1 || it_a2->type == -2) parse.sel_len.push_back(4);
                        else parse.sel_len.push_back(it_a2->type);
                    }
                }
                if (not_find) {
                    cout << "The attribute doesn't exsited!" << endl;
                    break;
                }
                
                if (parse.all == 1)
                    record.SelectNoCondition(parse.m_tabname, parse.ins_value, catalog.GetTableAttributeType(parse.m_tabname), parse.sel_val_off, parse.sel_len, catalog.GetRecordLength(parse.m_tabname));
                else
                    record.SelectNoCondition(parse.m_tabname, parse.attnames, parse.ins_type, parse.sel_val_off, parse.sel_len, catalog.GetRecordLength(parse.m_tabname));
                //cout << "select no where" << endl;
                break;
            case SELECT_WHERE_CAULSE:
                
                //cout<< "Select with Condition "<< endl;
                
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Do Not Exist" << endl;
                    break;
                }
                //parse.col_info: the attribute that the user tap
                parse.getTableinfo = catalog.getTableInformation(parse.m_tabname);
                not_find = 0;
                if (parse.all) {
                    parse.col_info = parse.getTableinfo.attr;
                    parse.ins_value = catalog.getAllAttrName(parse.m_tabname);
                }
                for (it_at = parse.col_info.begin(); it_at != parse.col_info.end(); it_at++) {
                    
                    for (it_a2 = parse.getTableinfo.attr.begin(); it_a2 != parse.getTableinfo.attr.end(); it_a2++) {
                        if (it_a2->name == it_at->name) break;
                    }
                    if (it_a2 == parse.getTableinfo.attr.end()) not_find = 1;
                    
                    
                    if (!not_find) {
                        parse.sel_val_off.push_back(it_a2->offset);
                        parse.ins_type.push_back(it_a2->type);
                        if (it_a2->type == -1 || it_a2->type == -2) parse.sel_len.push_back(4);
                        else parse.sel_len.push_back(it_a2->type);
                    }
                }
                if (not_find) {
                    cout << "The attribute doesn't exsited!" << endl;
                    break;
                }
                
                //Â²Â¹Â³Ã¤ con_itÃ–ÃÂµÃ„type ÂºÃ offset
                for (con_it = parse.condition.begin(); con_it != parse.condition.end(); con_it++) {
                    for (it_a2 = parse.getTableinfo.attr.begin(); it_a2 != parse.getTableinfo.attr.end(); it_a2++) {
                        
                        if (con_it->attrname == it_a2->name) {
                            break;
                        }
                    }
                    if (it_a2 == parse.getTableinfo.attr.end()) not_find = 1;
                    if (!not_find) {
                        con_it->type = it_a2->type;
                        con_it->offset = it_a2->offset;
                        if((*con_it).type>0){
                            len=(*con_it).value.length();
                            while ((*con_it).type > len) {
                                (*con_it).value += " ";
                                len++;
                            }
                            //cout <<"Length: " <<len << endl;
                            len = 0;
                        }
                        //cout<<(it_a2->type)<<endl;
                        //cout<<(it_a2->offset)<<endl;
                    }
                }
                //Â´Ã“indexÃ„Ã‡Ã€Ã¯Â»Ã±ÂµÃƒÂµÃ„ vector<Index>
                sel_ind = catalog.getTableIndex(parse.m_tabname);
                
                for (con_it = parse.condition.begin(); con_it != parse.condition.end(); con_it++) {
                    for (sel_ind_it = sel_ind.begin(); sel_ind_it != sel_ind.end(); sel_ind_it++) {
                        if (sel_ind_it->attrname == con_it->attrname) {
                            if (re_ord.size() == 0){
                                re_ord = ind.searchIndex(sel_ind_it->name, con_it->value, con_it->op);
                            }
                            
                            else {
                                re_tmp = ind.searchIndex(sel_ind_it->name, con_it->value, con_it->op);
                                // set_intersection( one.begin(), one.end(), two.begin(), two.end() ,result.begin());
                                it2 = set_intersection(re_ord.begin(), re_ord.end(), re_tmp.begin(), re_tmp.end(), re_ord.begin());
                                //result.resize( retEndPos - result.begin() )
                                re_ord.resize(it2 - re_ord.begin());
                            }
                            sel_flag = 1;
                            break;
                        }
                    }
                    if (!sel_flag) {
                        sel_no.push_back(*con_it);
                    }
                    sel_flag = 0;
                }
                
                out.open("sel_con.txt", ios::app);
                out << parse.m_tabname << endl;
                out << "select string value:" << endl;//Ã©Å“â‚¬Ã¨Â¦ÂÃ¨Â¡Â¥0
                for (it = parse.attnames.begin(); it != parse.attnames.end(); it++) {
                    out << *it << endl;
                }
                
                out << "select string type:" << endl;
                sel_con=catalog.GetTableAttributeType(parse.m_tabname);
                for (it2 = parse.ins_type.begin(); it2 != parse.ins_type.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "select val offset:" << endl;
                for (it2 = parse.sel_val_off.begin(); it2 != parse.sel_val_off.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "select length:" << endl;
                for (it2 = parse.sel_len.begin(); it2 != parse.sel_len.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "select record length" << endl;
                out << parse.getTableinfo.recordlength << endl;
                out<<"select condition"<<endl;
                for(con_it=parse.condition.begin();con_it!=parse.condition.end();con_it++){
                    out<<con_it->value<<endl;
                    out<<con_it->op<<endl;
                    out<<con_it->attrname<<endl;
                    out<<con_it->type<<endl;
                    out<<con_it->offset<<endl;
                }
                
                out.close();
                
                if (parse.all == 1) {
                    if (re_ord.size() && !sel_no.size()) {
                        record.SelectOnlyWithIndex(parse.m_tabname, parse.ins_value, sel_con, parse.sel_val_off, parse.sel_len, re_ord, catalog.GetRecordLength(parse.m_tabname));
                    }
                    //
                    else if (!re_ord.size() && sel_no.size()) {
                        //void SelectWithConditionNoIndex(string tablename, vector<string> attname, vector<int>type, vector<int> Offset, vector<int>length, int record_length, vector<Condition>);
                        record.SelectWithConditionNoIndex(parse.m_tabname, parse.ins_value, sel_con, parse.sel_val_off, parse.sel_len, catalog.GetRecordLength(parse.m_tabname),sel_no);
                        
                    }
                    else {
                        record.SelectWithCondition(parse.m_tabname, parse.ins_value, sel_con, parse.sel_val_off, parse.sel_len, re_ord, catalog.GetRecordLength(parse.m_tabname), sel_no);
                        
                    }
                    
                }
                else {
                    if (re_ord.size() && !sel_no.size()) {
                        record.SelectOnlyWithIndex(parse.m_tabname, parse.attnames, parse.ins_type, parse.sel_val_off, parse.sel_len, re_ord, catalog.GetRecordLength(parse.m_tabname));
                    }
                    //
                    else if (!re_ord.size() && sel_no.size()) {
                        record.SelectWithConditionNoIndex(parse.m_tabname, parse.attnames, parse.ins_type, parse.sel_val_off, parse.sel_len, catalog.GetRecordLength(parse.m_tabname), sel_no);
                        
                    }
                    else {
                        record.SelectWithCondition(parse.m_tabname, parse.attnames, parse.ins_type, parse.sel_val_off, parse.sel_len, re_ord, catalog.GetRecordLength(parse.m_tabname), sel_no);
                        
                    }
                }
                break;
            case Quit:
                if (!parse.QuitFile())													//wjs
                {
                    //catalog.UpdateIndex();
                    
                    //void CatalogManager::UpdateIndex(vector<IndexChangeInfo*> ChangeIndexInfo)
                    catalog.UpdateIndex(ind.WriteBackTreeInfo());
                    catalog.UpdateTable();
                    record.UpdateFile();
                    return 0;
                }
                break;																	//wjf
            case Del_Nowhere:
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Do Not Exist" << endl;
                    break;
                }
                parse.getTableinfo = catalog.getTableInformation(parse.m_tabname);
                not_find = 0;
                
                record.DeleteNoCondition(parse.m_tabname, catalog.GetRecordLength(parse.m_tabname));
                cout << "delete no where" << endl;
                break;
            case Del:
                
                //cout<< "delete with Condition "<< endl;
                
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Do Not Exist" << endl;
                    break;
                }
                //parse.col_info: the attribute that the user tap
                parse.getTableinfo = catalog.getTableInformation(parse.m_tabname);
                not_find = 0;
                
                //²¹³ä con_itÖÐµÄtype ºÍ offset
                for (con_it = parse.condition.begin(); con_it != parse.condition.end(); con_it++) {
                    for (it_a2 = parse.getTableinfo.attr.begin(); it_a2 != parse.getTableinfo.attr.end(); it_a2++) {
                        
                        if (con_it->attrname == it_a2->name) {
                            break;
                        }
                    }
                    if (it_a2 == parse.getTableinfo.attr.end()) not_find = 1;
                    if (!not_find) {
                        con_it->type = it_a2->type;
                        con_it->offset = it_a2->offset;
                        if((*con_it).type>0){
                            len=(*con_it).value.length();
                            while ((*con_it).type > len) {
                                (*con_it).value += " ";
                                len++;
                            }
                            //cout <<"Length: " <<len << endl;
                            len = 0;
                        }
                        //cout<<(it_a2->type)<<endl;
                        //cout<<(it_a2->offset)<<endl;
                    }
                }
                
                sel_ind = catalog.getTableIndex(parse.m_tabname);
                
                for (con_it = parse.condition.begin(); con_it != parse.condition.end(); con_it++) {
                    for (sel_ind_it = sel_ind.begin(); sel_ind_it != sel_ind.end(); sel_ind_it++) {
                        if (sel_ind_it->attrname == con_it->attrname) {
                            if (re_ord.size() == 0){
                                re_ord = ind.searchIndex(sel_ind_it->name, con_it->value, con_it->op);
                            }
                            
                            else {
                                re_tmp = ind.searchIndex(sel_ind_it->name, con_it->value, con_it->op);
                                // set_intersection( one.begin(), one.end(), two.begin(), two.end() ,result.begin());
                                it2 = set_intersection(re_ord.begin(), re_ord.end(), re_tmp.begin(), re_tmp.end(), re_ord.begin());
                                //result.resize( retEndPos - result.begin() )
                                re_ord.resize(it2 - re_ord.begin());
                            }
                            sel_flag = 1;
                            break;
                        }
                    }
                    if (!sel_flag) {
                        sel_no.push_back(*con_it);
                    }
                    sel_flag = 0;
                }
                
                out.open("del_con.txt", ios::app);
                out << parse.m_tabname << endl;
                out << "delete string value:" << endl;//Ã©Å“â‚¬Ã¨Â¦ÂÃ¨Â¡Â¥0
                for (it = parse.attnames.begin(); it != parse.attnames.end(); it++) {
                    out << *it << endl;
                }
                
                out << "delete string type:" << endl;
                sel_con=catalog.GetTableAttributeType(parse.m_tabname);
                for (it2 = parse.ins_type.begin(); it2 != parse.ins_type.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "delete val offset:" << endl;
                for (it2 = parse.sel_val_off.begin(); it2 != parse.sel_val_off.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "delete length:" << endl;
                for (it2 = parse.sel_len.begin(); it2 != parse.sel_len.end(); it2++) {
                    out << *it2 << endl;
                }
                out << "delete record length" << endl;
                out << parse.getTableinfo.recordlength << endl;
                out<<"delete condition"<<endl;
                for(con_it=parse.condition.begin();con_it!=parse.condition.end();con_it++){
                    out<<con_it->value<<endl;
                    out<<con_it->op<<endl;
                    out<<con_it->attrname<<endl;
                    out<<con_it->type<<endl;
                    out<<con_it->offset<<endl;
                }
                
                out.close();
                del_to=parse.getTableinfo.attr.size();
                if (re_ord.size() && !sel_no.size()) {
                    //DeleteOnlyWithIndex(string tablename, vector<int>RecordOrder, int record_length,vector<int> type);
                    Del_Index=record.DeleteOnlyWithIndex(parse.m_tabname, re_ord, catalog.GetRecordLength(parse.m_tabname),catalog.GetTableAttributeType(parse.m_tabname));
                    //Del_Index ÊÇ·µ»ØµÄstring
                    
                    for(sel_ind_it=sel_ind.begin();sel_ind_it!=sel_ind.end();sel_ind_it++){
                        del_off=catalog.GetAttrNameOffset(sel_ind_it->tablename, sel_ind_it->attrname);
                        for(int j= del_off;j<Del_Index.size();j+=del_to){
                            ind.DeleteInIndex(sel_ind_it->name, Del_Index[j]);
                        }
                    }
                }
                //	vector<string> DeleteWithConditionNoIndex(string tablename, int record_length, vector<Condition> condition,vector<int> type);
                else if (!re_ord.size() && sel_no.size()) {
                    record.DeleteWithConditionNoIndex(parse.m_tabname,  catalog.GetRecordLength(parse.m_tabname), sel_no,catalog.GetTableAttributeType(parse.m_tabname));
                    for(sel_ind_it=sel_ind.begin();sel_ind_it!=sel_ind.end();sel_ind_it++){
                        del_off=catalog.GetAttrNameOffset(sel_ind_it->tablename, sel_ind_it->attrname);
                        for(int j= del_off;j<Del_Index.size();j+=del_to){
                            ind.DeleteInIndex(sel_ind_it->name, Del_Index[j]);
                        }
                    }
                }
                else {
                    record.DeleteWithCondition(parse.m_tabname, re_ord, catalog.GetRecordLength(parse.m_tabname), sel_no,catalog.GetTableAttributeType(parse.m_tabname));
                    for(sel_ind_it=sel_ind.begin();sel_ind_it!=sel_ind.end();sel_ind_it++){
                        del_off=catalog.GetAttrNameOffset(sel_ind_it->tablename, sel_ind_it->attrname);
                        for(int j= del_off;j<Del_Index.size();j+=del_to){
                            ind.DeleteInIndex(sel_ind_it->name, Del_Index[j]);
                        }
                    }
                }
                
                break;
            case Ins:
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Do Not Exist" << endl;
                    break;
                }
                parse.getTableinfo = catalog.getTableInformation(parse.m_tabname);
                if (catalog.IsTableExist(parse.m_tabname) == -1) {
                    cout << "Table Existed" << endl;
                    break;
                }
                int ins_c;
                for (ins_c = 0, ins_st_it = parse.getTableinfo.attr.begin(), it = parse.ins_value.begin(); ins_st_it != parse.getTableinfo.attr.end() && it != parse.ins_value.end(); ins_c++, it++, ins_st_it++) {
                    if (ins_st_it->type == -1) {
                        if (!parse.IsInt((char *)(*it).c_str())) {
                            cout << "Type Imcompatible!" << endl;
                            break;
                        }
                    }
                    else if (ins_st_it->type == -2) {
                        if (!parse.IsFloat((char *)(*it).c_str())) {
                            cout << "Type Imcompatible!" << endl;
                            break;
                        }
                    }
                    else {
                        len = (*it).length();
                        
                        while (ins_st_it->type > len) {
                            (*it) += " ";
                            len++;
                        }
                        //cout << len << endl;
                        len = 0;
                    }
                    parse.ins_type.push_back(ins_st_it->type);
                    parse.ins_keytype.push_back(ins_st_it->key_type);
                }
                if (wr_type) {
                    wr_type = 0;
                    break;
                }
                t_attr_size = parse.getTableinfo.attr.size();
                if (parse.ins_value.size() != t_attr_size) {
                    cout << "Invalid Value" << endl;
                    break;
                }
                /*
                 out.open("ins_info.txt", ios::app);
                 out << parse.m_tabname << endl;
                 out << "insert string value:" << endl;
                 for (it = parse.ins_value.begin(); it != parse.ins_value.end(); it++) {
                 out << *it << endl;
                 }
                 out << "insert string type:" << endl;
                 for (it2 = parse.ins_type.begin(); it2 != parse.ins_type.end(); it2++) {
                 out << *it2 << endl;
                 }
                 out << "insert string key_type:" << endl;
                 for (it3 = parse.ins_keytype.begin(); it3 != parse.ins_keytype.end(); it3++) {
                 out << *it3 << endl;
                 }
                 out << "record length" << endl;
                 out << parse.getTableinfo.recordlength << endl;
                 out.close();
                 */
                RecordOffset = record.Insert(parse.m_tabname, parse.ins_value, parse.ins_type, parse.ins_keytype, parse.getTableinfo.recordlength);
                TableIndexInfo = catalog.getTableIndex(parse.m_tabname);
                if(RecordOffset==-1) break;
                //cout<<RecordOffset<<endl;
                //cout<<"insert fails"<<endl;
                for (int i=0; i<TableIndexInfo.size(); i++)
                    ind.InsertIntoIndex(TableIndexInfo[i].name, parse.ins_value[TableIndexInfo[i].AttrNameOffset], RecordOffset);
                //cout << "insert succeed!" << endl;
                //cout<<"insert succedd!"<<endl;
                break;
            case InvalidPrimary:
                cout << "primary key existed" << endl;
                break;
            case InsNumErr:
                cout << "Invalid Input" << endl;
                break;
            case ExecFile:														//wjs
                if (!(parse.NewFile())) cout << "The file do not exist" << endl;
                break;
            case ExecFileErr:
                cout << "Execute no file" << endl;
                break;															//wjf
            default:
                break;
        }
        
    }
    return 0;
}

