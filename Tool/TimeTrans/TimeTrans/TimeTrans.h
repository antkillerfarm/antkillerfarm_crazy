
// TimeTrans.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTimeTransApp:
// �йش����ʵ�֣������ TimeTrans.cpp
//

class CTimeTransApp : public CWinApp
{
public:
	CTimeTransApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTimeTransApp theApp;