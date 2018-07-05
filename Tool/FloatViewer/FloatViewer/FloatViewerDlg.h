
// FloatViewerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CFloatViewerDlg �Ի���
class CFloatViewerDlg : public CDialogEx
{
// ����
public:
	CFloatViewerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FLOATVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
