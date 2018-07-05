
//_________________________________Copyright (c) 2006,Flying Co.,Ltd______________________________
//                                       All rights reserved.
// 
//   �ļ����ƣ�Myini.cpp
//   �ļ���ʶ�������ù���ƻ���
//   ��    ������д�����ļ���
// 
//   ��ǰ�汾��1.0
//   ��    �ߣ���С��
//   ������ڣ�2007.02.18
//________________________________________________________________________________________________

#include <stdio.h>
#include <string.h>
#include "Myini.h"

/////////////////////////////////////////////////////////////////////
// *��������*
//  ���캯��(��ʼ������)
// *����*   
//  ��
// *����ֵ* 
//  ��

CMyini::CMyini(void)
{
	m_pBigContent = new strBigContent[INITNUM_BIGCONTENT];
	memset(m_pBigContent, 0, sizeof(strBigContent)*INITNUM_BIGCONTENT);
	
	memset(m_cFileName, 0, sizeof(m_cFileName));
}

/////////////////////////////////////////////////////////////////////
// *��������*
//  ��������
// *����*   
//  ��
// *����ֵ*
//  ��

CMyini::~CMyini(void)
{
	if(m_pBigContent)
	{
		delete [] m_pBigContent;
		m_pBigContent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////
// *��������*
//  �������ļ�
// *����*   
//  cFileName �ļ���
// *����ֵ*
//  true    : ���ļ��ɹ�
//  false   : ���ļ�ʧ��

bool CMyini::Open(char *cFileName)
{
	FILE *pFile = fopen(cFileName, "r");
	if(!pFile)
	{
		return false;
	}

	strcpy(m_cFileName, cFileName);

	long  lDataLen;  // �������������ļ�����
	long  lBigContentNum     = 0;  // []����Ŀ
	long  m = 0;
	long  n = 0;
	bool  bHaveRemark = false;
	char cTemp[256];
	memset(cTemp, 0, sizeof(cTemp));

	char *cData = NULL;

	// ����ļ�����
	fseek(pFile, 0, SEEK_END);   
	lDataLen = ftell(pFile); 
	fseek(pFile, 0, SEEK_SET);

	// ��ȡԭʼ����	
	if(lDataLen > 0)
	{
		cData = new char[lDataLen];
		memset(cData, 0, lDataLen);
		lDataLen = fread(cData, 1, lDataLen, pFile);
	}
	
	// ���[]����Ŀ
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

    // ��ȡ����	
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
				if(cData[j] == '/' && cData[j + 1] == '/') // ��ע��
				{
					bHaveRemark = true;
					break;
				}

				if(cData[j] == ';') // ��ע��
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
// *��������*
//  ���浽�����ļ���
// *����*   
//  ��
// *����ֵ*
//  ��

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
		if(strlen(m_pBigContent[i].cTitle) != 0) // ���ⲻΪ��
		{
			sprintf(cTitleInfo, "%s\n", 
				    m_pBigContent[i].cTitle
				    );
			fwrite(cTitleInfo, strlen(cTitleInfo), 1, pFile);

			for(int j=0; j<INITNUM_SMALLCONTENT; j++)
			{
				if(strlen(m_pBigContent[i].SmallContent[j].cName) != 0) // ���Ʋ�Ϊ��
				{
					if(strlen(m_pBigContent[i].SmallContent[j].cRemark) == 0) // ��ע��
					{
						sprintf(cContentInfo, "%s%s%s%s\n", 
							    m_pBigContent[i].SmallContent[j].cName,			  
							    "=", 
							    m_pBigContent[i].SmallContent[j].cContent,
							    ";"
							    );	
					}
					else // ��ע��
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
// *��������*
//  ��ȡ�ַ���
// *����*   
//  cTitle  : �����ļ��еı���
//  cName   : �����ļ��е�����
//  cContent: �����ļ��е�����
// *����ֵ*
//  true    : ���ɹ�
//  false   : ��ʧ��

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
				if(strcmp(m_pBigContent[i].SmallContent[j].cName, cName) == 0) // �����ļ����������������cName
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
// *��������*
//  дһ���ַ���
// *����*   
//  cTitle  : �����ļ��еı���
//  cName   : �����ļ��е�����
//  cContent: �����ļ��е�����
// *����ֵ*
//  true    : д�ɹ�
//  false   : дʧ��

bool CMyini::Write(char *cTitle, char *cName, char *cContent)
{
	bool bHaveTitle          = false;
	bool bHaveName           = false;
	bool bHaveBigFreeSpace   = false;
	bool bHaveSmallFreeSpace = false;

	for(int i=0; i<INITNUM_BIGCONTENT; i++)
	{
		if(strcmp(m_pBigContent[i].cTitle, cTitle) == 0) // �����ļ����������������cTitle 
		{
			for(int j=0; j < m_pBigContent[i].nSmallContentCnt; j++)
			{
				if(strcmp(m_pBigContent[i].SmallContent[j].cName, cName) == 0) // �����ļ����������������cName 
				{
					bHaveName = true;
					strcpy(m_pBigContent[i].SmallContent[j].cContent, cContent);
					break;
				}
			}

			if(!bHaveName) // �����ļ�������û���������cName 
			{
				for(int k=0; k < m_pBigContent[i].nSmallContentCnt; k++)
				{		
					if(strlen(m_pBigContent[i].SmallContent[k].cName) == 0) // �п��еط�������������
					{	
						bHaveSmallFreeSpace = true;
						strcpy(m_pBigContent[i].SmallContent[k].cName, cName);
						strcpy(m_pBigContent[i].SmallContent[k].cContent, cContent);
						break;
					}
				}

				if(!bHaveSmallFreeSpace) // �����ļ�С������û���㹻�Ŀռ� 
				{
					return false;
				}
			}

			return true;
		}
	}

	if(!bHaveTitle) // �����ļ�������û���������cTitle  
	{		
		for(int m=0; m<INITNUM_BIGCONTENT; m++)
		{
			if(strlen(m_pBigContent[m].cTitle) == 0) // �п��еط�������������
			{	
				bHaveBigFreeSpace = true;
				strcpy(m_pBigContent[m].cTitle, cTitle);
				strcpy(m_pBigContent[m].SmallContent[0].cName, cName);
				strcpy(m_pBigContent[m].SmallContent[0].cContent, cContent); 
				break;
			}
		}

		if(!bHaveBigFreeSpace) // �����ļ���������û���㹻�Ŀռ� 
		{			
			return false;
		}

		return true;
	}

	return false;
}

//______________________________________________�ļ�����__________________________________________
