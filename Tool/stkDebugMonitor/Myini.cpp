
//_________________________________Copyright (c) 2006,Flying Co.,Ltd______________________________
//                                       All rights reserved.
// 
//   文件名称：Myini.cpp
//   文件标识：见配置管理计划书
//   描    述：读写配置文件类
// 
//   当前版本：1.0
//   作    者：张小刚
//   完成日期：2007.02.18
//________________________________________________________________________________________________

#include <stdio.h>
#include <string.h>
#include "Myini.h"

/////////////////////////////////////////////////////////////////////
// *函数功能*
//  构造函数(初始化变量)
// *参数*   
//  无
// *返回值* 
//  无

CMyini::CMyini(void)
{
	m_pBigContent = new strBigContent[INITNUM_BIGCONTENT];
	memset(m_pBigContent, 0, sizeof(strBigContent)*INITNUM_BIGCONTENT);
	
	memset(m_cFileName, 0, sizeof(m_cFileName));
}

/////////////////////////////////////////////////////////////////////
// *函数功能*
//  析构函数
// *参数*   
//  无
// *返回值*
//  无

CMyini::~CMyini(void)
{
	if(m_pBigContent)
	{
		delete [] m_pBigContent;
		m_pBigContent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////
// *函数功能*
//  打开配置文件
// *参数*   
//  cFileName 文件名
// *返回值*
//  true    : 打开文件成功
//  false   : 打开文件失败

bool CMyini::Open(char *cFileName)
{
	FILE *pFile = fopen(cFileName, "r");
	if(!pFile)
	{
		return false;
	}

	strcpy(m_cFileName, cFileName);

	long  lDataLen;  // 读出来的配置文件长度
	long  lBigContentNum     = 0;  // []的数目
	long  m = 0;
	long  n = 0;
	bool  bHaveRemark = false;
	char cTemp[256];
	memset(cTemp, 0, sizeof(cTemp));

	char *cData = NULL;

	// 获得文件长度
	fseek(pFile, 0, SEEK_END);   
	lDataLen = ftell(pFile); 
	fseek(pFile, 0, SEEK_SET);

	// 读取原始数据	
	if(lDataLen > 0)
	{
		cData = new char[lDataLen];
		memset(cData, 0, lDataLen);
		lDataLen = fread(cData, 1, lDataLen, pFile);
	}
	
	// 获得[]的数目
	for(int i=0; i<lDataLen; i++)
	{			
		if(i > 0)
		{
			if(cData[i]=='[' && cData[i-1]=='\n')
			{
				lBigContentNum++;
			}
		}
		else
		{
			if(cData[i] == '[')
			{
				lBigContentNum++;
			}
		}
	}

    // 读取数据	
	for(int t=0; t<lBigContentNum; t++)
	{
		m_pBigContent[t].nSmallContentCnt = 0;
		// cTitle
		for(int j=n; j<lDataLen; j++)
		{							
			if(j > 0)
			{
				if(cData[j] == ']')
				{
					cTemp[m] = cData[j];
					
					n+=2;
					break;
				}
			}

			cTemp[m] = cData[j];
			m++;
			n++;
		}
		long f = 0;
		for(int e=0; e<(int)strlen(cTemp); e++)
		{
			if(cTemp[e]==' '|| cTemp[e]== '\n' || cTemp[e]== '\r')
			{
				continue;
			}

			m_pBigContent[t].cTitle[f] = cTemp[e];
			f++;
		}
		memset(cTemp, 0, sizeof(cTemp));
		m = 0;

		while(true)
		{
			if(cData[n]=='[' || n >= lDataLen)
			{	
				break;
			}

			// cName
			for(int j=n; j<lDataLen; j++)
			{				
				if(cData[j] == '=')
				{
					n++;
					break;
				}

				cTemp[m] = cData[j];
				m++;
				n++;
			}
			f = 0;
			for(int e=0; e<(int)strlen(cTemp); e++)
			{
				if(cTemp[e]==' '|| cTemp[e]== '\n' || cTemp[e]== '\r')
				{
					continue;
				}

				m_pBigContent[t].SmallContent[m_pBigContent[t].nSmallContentCnt].cName[f] = cTemp[e];
				f++;
			}
			memset(cTemp, 0, sizeof(cTemp));
			m = 0;

			// cContent
			for(int j=n; j<lDataLen; j++)
			{
				if(cData[j] == '/' && cData[j + 1] == '/') // 有注释
				{
					bHaveRemark = true;
					break;
				}

				if(cData[j] == ';') // 无注释
				{
					bHaveRemark = false;
					n+=2;
					break;
				}

				cTemp[m] = cData[j];
				m++;
				n++;
			}
			f = 0;
			for(int e=0; e<(int)strlen(cTemp); e++)
			{
				if(cTemp[e]==' '|| cTemp[e]== '\n' || cTemp[e]== '\r')
				{
					continue;
				}

				m_pBigContent[t].SmallContent[m_pBigContent[t].nSmallContentCnt].cContent[f] = cTemp[e];
				f++;
			}
			memset(cTemp, 0, sizeof(cTemp));
			m = 0;

			if(bHaveRemark)
			{
				// cRemark
				for(int j=n; j<lDataLen; j++)
				{
					if(cData[j] == ';')
					{
						n+=2;
						break;
					}

					cTemp[m] = cData[j];
					m++;
					n++;
				}
				strcpy(m_pBigContent[t].SmallContent[m_pBigContent[t].nSmallContentCnt].cRemark, cTemp);
				memset(cTemp, 0, sizeof(cTemp));
				m = 0;
			}
			m_pBigContent[t].nSmallContentCnt++;
		}
	}

	if(cData)
	{
		delete [] cData;
	}

	if(pFile)
	{
		fclose(pFile);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// *函数功能*
//  保存到配置文件中
// *参数*   
//  无
// *返回值*
//  无

bool CMyini::Save(void)
{
	FILE *pFile = fopen(m_cFileName, "w");
	if(!pFile)
	{
		return false;
	}

	char cTitleInfo[512];
	memset(cTitleInfo, 0, sizeof(cTitleInfo));

	char cContentInfo[512];
	memset(cContentInfo, 0, sizeof(cContentInfo));
	
	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strlen(m_pBigContent[i].cTitle) != 0) // 标题不为空
		{
			sprintf(cTitleInfo, "%s\n", 
				    m_pBigContent[i].cTitle
				    );
			fwrite(cTitleInfo, strlen(cTitleInfo), 1, pFile);

			for(int j=0; j<INITNUM_SMALLCONTENT; j++)
			{
				if(strlen(m_pBigContent[i].SmallContent[j].cName) != 0) // 名称不为空
				{
					if(strlen(m_pBigContent[i].SmallContent[j].cRemark) == 0) // 有注释
					{
						sprintf(cContentInfo, "%s%s%s%s\n", 
							    m_pBigContent[i].SmallContent[j].cName,			  
							    "=", 
							    m_pBigContent[i].SmallContent[j].cContent,
							    ";"
							    );	
					}
					else // 无注释
					{
						sprintf(cContentInfo, "%s%s%s%s%s%s\n", 
							    m_pBigContent[i].SmallContent[j].cName,			  
							    "=", 
							    m_pBigContent[i].SmallContent[j].cContent,
							    " ",
							    m_pBigContent[i].SmallContent[j].cRemark,
							    ";"
							    );		
					}					
					
					fwrite(cContentInfo, strlen(cContentInfo), 1, pFile);
				}
			}
		}
	}

	if(pFile)
	{
		fclose(pFile);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// *函数功能*
//  读取字符串
// *参数*   
//  cTitle  : 配置文件中的标题
//  cName   : 配置文件中的名称
//  cContent: 配置文件中的内容
// *返回值*
//  true    : 读成功
//  false   : 读失败

bool CMyini::Read(char *cTitle, char *cName,  char *cContent)
{
	if(!cTitle || !cName || !cContent)
	{
		return false;
	}

	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strcmp(m_pBigContent[i].cTitle, cTitle) == 0)
		{
			for(int j=0; j < m_pBigContent[i].nSmallContentCnt; j++)
			{
				if(strcmp(m_pBigContent[i].SmallContent[j].cName, cName) == 0) // 配置文件内容中有这个名称cName
				{
					strcpy(cContent, m_pBigContent[i].SmallContent[j].cContent);
					
					return true;
				}
			}
		}
	}

	return false;
}

bool CMyini::ReadFromIndex(char *cTitle, int nIndex,  char *cContent)
{
	if(!cTitle || !cContent)
	{
		return false;
	}

	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strcmp(m_pBigContent[i].cTitle, cTitle) == 0)
		{
			strcpy(cContent, m_pBigContent[i].SmallContent[nIndex].cContent);
			return true;
		}
	}

	return false;
}

