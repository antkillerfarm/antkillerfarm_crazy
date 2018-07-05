
//_________________________________Copyright (c) 2006,Flying Co.,Ltd______________________________
//                                       All rights reserved.
// 
//   �ļ����ƣ�Myini.h
//   �ļ���ʶ�������ù���ƻ���
//   ��    ������д�����ļ���
// 
//   ��ǰ�汾��1.0
//   ��    �ߣ���С��
//   ������ڣ�2007.02.18
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

// �����ļ���д��ʽ
// �ļ�����
/*
[ONE]
name=�Ŵ��� //����;

 heigh = 200 // ���;
  weight=80 // ����;
sex=�� // �Ա�;
[TWO]
name=���� //����;
heigh=185 // ���;
weight=75 // ����;
sex=�� // �Ա�;
[THREE]
name=��;
heigh=165;
weight=54;
sex=��;
[FOUR]
name=200;
heigh=180;
[FIVE]
heigh=200 //���;
*/
// 1.ÿ�������ԷֺŽ���  
// 2.ע�ͱ�����//��ͷ,�ԷֺŽ���
// 3.[]ǰ�����пո�ͻس�
// 4.��������������д

class CMyini
{
public:
	CMyini(void);
	~CMyini(void);

public:	
	// ����ӿ�
	bool Open(char *cFileName);
	bool Save(void);              
    
	bool Read(char *cTitle, char *cName, char *cContent);
	bool ReadFromIndex(char *cTitle, int nIndex,  char *cContent);
	bool ReadFromTitle(char *cTitle, strBigContent** pp_strBigContent);
	bool Write(char *cTitle, char *cName, char *cContent);   
	
private:
	strBigContent *m_pBigContent;     // �����ļ�����
	char           m_cFileName[256];  // �����ļ�������
};

#endif
//______________________________________________�ļ�����__________________________________________