
// AsciiBinTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAsciiBinToolApp:
// �йش����ʵ�֣������ AsciiBinTool.cpp
//

class CAsciiBinToolApp : public CWinApp
{
public:
	CAsciiBinToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAsciiBinToolApp theApp;