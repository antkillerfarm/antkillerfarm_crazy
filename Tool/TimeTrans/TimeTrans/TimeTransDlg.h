
// TimeTransDlg.h : ͷ�ļ�
//

#pragma once


// CTimeTransDlg �Ի���
class CTimeTransDlg : public CDialogEx
{
// ����
public:
	CTimeTransDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TIMETRANS_DIALOG };

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
	int m_gnss_mode;
	afx_msg void OnBnClickedRadioGps();
	afx_msg void OnBnClickedRadioBd2();
	afx_msg void OnBnClickedButtonToUtc();
	afx_msg void OnBnClickedButtonUtcTo();
};
