
// NmeaCRCCalcDlg.h : ͷ�ļ�
//

#pragma once


// CNmeaCRCCalcDlg �Ի���
class CNmeaCRCCalcDlg : public CDialogEx
{
// ����
public:
	CNmeaCRCCalcDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NMEACRCCALC_DIALOG };

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
	CString m_nmea_string;
	CString m_crc_value;
	afx_msg void OnBnClickedButtonCalc();
};
