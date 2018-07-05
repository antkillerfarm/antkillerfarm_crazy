
//_________________________________Copyright (c) 2006,Flying Co.,Ltd______________________________
//                                       All rights reserved.
// 
//   文件名称：Myini.h
//   文件标识：见配置管理计划书
//   描    述：读写配置文件类
// 
//   当前版本：1.0
//   作    者：张小刚
//   完成日期：2007.02.18
//________________________________________________________________________________________________

#ifndef	__MYINI__H
#define __MYINI__H

#define INITNUM_SMALLCONTENT 16
#define INITNUM_BIGCONTENT   64

struct strSmallContent
{
	char cName[60];
	char cContent[60];
	char cRemark[30];
};

struct strBigContent
{
	char cTitle[40];
	int nSmallContentCnt;
	strSmallContent SmallContent[INITNUM_SMALLCONTENT];
};

// 配置文件书写格式
// 文件例子
/*
[ONE]
name=张大人 //姓名;

 heigh = 200 // 身高;
  weight=80 // 体重;
sex=男 // 性别;
[TWO]
name=张子 //姓名;
heigh=185 // 身高;
weight=75 // 体重;
sex=男 // 性别;
[THREE]
name=张;
heigh=165;
weight=54;
sex=男;
[FOUR]
name=200;
heigh=180;
[FIVE]
heigh=200 //身高;
*/
// 1.每项内容以分号结束  
// 2.注释必须以//开头,以分号结束
// 3.[]前不能有空格和回车
// 4.其他可以随意书写

class CMyini
{
public:
	CMyini(void);
	~CMyini(void);

public:	
	// 对外接口
	bool Open(char *cFileName);
	bool Save(void);              
    
	bool Read(char *cTitle, char *cName, char *cContent);
	bool ReadFromIndex(char *cTitle, int nIndex,  char *cContent);
	bool ReadFromTitle(char *cTitle, strBigContent** pp_strBigContent);
	bool Write(char *cTitle, char *cName, char *cContent);   
	
private:
	strBigContent *m_pBigContent;     // 配置文件内容
	char           m_cFileName[256];  // 配置文件的名称
};

#endif
//______________________________________________文件结束__________________________________________