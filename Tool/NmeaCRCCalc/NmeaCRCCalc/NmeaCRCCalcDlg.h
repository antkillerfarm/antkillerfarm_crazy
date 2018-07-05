
// NmeaCRCCalcDlg.h : 头文件
//

#pragma once


// CNmeaCRCCalcDlg 对话框
class CNmeaCRCCalcDlg : public CDialogEx
{
// 构造
public:
	CNmeaCRCCalcDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NMEACRCCALC_DIALOG };

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
	CString m_nmea_string;
	CString m_crc_value;
	afx_msg void OnBnClickedButtonCalc();
};
