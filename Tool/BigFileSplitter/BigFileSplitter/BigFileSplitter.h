
// BigFileSplitter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBigFileSplitterApp:
// �йش����ʵ�֣������ BigFileSplitter.cpp
//

class CBigFileSplitterApp : public CWinApp
{
public:
	CBigFileSplitterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBigFileSplitterApp theApp;