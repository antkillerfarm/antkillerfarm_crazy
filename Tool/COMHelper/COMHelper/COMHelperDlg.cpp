
// COMHelperDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "COMHelper.h"
#include "COMHelperDlg.h"
#include "afxdialogex.h"
#include "SettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COMParameter cp;
CMyini myini;
HANDLE hCom;

DWORD WINAPI ReadSerialPort(LPVOID lpParameter);
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


// CCOMHelperDlg �Ի���

CCOMHelperDlg::CCOMHelperDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCOMHelperDlg::IDD, pParent)
	, m_IsConnected(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCOMHelperDlg::~CCOMHelperDlg()
{
	myini.Save();
}

void CCOMHelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_Edit_Path);
	DDX_Control(pDX, IDC_LIST_TEST_CASE, m_List_TestCase);
	DDX_Control(pDX, IDC_EDIT_RECEIVE, m_Edit_Receive);
}

BEGIN_MESSAGE_MAP(CCOMHelperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, &CCOMHelperDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_SETTING, &CCOMHelperDlg::OnBnClickedSetting)
	ON_BN_CLICKED(IDC_SEND, &CCOMHelperDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_SAVE, &CCOMHelperDlg::OnBnClickedSave)
	ON_EN_CHANGE(IDC_EDIT_PATH, &CCOMHelperDlg::OnEnChangePathEdit)
	ON_BN_CLICKED(IDC_CONNECT, &CCOMHelperDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


// CCOMHelperDlg ��Ϣ�������

BOOL CCOMHelperDlg::OnInitDialog()
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
	char tmp[MAX_PATH];
	ZeroMemory(tmp, MAX_PATH);

	FILE* f;
	f = fopen("COMHelper.ini","r");
	if (!f)
	{
		f = fopen("COMHelper.ini","w");
	}
	fclose(f);

	myini.Open("COMHelper.ini");
	if (myini.Read("[SETTING]","dir",tmp))
	{
		USES_CONVERSION;
		LPTSTR path = A2T(tmp);
		m_Edit_Path.SetWindowText(path);
	}

	if (myini.Read("[COMSETTING]","port",tmp))
	{
		cp.Port = tmp;
	}
	else
	{
		cp.Port = _T("COM1");
	}

	if (myini.Read("[COMSETTING]","baud rate",tmp))
	{
		cp.BaudRate = atoi(tmp);
	}
	else
	{
		cp.BaudRate = 115200;
	}

	if (myini.Read("[COMSETTING]","data bits",tmp))
	{
		cp.DataBits = atoi(tmp);
	}
	else
	{
		cp.DataBits = 8;
	}

	if (myini.Read("[COMSETTING]","stop bits",tmp))
	{
		cp.StopBits = atoi(tmp);
	}
	else
	{
		cp.StopBits = 1;
	}

	if (myini.Read("[COMSETTING]","parity",tmp))
	{
		cp.Parity = tmp;
	}
	else
	{
		cp.Parity = _T("None");
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCOMHelperDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCOMHelperDlg::OnPaint()
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
HCURSOR CCOMHelperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCOMHelperDlg::OnBnClickedBrowse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	CString title;
	TCHAR path[MAX_PATH];
	title = _T("Select a folder for test cases");
	bi.hwndOwner = GetSafeHwnd();
	bi.lpszTitle = title.GetString();
	SHGetPathFromIDList(SHBrowseForFolder(&bi), path);
	m_Edit_Path.SetWindowText(path);
}


void CCOMHelperDlg::OnBnClickedSetting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSettingDlg dlg;
	dlg.DoModal();
}


void CCOMHelperDlg::OnBnClickedSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str,str1;
	int nIndex = m_List_TestCase.GetCurSel();
	TCHAR path[MAX_PATH];
	BYTE szBuffer[1024];
	DWORD dwBytesRead = 0;
	BOOL bResult;
	SYSTEMTIME st;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	if (nIndex != LB_ERR)
	{
		int n = m_List_TestCase.GetTextLen(nIndex);
		m_List_TestCase.GetText(nIndex, str.GetBuffer(n));
		str.ReleaseBuffer();

		m_Edit_Path.GetWindowText(path, MAX_PATH);
		int len = _tcslen(path);
		if (path[len-1] == _T('\\'))
		{
			str1 = path;
			str = str1 + str;
		}
		else
		{
			str1 = path;
			str = str1 + _T("\\") + str;
		}

		USES_CONVERSION;
		FILE* f;
		LPSTR apath = T2A(str);
		f = fopen(apath, "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);   
			int lDataLen = ftell(f); 
			fseek(f, 0, SEEK_SET); 
			fread(szBuffer,1,lDataLen,f);
			fclose(f);

			OVERLAPPED m_osWrite;
			memset(&m_osWrite,0,sizeof(OVERLAPPED));
			m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

			ClearCommError(hCom,&dwErrorFlags,&ComStat);
			//PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

			bResult = WriteFile(hCom, szBuffer, lDataLen, &dwBytesRead, &m_osWrite);
			if (!bResult)
			{
				if(GetLastError()==ERROR_IO_PENDING)
				{
					WaitForSingleObject(m_osWrite.hEvent,1000);
					if(!GetOverlappedResult(hCom,&m_osWrite,&dwBytesRead,TRUE))
					{
						dwBytesRead = 0;
					}
				}
				else
				{
					dwBytesRead = 0;
				}
			}

			if (dwBytesRead > 0)
			{
				GetLocalTime(&st);
				str1.Format(_T("%d-%d-%d %d:%d:%d Send File: %s %d bytes.\n"),
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, str,dwBytesRead);
				len = m_Edit_Receive.SendMessage(WM_GETTEXTLENGTH);
				m_Edit_Receive.SetSel(len,len);
				m_Edit_Receive.ReplaceSel(str1);
			}
		}
	}
}


