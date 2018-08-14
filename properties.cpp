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

CProperties::CProperties(){mulremark = false;};
CProperties::~CProperties(){};

PSTATUS CProperties::open(const char* path){
	
	if (nullptr == path)
	{
		return IS_PROPERTIES_ERROR;
	}
	this->path = path;
	ifstream ifs;
	ifs.open(path,ios::in);
	if (!ifs)
	{
		return IS_PROPERTIES_ERROR;//���ļ�ʧ��
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
	return IS_PROPERTIES_OK;
}
//ʵ���ļ����ص�map��
PSTATUS CProperties::load(){
	string key,value;string sSubStr;
	for (unsigned int i = 0;i<vLine.size();i++)
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
	return IS_PROPERTIES_OK;
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
		return IS_PROPERTIES_ERROR; //У�����
	}
	ofstream ofs;
	ofs.open(this->path.c_str(),ios::app);
	if(!ofs)
	{
		return IS_PROPERTIES_ERROR;//���ļ�ʧ��
	}
	char sStr[1024] = {};
	sprintf_s(sStr,1024,"%s=%s",k,v);
	ofs<<endl<<sStr;
	msKV.insert(make_pair(k,v));
	if(ofs.is_open())
	{
		ofs.close();
	}
	return IS_PROPERTIES_OK;
}

PSTATUS CProperties::modify(const char *k,const char* v){
	mulremark = false;
	bool notexit = true;
	//������ʱ�ļ���ʽ ��ʱ�ļ�����tmp_properties_modify
	ifstream ifs;
	ifs.open(this->path,ios::in);
	string temp="";
	if (!ifs)
	{
		return IS_PROPERTIES_ERROR;//���ļ�ʧ��
	}
	string sLine;
	char sStr[1024] = {};
	sprintf_s(sStr,1024,"%s=",k);
	char sModify[1024] = {};
	sprintf_s(sModify,1024,"%s=%s",k,v);
	while(!ifs.eof()){
		sLine = "";
		getline(ifs,sLine);
		if (mulremark)//�Ѵ򿪶���ע�Ϳ��� ���жϸ�����û�йرտ���
		{
			temp.append(sLine.c_str());
			temp.append("\n");
			if(sLine.find("*/") != string::npos){
				//ofstemp<<sLine.c_str()<<endl;
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
			if (sSubLine.length()<= 0) {temp.append(sLine.c_str());temp.append("\n");continue;}
			if (sSubLine[0] == '#'){temp.append(sLine.c_str());temp.append("\n");continue;}
			if (sSubLine[0] == '['){temp.append(sLine.c_str());temp.append("\n");continue;}
			if (sSubLine.length()>2 && sSubLine[0] == '/' && sSubLine[1] == '/'){temp.append(sLine.c_str());temp.append("\n");continue;}
			if (sSubLine.length()>4 && sSubLine[0] == '<' && sSubLine[1] == '!'){temp.append(sLine.c_str());temp.append("\n");continue;}

			if (sSubLine.find(sStr) == string::npos){temp.append(sLine.c_str());temp.append("\n");continue;}
			{
				notexit = false;
				temp.append(sModify);
				if (pos!=string::npos)
				{
					temp.append(sLine.substr(pos,sLine.length()));
				}
				temp.append("\n");
				continue;
			}
		}
	}
	if (notexit)
	{
		temp.append(sModify);
	}
	if (ifs.is_open())
	{
		ifs.close();
	}
	ofstream ofs;
	ofs.open(this->path.c_str(),ios::trunc);
	if(!ofs)
	{
		return IS_PROPERTIES_ERROR;//���ļ�ʧ��
	}
	ofs<<temp.c_str();
	if (ofs.is_open())
	{
		ofs.close();
	}
	msKV.erase(k);
	msKV.insert(make_pair(k,v));
	return IS_PROPERTIES_OK;
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



int main()
{
//########################################################//
//read
//1.��������open()�ļ�
//2.load()���ص��ڴ���
//3.read()��ȡ���keyֵ��value/write()׷��д��key=value/modify()�޸�k=v0ֵΪk=v
//4.close()�ͷ���Դ

CProperties cprop;
PSTATUS ret = cprop.open(".\\bank.properties");
if (ret != IS_PROPERTIES_OK)
{
	cout<<"�������ļ�ʧ��"<<endl;
	getchar();
	return 0;
}
cprop.load();
vector<string> vec = cprop.read("3");
for (int i=0; i<vec.size();i++)
{
	cout<<vec[i].c_str()<<endl;
}
cout<<"-----------------------"<<endl;
cprop.write("5","rtu");
cprop.write("5","25wsd");
vec = cprop.read("5");
for (int i=0; i<vec.size();i++)
{
	cout<<vec[i].c_str()<<endl;
}

cprop.modify("3","�й���Դ");
cout<<"-----------------------"<<endl;
vec = cprop.read("5");
for (int i=0; i<vec.size();i++)
{
	cout<<vec[i].c_str()<<endl;
}
cprop.close();
getchar();
}
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