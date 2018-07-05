
// NmeaCRCCalcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NmeaCRCCalc.h"
#include "NmeaCRCCalcDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNmeaCRCCalcDlg �Ի���




CNmeaCRCCalcDlg::CNmeaCRCCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNmeaCRCCalcDlg::IDD, pParent)
	, m_nmea_string(_T(""))
	, m_crc_value(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNmeaCRCCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nmea_string);
	DDX_Text(pDX, IDC_EDIT2, m_crc_value);
}

BEGIN_MESSAGE_MAP(CNmeaCRCCalcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CNmeaCRCCalcDlg::OnBnClickedButtonCalc)
END_MESSAGE_MAP()


// CNmeaCRCCalcDlg ��Ϣ�������

BOOL CNmeaCRCCalcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNmeaCRCCalcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNmeaCRCCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

unsigned int xor_check(const char *p_chk_str)
{
	int check_sum = 0;

	while ((*p_chk_str) != '\0') {
		check_sum ^= (*p_chk_str);
		p_chk_str++;
	}

	return check_sum;
}

void CNmeaCRCCalcDlg::OnBnClickedButtonCalc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	USES_CONVERSION;
	char* p = T2A(m_nmea_string);
	unsigned int check_sum = xor_check(p);
	m_crc_value.Format(_T("%02X"),check_sum);
	UpdateData(FALSE);
}