bool CMyini::ReadFromTitle(char *cTitle, strBigContent** pp_strBigContent)
{
        *pp_strBigContent = NULL;
	if(!cTitle)
	{
		return false;
	}

	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strcmp(m_pBigContent[i].cTitle, cTitle) == 0)
		{
                        *pp_strBigContent = &(m_pBigContent[i]);
			return true;
		}
	}

	return false;
}
/////////////////////////////////////////////////////////////////////
// *函数功能*
//  写一个字符串
// *参数*   
//  cTitle  : 配置文件中的标题
//  cName   : 配置文件中的名称
//  cContent: 配置文件中的内容
// *返回值*
//  true    : 写成功
//  false   : 写失败

bool CMyini::Write(char *cTitle, char *cName, char *cContent)
{
	bool bHaveTitle          = false;
	bool bHaveName           = false;
	bool bHaveBigFreeSpace   = false;
	bool bHaveSmallFreeSpace = false;

	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strcmp(m_pBigContent[i].cTitle, cTitle) == 0) // 配置文件内容中有这个标题cTitle 
		{
			for(int j=0; j < m_pBigContent[i].nSmallContentCnt; j++)
			{
				if(strcmp(m_pBigContent[i].SmallContent[j].cName, cName) == 0) // 配置文件内容中有这个名称cName 
				{
					bHaveName = true;
					strcpy(m_pBigContent[i].SmallContent[j].cContent, cContent);
					break;
				}
			}

			if(!bHaveName) // 配置文件内容中没有这个名称cName 
			{
				for(int k=0; k < m_pBigContent[i].nSmallContentCnt; k++)
				{		
					if(strlen(m_pBigContent[i].SmallContent[k].cName) == 0) // 有空闲地方放置配置内容
					{	
						bHaveSmallFreeSpace = true;
						strcpy(m_pBigContent[i].SmallContent[k].cName, cName);
						strcpy(m_pBigContent[i].SmallContent[k].cContent, cContent);
						break;
					}
				}

				if(!bHaveSmallFreeSpace) // 配置文件小内容中没有足够的空间 
				{
					return false;
				}
			}

			return true;
		}
	}

	if(!bHaveTitle) // 配置文件内容中没有这个标题cTitle  
	{		
		for(int m=0; m<INITNUM_BIGCONTENT; m++)
		{
			if(strlen(m_pBigContent[m].cTitle) == 0) // 有空闲地方放置配置内容
			{	
				bHaveBigFreeSpace = true;
				strcpy(m_pBigContent[m].cTitle, cTitle);
				strcpy(m_pBigContent[m].SmallContent[0].cName, cName);
				strcpy(m_pBigContent[m].SmallContent[0].cContent, cContent); 
				break;
			}
		}

		if(!bHaveBigFreeSpace) // 配置文件大内容中没有足够的空间 
		{			
			return false;
		}

		return true;
	}

	return false;
}

//______________________________________________文件结束__________________________________________
