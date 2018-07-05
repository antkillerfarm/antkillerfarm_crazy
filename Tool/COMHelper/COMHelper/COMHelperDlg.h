
// COMHelperDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "CHTypes.h"
#include "Myini.h"

extern COMParameter cp;
extern CMyini myini;
extern HANDLE hCom;
// CCOMHelperDlg 对话框
class CCOMHelperDlg : public CDialogEx
{
// 构造
public:
	CCOMHelperDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CCOMHelperDlg();

// 对话框数据
	enum { IDD = IDD_COMHELPER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CEdit m_Edit_Path;
	CListBox m_List_TestCase;
	CEdit m_Edit_Receive;
	bool m_IsConnected;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedSetting();
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedSave();
	afx_msg void OnEnChangePathEdit();
	afx_msg void OnBnClickedConnect();
	void ReadCOM(void);
};
