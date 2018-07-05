
// TimeTransDlg.h : 头文件
//

#pragma once


// CTimeTransDlg 对话框
class CTimeTransDlg : public CDialogEx
{
// 构造
public:
	CTimeTransDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TIMETRANS_DIALOG };

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
	int m_gnss_mode;
	afx_msg void OnBnClickedRadioGps();
	afx_msg void OnBnClickedRadioBd2();
	afx_msg void OnBnClickedButtonToUtc();
	afx_msg void OnBnClickedButtonUtcTo();
};
