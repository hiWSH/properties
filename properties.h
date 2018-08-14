#pragma once

#ifndef CProperties_H
#define CProperties_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
using namespace std;

#define  IS_PROPERTIES_OK 1
#define	 IS_PROPERTIES_ERROR 0
typedef  bool PSTATUS;
class CProperties{
private:
	string path;
	vector<string> vLine;
	multimap<string,string> msKV;
	bool mulremark;//多行注释开关
protected:
public:
	CProperties();
	virtual ~CProperties();
	//打开文件
	PSTATUS open(const char* path);
	//实现文件加载到map中
	PSTATUS load();
	void print();
	void close();
	vector<string> read(const char *k);
	PSTATUS write(const char *k,const char* v);
	PSTATUS modify(const char *k,const char* v);
	void trim_first(string &s);
	void trim_end(string &s);
	void trim(string &s);
};

#endif



