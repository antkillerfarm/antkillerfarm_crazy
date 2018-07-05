
// TimeTransDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TimeTrans.h"
#include "TimeTransDlg.h"
#include "afxdialogex.h"

#include "GNSS_Time.h"

GPSUTCParameter gps_utc_para ={0};
BD2UTCParameter bd2_utc_para ={0};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeTransDlg �Ի���




CTimeTransDlg::CTimeTransDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTransDlg::IDD, pParent)
	, m_gnss_mode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_GPS, m_gnss_mode);
}

BEGIN_MESSAGE_MAP(CTimeTransDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_GPS, &CTimeTransDlg::OnBnClickedRadioGps)
	ON_BN_CLICKED(IDC_RADIO_BD2, &CTimeTransDlg::OnBnClickedRadioBd2)
	ON_BN_CLICKED(IDC_BUTTON_TO_UTC, &CTimeTransDlg::OnBnClickedButtonToUtc)
	ON_BN_CLICKED(IDC_BUTTON_UTC_TO, &CTimeTransDlg::OnBnClickedButtonUtcTo)
END_MESSAGE_MAP()


// CTimeTransDlg ��Ϣ�������

BOOL CTimeTransDlg::OnInitDialog()
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

void CTimeTransDlg::OnPaint()
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
HCURSOR CTimeTransDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTimeTransDlg::OnBnClickedRadioGps()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_gnss_mode = 0;
}


void CTimeTransDlg::OnBnClickedRadioBd2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_gnss_mode = 1;
}


void CTimeTransDlg::OnBnClickedButtonToUtc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString tmp;
	int wn, turn;
	float tow;
	UTCTime utc_time;

	GetDlgItem(IDC_EDIT_WN)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	wn = _ttoi(tmp);

	GetDlgItem(IDC_EDIT_TOW)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	tow = _ttof(tmp);

	GetDlgItem(IDC_EDIT_TURN)->GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		return;
	}
	turn = _ttoi(tmp);

	if (m_gnss_mode == 0)
	{
		GPSConvertUTC(&gps_utc_para, wn, tow, 2, &utc_time, turn);
	}
	else
	{
		BD2ConvertUTC(&bd2_utc_para, wn, tow, 2, &utc_time);
	}

	tmp.Format(_T("%4d"), utc_time.year);
	GetDlgItem(IDC_EDIT_YEAR)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.mon);
	GetDlgItem(IDC_EDIT_MONTH)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.day);
	GetDlgItem(IDC_EDIT_DAY)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.hour);
	GetDlgItem(IDC_EDIT_HOUR)->SetWindowText(tmp);
	tmp.Format(_T("%2d"), utc_time.min);
	GetDlgItem(IDC_EDIT_MINUTE)->SetWindowText(tmp);
	tmp.Format(_T("%f"), utc_time.sec);
	GetDlgItem(IDC_EDIT_SECOND)->SetWindowText(tmp);
}


void CTimeTransDlg::OnBnClickedButtonUtcTo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
