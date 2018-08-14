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
	bool mulremark;//����ע�Ϳ���
protected:
public:
	CProperties();
	virtual ~CProperties();
	//���ļ�
	PSTATUS open(const char* path);
	//ʵ���ļ����ص�map��
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



