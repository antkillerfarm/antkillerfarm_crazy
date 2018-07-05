
// BigFileSplitterDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CBigFileSplitterDlg 对话框
class CBigFileSplitterDlg : public CDialogEx
{
// 构造
public:
	CBigFileSplitterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BIGFILESPLITTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFileSelect();
	afx_msg void OnBnClickedButtonSplit();
	void SplitFile(char* _file_name, int _file_size);
public:
	CString m_file_name;
	CString m_file_size;
	CProgressCtrl m_progress;
	CButton m_btn_split;
};
