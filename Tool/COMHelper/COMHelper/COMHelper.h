
// COMHelper.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCOMHelperApp:
// �йش����ʵ�֣������ COMHelper.cpp
//

class CCOMHelperApp : public CWinApp
{
public:
	CCOMHelperApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCOMHelperApp theApp;