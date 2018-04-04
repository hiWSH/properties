#include "stdafx.h"
#include "properties.h"
/************************************************************************************************/
///*
//*���ߣ����ۻ�
//*email��loveofprogram@sina.com
//*ʱ�䣺20180404
//*�汾���̣�
//*�汾��    ������      ����    
//*V1.0.0.1  ���ۻ�      ����  
//***************************************************************************************************
//***************************************************************************************************
//*��������ʵ��c++��properties�ļ��Ķ�д������
//*1.֧�ֵ���ע���磺���׳�ȥ�ո���� //��#��<!--����--> ��ʼ��
//* //���ǵ���ע��
//* #���ǵ���ע��
//* <!--���ǵ���ע��-->
//*2.֧�ֽڵ� ���ڵ㲢û������ [section]
//*[section0]
//*key=value
//*[section1]
//*key=value
//*�õ�multimap���Դ���key����2��ֵ���ѣ��ڵ㲢û�������ֵ����ã���һ��Ҳ����properties�����ļ�Ҫ��淶��
//*3.֧�ֶ���ע�� /*��һ�У��ڶ���.....*/
//*������1��ͬ����/*..*/ �磺key=123/*oooooo*/b=890  �������key=123
//*������2��������/*..*/ �磺key=123/*ooo
//*0000000000000000000
//*ooo*/b=890  �������key=123
//*������3����ͷ��β��/*..*/�磺/*ooooo*/key=123 ���������
//*
//*/
/************************************************************************************************/
static vector<string> vLine;
static multimap<string,string> msKV;
static bool mulremark = false;//����ע�Ϳ���

CProperties::CProperties(){};
CProperties::~CProperties(){};

PSTATUS CProperties::open(const char* path){
	/**************************************************************/
	//�ж��Ƿ����ı��ĵ� �������ļ��� �˴����������һֱhFind = 0xFFFFFFFF
	//���� �Ա��պ��Ż�
	//author�����ۻ�
	//time��  20180404
	/*WIN32_FIND_DATA fd; 
	bool ret = true;
	HANDLE hFind = FindFirstFile(LPCWSTR(path), &fd); 
	if ((hFind != INVALID_HANDLE_VALUE) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
	{ 
		ret = false;
	}
	FindClose(hFind); 
	if (ret)
	{
		//�ļ����Բ���
		return IS_ERROR;
	}*/
	/**************************************************************/
	if (nullptr == path)
	{
		return IS_ERROR;
	}
	this->path = path;
	ifstream ifs;
	ifs.open(path,ios::in);
	if (!ifs)
	{
		return IS_ERROR;//���ļ�ʧ��
	}
	string sLine;
	while(!ifs.eof()){
		sLine = "";
		getline(ifs,sLine);
		if (mulremark)//�Ѵ򿪶���ע�Ϳ��� ���жϸ�����û�йرտ���
		{
			if(sLine.find("*/") != string::npos){
				mulremark = false;
			}
			continue;//���ۿ����Ƿ�ر� ��������һ������
		}else
		{
			string::size_type pos =sLine.find("/*");string sSubLine;
			if (pos != string::npos)//�����ж���ע�Ϳ���  ���
			{
				string::size_type epos = sLine.rfind("*/");
				mulremark = epos==string::npos || epos<pos?true:false;
				sSubLine = sLine.substr(0,pos);
			}else{
				sSubLine = sLine;
			}
			trim(sSubLine);
			if (sSubLine.length()<= 0) continue;
			if (sSubLine[0] == '#')continue;
			if (sSubLine[0] == '[')continue;
			if (sSubLine.length()>2 && sSubLine[0] == '/' && sSubLine[1] == '/')continue;
			if (sSubLine.length()>4 && sSubLine[0] == '<' && sSubLine[1] == '!')continue;
			vLine.push_back(sSubLine);
		}
	}
	if (ifs.is_open())
	{
		ifs.close();
	}
	return IS_OK;
}
//ʵ���ļ����ص�map��
PSTATUS CProperties::load(){
	string key,value;string sSubStr;
	for (int i = 0;i<vLine.size();i++)
	{
		string::size_type pos = vLine.at(i).find_first_of("=");
		if (pos == string::npos)
		{
			continue;
		}
		key = vLine.at(i).substr(0,pos);
		value = vLine.at(i).substr(pos+1,vLine.at(i).size()-pos);
		msKV.insert(make_pair(key,value));
	}
	return IS_OK;
}

