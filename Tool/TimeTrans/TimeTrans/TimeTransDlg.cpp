
// TimeTransDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimeTrans.h"
#include "TimeTransDlg.h"
#include "afxdialogex.h"

#include "GNSS_Time.h"

GPSUTCParameter gps_utc_para ={0};
BD2UTCParameter bd2_utc_para ={0};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeTransDlg 对话框




CTimeTransDlg::CTimeTransDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTransDlg::IDD, pParent)
	, m_gnss_mode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_GPS, m_gnss_mode);
}

BEGIN_MESSAGE_MAP(CTimeTransDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_GPS, &CTimeTransDlg::OnBnClickedRadioGps)
	ON_BN_CLICKED(IDC_RADIO_BD2, &CTimeTransDlg::OnBnClickedRadioBd2)
	ON_BN_CLICKED(IDC_BUTTON_TO_UTC, &CTimeTransDlg::OnBnClickedButtonToUtc)
	ON_BN_CLICKED(IDC_BUTTON_UTC_TO, &CTimeTransDlg::OnBnClickedButtonUtcTo)
END_MESSAGE_MAP()


// CTimeTransDlg 消息处理程序

BOOL CTimeTransDlg::OnInitDialog()
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

void CTimeTransDlg::OnPaint()
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
HCURSOR CTimeTransDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTimeTransDlg::OnBnClickedRadioGps()
{
	// TODO: 在此添加控件通知处理程序代码
	m_gnss_mode = 0;
}


void CTimeTransDlg::OnBnClickedRadioBd2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_gnss_mode = 1;
}


void CTimeTransDlg::OnBnClickedButtonToUtc()
{
	// TODO: 在此添加控件通知处理程序代码
	CString tmp;
	int wn, turn;
	float tow;
	UTCTime utc_time;

	GetDlgItem(IDC_EDIT_WN)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	wn = _ttoi(tmp);

	GetDlgItem(IDC_EDIT_TOW)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	tow = _ttof(tmp);

	GetDlgItem(IDC_EDIT_TURN)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	turn = _ttoi(tmp);

	if (m_gnss_mode == 0)
	{
		GPSConvertUTC(&gps_utc_para, wn, tow, 2, &utc_time, turn);
	}
	else
	{
		BD2ConvertUTC(&bd2_utc_para, wn, tow, 2, &utc_time);
	}

	tmp.Format(_T("%4d"), utc_time.year);
	GetDlgItem(IDC_EDIT_YEAR)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.mon);
	GetDlgItem(IDC_EDIT_MONTH)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.day);
	GetDlgItem(IDC_EDIT_DAY)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.hour);
	GetDlgItem(IDC_EDIT_HOUR)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.min);
	GetDlgItem(IDC_EDIT_MINUTE)->SetWindowText(tmp);
	tmp.Format(_T("%f"), utc_time.sec);
	GetDlgItem(IDC_EDIT_SECOND)->SetWindowText(tmp);
}


void CTimeTransDlg::OnBnClickedButtonUtcTo()
{
	// TODO: 在此添加控件通知处理程序代码
}
