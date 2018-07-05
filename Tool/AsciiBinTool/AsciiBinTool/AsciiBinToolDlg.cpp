
// AsciiBinToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AsciiBinTool.h"
#include "AsciiBinToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAsciiBinToolDlg �Ի���




CAsciiBinToolDlg::CAsciiBinToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAsciiBinToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsciiBinToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsciiBinToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ASCII_TO_BIN, &CAsciiBinToolDlg::OnBnClickedButtonAsciiToBin)
	ON_BN_CLICKED(IDC_BUTTON_BIN_TO_ASCII, &CAsciiBinToolDlg::OnBnClickedButtonBinToAscii)
END_MESSAGE_MAP()


// CAsciiBinToolDlg ��Ϣ�������

BOOL CAsciiBinToolDlg::OnInitDialog()
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

void CAsciiBinToolDlg::OnPaint()
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
HCURSOR CAsciiBinToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAsciiBinToolDlg::OnBnClickedButtonAsciiToBin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str_ascii;
	CString str_bin;
	int i, j, len;
	
	GetDlgItem(IDC_EDIT_ASCII)->GetWindowText(str_ascii);
	USES_CONVERSION;
	char* p = T2A(str_ascii);
	len = strlen(p);

	for (i = 0; i < len; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (*p & (0x1 << (7 - j)))
			{
				str_bin += _T("1");
			}
			else
			{
				str_bin += _T("0");
			}
		}
		//str_bin += _T(" ");
		p++;
	}
	GetDlgItem(IDC_EDIT_BIN)->SetWindowText(str_bin);
}


void CAsciiBinToolDlg::OnBnClickedButtonBinToAscii()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str_ascii;
	CString str_bin;
	CString str_tmp;
	int i, j, len;
	char tmp;
	GetDlgItem(IDC_EDIT_BIN)->GetWindowText(str_bin);

	USES_CONVERSION;
	char* p = T2A(str_bin);
	len = strlen(p);

	for (i = 0; i < (len / 8); i++)
	{
		tmp = 0;
		for (j = 0; j < 8; j++)
		{
			if (*p == '1')
			{
				tmp = (tmp << 1) | 1;
			}
			else
			{
				tmp = (tmp << 1) | 0;
			}
			p++;
		}
		str_tmp.Format(_T("%c"),tmp);
		str_ascii += str_tmp;
	}
	GetDlgItem(IDC_EDIT_ASCII)->SetWindowText(str_ascii);
}
