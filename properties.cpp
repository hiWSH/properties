#include "properties.h"
/************************************************************************************************/
///*
//*作者：王邵华
//*email：loveofprogram@sina.com
//*时间：20180404
//*版本历程：
//*版本号    参与人      事项    
//*V1.0.0.1  王邵华      创建  
//***************************************************************************************************
//***************************************************************************************************
//*本工具类实现c++对properties文件的读写操作，
//*1.支持单行注释如：行首除去空格后以 //、#、<!--内容--> 开始的
//* //这是单行注释
//* #这是单行注释
//* <!--这是单行注释-->
//*2.支持节点 但节点并没有意义 [section]
//*[section0]
//*key=value
//*[section1]
//*key=value
//*用到multimap所以存在key键有2个值而已，节点并没有起到区分的作用，这一点也符合properties配置文件要求规范的
//*3.支持多行注释 /*第一行，第二行.....*/
//*案例（1）同行有/*..*/ 如：key=123/*oooooo*/b=890  结果保留key=123
//*案例（2）多行有/*..*/ 如：key=123/*ooo
//*0000000000000000000
//*ooo*/b=890  结果保留key=123
//*案例（3）行头行尾有/*..*/如：/*ooooo*/key=123 结果不保留
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
		return IS_PROPERTIES_ERROR;//打开文件失败
	}
	string sLine;
	while(!ifs.eof()){
		sLine = "";
		getline(ifs,sLine);
		if (mulremark)//已打开多行注释开关 需判断该行有没有关闭开关
		{
			if(sLine.find("*/") != string::npos){
				mulremark = false;
			}
			continue;//无论开关是否关闭 继续读下一行数据
		}else
		{
			string::size_type pos =sLine.find("/*");string sSubLine;
			if (pos != string::npos)//改行有多行注释开关  需打开
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
//实现文件加载到map中
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
*aim:将key=value追加到文本末尾，更新multimap映射插入
*/
PSTATUS CProperties::write(const char *k,const char* v){
	if (nullptr == k || nullptr == v)
	{
		return IS_PROPERTIES_ERROR; //校验入参
	}
	ofstream ofs;
	ofs.open(this->path.c_str(),ios::app);
	if(!ofs)
	{
		return IS_PROPERTIES_ERROR;//打开文件失败
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
	//创建临时文件方式 临时文件命名tmp_properties_modify
	ifstream ifs;
	ifs.open(this->path,ios::in);
	string temp="";
	if (!ifs)
	{
		return IS_PROPERTIES_ERROR;//打开文件失败
	}
	string sLine;
	char sStr[1024] = {};
	sprintf_s(sStr,1024,"%s=",k);
	char sModify[1024] = {};
	sprintf_s(sModify,1024,"%s=%s",k,v);
	while(!ifs.eof()){
		sLine = "";
		getline(ifs,sLine);
		if (mulremark)//已打开多行注释开关 需判断该行有没有关闭开关
		{
			temp.append(sLine.c_str());
			temp.append("\n");
			if(sLine.find("*/") != string::npos){
				//ofstemp<<sLine.c_str()<<endl;
				mulremark = false;
			}
			continue;//无论开关是否关闭 继续读下一行数据
		}else
		{
			string::size_type pos =sLine.find("/*");string sSubLine;
			if (pos != string::npos)//改行有多行注释开关  需打开
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
		return IS_PROPERTIES_ERROR;//打开文件失败
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
*aim:防止多次造作脏数据干扰
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
//1.创建对象open()文件
//2.load()加载到内存中
//3.read()读取相关key值的value/write()追加写入key=value/modify()修改k=v0值为k=v
//4.close()释放资源

CProperties cprop;
PSTATUS ret = cprop.open(".\\bank.properties");
if (ret != IS_PROPERTIES_OK)
{
	cout<<"打开配置文件失败"<<endl;
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

cprop.modify("3","中国开源");
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
//1.创建对象
//2.load()加载到内存中
//3.write()读取相关key值的value
//4.close()释放资源
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