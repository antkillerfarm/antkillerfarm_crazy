
// AsciiBinToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AsciiBinTool.h"
#include "AsciiBinToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAsciiBinToolDlg 对话框




CAsciiBinToolDlg::CAsciiBinToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAsciiBinToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsciiBinToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsciiBinToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ASCII_TO_BIN, &CAsciiBinToolDlg::OnBnClickedButtonAsciiToBin)
	ON_BN_CLICKED(IDC_BUTTON_BIN_TO_ASCII, &CAsciiBinToolDlg::OnBnClickedButtonBinToAscii)
END_MESSAGE_MAP()


// CAsciiBinToolDlg 消息处理程序

BOOL CAsciiBinToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAsciiBinToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAsciiBinToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAsciiBinToolDlg::OnBnClickedButtonAsciiToBin()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str_ascii;
	CString str_bin;
	int i, j, len;
	
	GetDlgItem(IDC_EDIT_ASCII)->GetWindowText(str_ascii);
	USES_CONVERSION;
	char* p = T2A(str_ascii);
	len = strlen(p);

	for (i = 0; i < len; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (*p & (0x1 << (7 - j)))
			{
				str_bin += _T("1");
			}
			else
			{
				str_bin += _T("0");
			}
		}
		//str_bin += _T(" ");
		p++;
	}
	GetDlgItem(IDC_EDIT_BIN)->SetWindowText(str_bin);
}


void CAsciiBinToolDlg::OnBnClickedButtonBinToAscii()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str_ascii;
	CString str_bin;
	CString str_tmp;
	int i, j, len;
	char tmp;
	GetDlgItem(IDC_EDIT_BIN)->GetWindowText(str_bin);

	USES_CONVERSION;
	char* p = T2A(str_bin);
	len = strlen(p);

	for (i = 0; i < (len / 8); i++)
	{
		tmp = 0;
		for (j = 0; j < 8; j++)
		{
			if (*p == '1')
			{
				tmp = (tmp << 1) | 1;
			}
			else
			{
				tmp = (tmp << 1) | 0;
			}
			p++;
		}
		str_tmp.Format(_T("%c"),tmp);
		str_ascii += str_tmp;
	}
	GetDlgItem(IDC_EDIT_ASCII)->SetWindowText(str_ascii);
}
