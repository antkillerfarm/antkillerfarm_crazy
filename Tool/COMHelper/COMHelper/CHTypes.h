#include "stdafx.h"

#ifndef _CHTYPES_H_
#define _CHTYPES_H_

struct COMParameter
{
	CString Port;
	DWORD BaudRate;
	CString Parity;
	BYTE StopBits;
	BYTE DataBits;
};

#endif