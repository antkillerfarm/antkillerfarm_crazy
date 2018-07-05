// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "COMHelper.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "COMHelperDlg.h"

// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
END_MESSAGE_MAP()


// CSettingDlg 消息处理程序


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString s;
	GetDlgItem(IDC_COMBO_PORT)->SetWindowText(cp.Port);
	s.Format(_T("%d"),cp.BaudRate);
	GetDlgItem(IDC_COMBO_BAUD)->SetWindowText(s);
	s.Format(_T("%d"),cp.DataBits);
	GetDlgItem(IDC_COMBO_DATA_BITS)->SetWindowText(s);
	s.Format(_T("%d"),cp.StopBits);
	GetDlgItem(IDC_COMBO_STOP_BITS)->SetWindowText(s);
	GetDlgItem(IDC_COMBO_PARITY)->SetWindowText(cp.Parity);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSettingDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	TCHAR tmp[MAX_PATH];
	USES_CONVERSION;

	GetDlgItem(IDC_COMBO_PORT)->GetWindowText(tmp, MAX_PATH);
	LPSTR atmp = T2A(tmp);
	myini.Write("[COMSETTING]","port",atmp);
	cp.Port = tmp;

	GetDlgItem(IDC_COMBO_BAUD)->GetWindowText(tmp, MAX_PATH);
	atmp = T2A(tmp);
	myini.Write("[COMSETTING]","baud rate",atmp);
	cp.BaudRate = atoi(atmp);

	GetDlgItem(IDC_COMBO_DATA_BITS)->GetWindowText(tmp, MAX_PATH);
	atmp = T2A(tmp);
	myini.Write("[COMSETTING]","data bits",atmp);
	cp.DataBits = atoi(atmp);

	GetDlgItem(IDC_COMBO_STOP_BITS)->GetWindowText(tmp, MAX_PATH);
	atmp = T2A(tmp);
	myini.Write("[COMSETTING]","stop bits",atmp);
	cp.StopBits = atoi(atmp);

	GetDlgItem(IDC_COMBO_PARITY)->GetWindowText(tmp, MAX_PATH);
	atmp = T2A(tmp);
	myini.Write("[COMSETTING]","parity",atmp);
	cp.Parity = tmp;

	CDialogEx::OnOK();
}
