
// BigFileSplitterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CBigFileSplitterDlg �Ի���
class CBigFileSplitterDlg : public CDialogEx
{
// ����
public:
	CBigFileSplitterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BIGFILESPLITTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
