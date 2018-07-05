
// FloatViewerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FloatViewer.h"
#include "FloatViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFloatViewerDlg �Ի���




CFloatViewerDlg::CFloatViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFloatViewerDlg::IDD, pParent)
	, m_EndianType(0)
	, m_FormatType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFloatViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_EndianType);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Radio(pDX, IDC_RADIO3, m_FormatType);
}

BEGIN_MESSAGE_MAP(CFloatViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CFloatViewerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO1, &CFloatViewerDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFloatViewerDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CFloatViewerDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CFloatViewerDlg::OnBnClickedRadio4)
END_MESSAGE_MAP()


// CFloatViewerDlg ��Ϣ�������

BOOL CFloatViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFloatViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFloatViewerDlg::OnPaint()
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
HCURSOR CFloatViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFloatViewerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString s;
	int i;
	double f;
	unsigned char *p = (unsigned char *)(&f);
	unsigned char c;

	m_Edit1.GetWindowText(s);
	if (m_FormatType==0)
	{
		s.MakeUpper();
		USES_CONVERSION;
		LPSTR tmp = T2A(s);

		for (i = 0; i<8; i++)
		{
			if (tmp[i*2]>='0' && tmp[i*2]<='9')
			{
				c = tmp[i*2]-'0';
			}
			else
			{
				c = tmp[i*2]-'A'+10;
			}
			c = c<<4;
			if (tmp[i*2+1]>='0' && tmp[i*2+1]<='9')
			{
				c += tmp[i*2+1]-'0';
			}
			else
			{
				c += tmp[i*2+1]-'A'+10;
			}
			if (m_EndianType)
			{
				p[i] = c;
			}
			else
			{
				p[7-i] = c;
			}
		}

		s.Format(_T("%.19g"),f);
	}
	else
	{
		f = _ttof(s);
		s = "";
		CString s1;
		if (m_EndianType)
		{
			for (i = 0; i<8; i++)
			{
				if (i==4)
				{
					s += " ";
				}
				s1.Format(_T("%02X"),p[i]);
				s += s1;
			}
		}
		else
		{
			for (i = 0; i<8; i++)
			{
				if (i==4)
				{
					s += " ";
				}
				s1.Format(_T("%02X"),p[7-i]);
				s += s1;
			}
		}
	}
	m_Edit2.SetWindowText(s);
}


void CFloatViewerDlg::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_EndianType = 0;
}




void CFloatViewerDlg::OnBnClickedRadio2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_EndianType = 1;
}


void CFloatViewerDlg::OnBnClickedRadio3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_FormatType = 0;
}


void CFloatViewerDlg::OnBnClickedRadio4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_FormatType = 1;
}
