
// FloatViewerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CFloatViewerDlg 对话框
class CFloatViewerDlg : public CDialogEx
{
// 构造
public:
	CFloatViewerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FLOATVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_EndianType;
	afx_msg void OnBnClickedOk();
	CEdit m_Edit1;
	CEdit m_Edit2;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	int m_FormatType;
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
};