void CProperties::print(){
	multimap<string,string>::iterator  itr = msKV.begin();
	cout<<"################################################################################"<<endl;
	for (;itr!=msKV.end();itr++)
	{
		cout<<"key:"<< itr->first.c_str()<<";value:"<<itr->second.c_str()<<endl;
	}
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
}

vector<string> CProperties::read(const char *k){
	vector<string> vVauleCol;
	if (msKV.size()>0)
	{
		multimap<string, string>::size_type  cnt = msKV.count(k);
		multimap<string, string>::iterator  iter = msKV.find(k);
		for(;cnt > 0; cnt--, iter++)
		{
			vVauleCol.push_back(iter->second);
		}
	}
	return vVauleCol;
}

/*
*aim:��key=value׷�ӵ��ı�ĩβ������multimapӳ�����
*/
PSTATUS CProperties::write(const char *k,const char* v){
	if (nullptr == k || nullptr == v)
	{
		return IS_ERROR; //У�����
	}
	ofstream ofs;
	ofs.open(this->path.c_str(),ios::app);
	if(!ofs)
	{
		return IS_ERROR;//���ļ�ʧ��
	}
	char sStr[1024] = {};
	sprintf(sStr,"%s=%s",k,v);
	ofs<<endl<<sStr;
	msKV.insert(make_pair(k,v));
	if(ofs.is_open())
	{
		ofs.close();
	}
	return IS_OK;
}

/*
*aim:��ֹ������������ݸ���
*/
void CProperties::close(){
	vLine.erase(vLine.begin(),vLine.end());
	msKV.erase(msKV.begin(),msKV.end());
}

void CProperties::trim_first(string &s){
	if( !s.empty() )
		s.erase(0,s.find_first_not_of(" "));
}
void CProperties::trim_end(string &s){
	if( !s.empty() )
		s.erase(s.find_last_not_of(" ") + 1);
}
void CProperties::trim(string &s)
{
	int index = 0;
	if( !s.empty())
	{
		while( (index = s.find(' ',index)) != string::npos)
		{
			s.erase(index,1);
		}
	}
}

/*

int _tmain1(int argc, _TCHAR* argv[])
{
//########################################################//
//read
//1.��������open()�ļ�
//2.load()���ص��ڴ���
//3.read()��ȡ���keyֵ��value
//4.close()�ͷ���Դ
/*
CProperties cprop;
PSTATUS ret = cprop.open("D:\\job\\greatwall\\test\\bank.properties");
if (ret != IS_OK)
{
	cout<<"�������ļ�ʧ��"<<endl;
	getchar();
	return 0;
}
cprop.load();
vector<string> vec = cprop.read("key3");
for (int i=0; i<vec.size();i++)
{
	cout<<vec[i].c_str()<<endl;
}
cprop.close();*/
//########################################################//

//########################################################//
//read
//1.��������
//2.load()���ص��ڴ���
//3.write()��ȡ���keyֵ��value
//4.close()�ͷ���Դ
/*
CProperties cprop_write;
ret = cprop_write.open("D:\\job\\greatwall\\test\\bank2.properties");
cprop_write.write("aaa","bbb");
cprop_write.write("aaa","bbb");
vec = cprop_write.read("aaa");
for (int i=0; i<vec.size();i++)
{
	cout<<vec[i].c_str()<<endl;
}
cprop_write.close();
*/
//########################################################//
/*
getchar();
return 0;
}
*/