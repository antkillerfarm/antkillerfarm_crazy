
// NmeaCRCCalcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NmeaCRCCalc.h"
#include "NmeaCRCCalcDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNmeaCRCCalcDlg 对话框




CNmeaCRCCalcDlg::CNmeaCRCCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNmeaCRCCalcDlg::IDD, pParent)
	, m_nmea_string(_T(""))
	, m_crc_value(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNmeaCRCCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nmea_string);
	DDX_Text(pDX, IDC_EDIT2, m_crc_value);
}

BEGIN_MESSAGE_MAP(CNmeaCRCCalcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CNmeaCRCCalcDlg::OnBnClickedButtonCalc)
END_MESSAGE_MAP()


// CNmeaCRCCalcDlg 消息处理程序

BOOL CNmeaCRCCalcDlg::OnInitDialog()
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

void CNmeaCRCCalcDlg::OnPaint()
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
HCURSOR CNmeaCRCCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

unsigned int xor_check(const char *p_chk_str)
{
	int check_sum = 0;

	while ((*p_chk_str) != '\0') {
		check_sum ^= (*p_chk_str);
		p_chk_str++;
	}

	return check_sum;
}

void CNmeaCRCCalcDlg::OnBnClickedButtonCalc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	USES_CONVERSION;
	char* p = T2A(m_nmea_string);
	unsigned int check_sum = xor_check(p);
	m_crc_value.Format(_T("%02X"),check_sum);
	UpdateData(FALSE);
}
