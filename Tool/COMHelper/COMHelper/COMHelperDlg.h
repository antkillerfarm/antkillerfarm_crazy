
// COMHelperDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "CHTypes.h"
#include "Myini.h"

extern COMParameter cp;
extern CMyini myini;
extern HANDLE hCom;
// CCOMHelperDlg �Ի���
class CCOMHelperDlg : public CDialogEx
{
// ����
public:
	CCOMHelperDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CCOMHelperDlg();

// �Ի�������
	enum { IDD = IDD_COMHELPER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CEdit m_Edit_Path;
	CListBox m_List_TestCase;
	CEdit m_Edit_Receive;
	bool m_IsConnected;

	// ���ɵ���Ϣӳ�亯��
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
