
// BigFileSplitterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BigFileSplitter.h"
#include "BigFileSplitterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBigFileSplitterDlg 对话框




CBigFileSplitterDlg::CBigFileSplitterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBigFileSplitterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_file_name = _T("");
	m_file_size = _T("");
}

void CBigFileSplitterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_FILE_NAME, m_edit_file_name);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_file_name);
	DDX_Text(pDX, IDC_EDIT_FILE_SIZE, m_file_size);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_BUTTON_SPLIT, m_btn_split);
}

BEGIN_MESSAGE_MAP(CBigFileSplitterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FILE_SELECT, &CBigFileSplitterDlg::OnBnClickedButtonFileSelect)
	ON_BN_CLICKED(IDC_BUTTON_SPLIT, &CBigFileSplitterDlg::OnBnClickedButtonSplit)
END_MESSAGE_MAP()


// CBigFileSplitterDlg 消息处理程序

BOOL CBigFileSplitterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBigFileSplitterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBigFileSplitterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBigFileSplitterDlg::OnBnClickedButtonFileSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		m_file_name = pathName;
		UpdateData(FALSE);
	}
}

DWORD WINAPI ThreadSplitFile(LPVOID lpParameter)
{
	CBigFileSplitterDlg* pDlg = (CBigFileSplitterDlg*)lpParameter;
	//pDlg->UpdateData(TRUE);
	USES_CONVERSION;
	LPSTR char_file_name = T2A(pDlg->m_file_name);
	LPSTR char_file_size = T2A(pDlg->m_file_size);
	int long_file_size = atoi(char_file_size);
	int radio_id = pDlg->GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3);
	switch(radio_id)
	{
		case IDC_RADIO1:
			long_file_size = long_file_size * 1024 * 1024;
			break;
		case IDC_RADIO2:
			long_file_size = long_file_size * 1024;
			break;
	}
	pDlg->m_btn_split.EnableWindow(FALSE);
	pDlg->SplitFile(char_file_name, long_file_size);
	pDlg->m_btn_split.EnableWindow(TRUE);
	return 0;
}

void CBigFileSplitterDlg::OnBnClickedButtonSplit()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CreateThread(
			NULL, 
			0, 
			ThreadSplitFile,
			(LPVOID)this, 
			0, 
			NULL);
}

#define PAGE_SIZE 102400
void CBigFileSplitterDlg::SplitFile(char* _file_name, int _file_size)
{
	char buf[PAGE_SIZE];
	char file_name_title[256];
	char file_name_ext[20];
	char file_name_out[256];
	char* p_write;
	int file_total_size, page_num, i, file_name_len, read_bytes, write_bytes;
	int write_len = 0, write_file_num = 0;

	FILE* f = fopen(_file_name,"rb");
	FILE* fout;
	if (f == NULL)
	{
		return;
	}

	file_name_len = strlen(_file_name);
	for (i = file_name_len; i >= 0; i--)
	{
		if (_file_name[i] == '.')
		{
			break;
		}
	}
	if (i == 0)
	{
		strncpy(file_name_title, _file_name, file_name_len);
		file_name_title[file_name_len] = 0;
		file_name_ext[0] = 0;
	}
	else
	{
		strncpy(file_name_title, _file_name, i);
		file_name_title[i] = 0;
		strncpy(file_name_ext, &(_file_name[i + 1]), file_name_len - i - 1);
		file_name_ext[file_name_len - i - 1] = 0;
	}

	fseek(f, 0, SEEK_END);
	file_total_size = ftell(f);
	page_num = (file_total_size / PAGE_SIZE) + 1;
	fseek(f, 0, SEEK_SET);

	m_progress.SetRange(0, page_num);
	m_progress.SetStep(1);
	m_progress.SetPos(0);
	for (i=0; i < page_num; i++)
	{
		read_bytes = fread(buf, sizeof(char), PAGE_SIZE, f);
		p_write = buf;
		while (read_bytes > 0)
		{
			if (write_len == 0)
			{
				sprintf(file_name_out, "%s_%d.%s",
					file_name_title,
					write_file_num,
					file_name_ext
					);
				fout = fopen(file_name_out, "wb");
			}

			if (write_len + read_bytes < _file_size)
			{
				write_bytes = read_bytes;
				fwrite(p_write, sizeof(char), write_bytes, fout);
				write_len += write_bytes;
				read_bytes = 0;
			}
			else
			{
				write_bytes = _file_size - write_len;
				fwrite(p_write, sizeof(char), write_bytes, fout);
				fclose(fout);
				fout = NULL;
				write_file_num++;
				write_len = 0;
				p_write += write_bytes;
				read_bytes -= write_bytes;
			}
		}
		m_progress.StepIt();
	}

	if (fout != NULL)
	{
		fclose(fout);
	}
	fclose(f);
}