void CCOMHelperDlg::OnBnClickedSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CCOMHelperDlg::OnEnChangePathEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR path[MAX_PATH];
	USES_CONVERSION;

	m_Edit_Path.GetWindowText(path, MAX_PATH);
	LPSTR apath = T2A(path);
	myini.Write("[SETTING]","dir",apath);

	int len = _tcslen(path);
	if (path[len-1] == _T('\\'))
	{
		_tcscat(path,  _T("*.bin"));
	}
	else
	{
		_tcscat(path,  _T("\\*.bin"));
	}
	
	hFind = FindFirstFile(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		m_List_TestCase.InsertString(0, FindFileData.cFileName);
	}while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
}


void CCOMHelperDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_IsConnected)
	{
		m_IsConnected = false;
	}
	else
	{
		GetDlgItem(IDC_CONNECT)->EnableWindow(0);
		hCom = CreateFile(cp.Port, GENERIC_READ|GENERIC_WRITE, 0,NULL , OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
		if (hCom == INVALID_HANDLE_VALUE)
		{
			MessageBox(cp.Port + _T(" can't be opened!"),_T("Error!"));
			GetDlgItem(IDC_CONNECT)->EnableWindow(1);
			return;
		}

		DCB dcb;
		COMMTIMEOUTS cti;
		ZeroMemory(&cti, sizeof(COMMTIMEOUTS));
		cti.ReadIntervalTimeout			= 1000; //MAXDWORD;
		cti.ReadTotalTimeoutMultiplier	= 1000; //MAXDWORD;
		cti.ReadTotalTimeoutConstant	= 1000;
		cti.WriteTotalTimeoutConstant = 1000;
		cti.WriteTotalTimeoutMultiplier = 1000;

		SetCommTimeouts(hCom, &cti);

		ZeroMemory(&dcb, sizeof(DCB));
		GetCommState(hCom, &dcb);

		dcb.BaudRate = cp.BaudRate;
		if (cp.Parity.Compare(_T("Odd")) == 0)
		{
			dcb.Parity = ODDPARITY;
		}
		else if (cp.Parity.Compare(_T("Even")) == 0)
		{
			dcb.Parity = EVENPARITY;
		}
		else
		{
			dcb.Parity = NOPARITY;
		}

		dcb.StopBits = cp.StopBits;

		dcb.ByteSize		= cp.DataBits;//(BYTE) pParams->COMParam.wCommDataBits;
		dcb.fAbortOnError	= FALSE; // Terminate Reads & Writes if there's an error
		dcb.fErrorChar		= TRUE;  // Replace any garbled bytes with ErrorChar
		dcb.ErrorChar		= ' ';   // Garbage bytes are spaces
		dcb.fBinary			= TRUE;  // Ignore EOF
		dcb.fParity			= TRUE;  // ������żУ��

		SetCommState(hCom, &dcb);
		PurgeComm(hCom, PURGE_RXABORT | PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

		m_IsConnected = true;
		GetDlgItem(IDC_CONNECT)->SetWindowText(_T("Disconnect"));
		GetDlgItem(IDC_CONNECT)->EnableWindow(1);

		CreateThread(
			NULL, 
			0, 
			ReadSerialPort,
			(LPVOID)this, 
			0, 
			NULL);
	}
}

DWORD WINAPI ReadSerialPort(LPVOID lpParameter)
{
	CCOMHelperDlg* pDlg = (CCOMHelperDlg*)lpParameter;
	pDlg->ReadCOM();
	return 0;
}

void CCOMHelperDlg::ReadCOM(void)
{
	BYTE szBuffer[1024];
	DWORD dwBytesRead = 0;
	BOOL bResult;
	SYSTEMTIME st;
	CString s;
	int len;
	FILE *f;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	OVERLAPPED m_osRead;
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	USES_CONVERSION;


	while (m_IsConnected)
	{
		//ClearCommError(hCom,&dwErrorFlags,&ComStat);
		bResult = ReadFile(hCom, szBuffer, sizeof(BYTE)*1024, &dwBytesRead, &m_osRead);
		if (!bResult)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				WaitForSingleObject(m_osRead.hEvent, 1000);
				if(!GetOverlappedResult(hCom,&m_osRead,&dwBytesRead,FALSE))
				{
					dwBytesRead = 0;
				}
			}
			else
			{
				dwBytesRead = 0;
			}
		}

		if (dwBytesRead > 0)
		{
			GetLocalTime(&st);
			s.Format(_T("%d-%d-%d %d:%d:%d Receive %d bytes.\n"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, dwBytesRead);
			len = m_Edit_Receive.SendMessage(WM_GETTEXTLENGTH);
			m_Edit_Receive.SetSel(len,len);
			m_Edit_Receive.ReplaceSel(s);
			s.Format(_T("%d_%d_%d_%d_%d_%d.data"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			LPSTR apath = T2A(s);
			f = fopen(apath,"wb");
			fwrite(szBuffer, 1, dwBytesRead, f);
			fclose(f);
		}
		ResetEvent(m_osRead.hEvent);
	}

	CloseHandle(hCom);
	GetDlgItem(IDC_CONNECT)->SetWindowText(_T("Connect"));
}
