
// NMsgrClntDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "NMsgrClnt.h"
#include "NMsgrClntDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	// �����Դϴ�.
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


// CNMsgrClntDlg ��ȭ ����




CNMsgrClntDlg::CNMsgrClntDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNMsgrClntDlg::IDD, pParent)
	, m_strStatus(_T(""))
	, m_strIP(_T(""))
	, m_strID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	CONNECT_STATUS = FALSE;
	m_nListCount = 0;
	m_hThread = NULL;
}

void CNMsgrClntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_Edit_Status);
	DDX_Control(pDX, IDC_EDIT_IP, m_Edit_IP);
	DDX_Control(pDX, IDC_EDIT_ID, m_Edit_ID);
	DDX_Control(pDX, IDC_EDIT_ID2, m_Edit_Delay);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIP);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_ID2, m_strDelay);
	DDX_Control(pDX, ID_BUTTON_CONNECT, m_Btn_Connect);
}

BEGIN_MESSAGE_MAP(CNMsgrClntDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BUTTON_CONNECT, &CNMsgrClntDlg::OnBnClickedButtonConnect)
	ON_WM_DESTROY()

	ON_MESSAGE(UM_GET_KINECT_DATA, OnGetKinectData)
	ON_BN_CLICKED(ID_BUTTON_CONNECT2, &CNMsgrClntDlg::OnBnClickedButtonConnect2)
END_MESSAGE_MAP()


// CNMsgrClntDlg �޽��� ó����

BOOL CNMsgrClntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	ShowWindow(SW_SHOW);

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Edit_IP.SetWindowTextA("./BinaryData");
	//	m_Edit_IP.SetWindowTextA("127.0.0.1");
	m_Edit_ID.SetWindowTextA("drink1");
	m_Edit_Delay.SetWindowText("30");

	m_bReceiveSkeletonMsg = true;

	SetWindowPos(NULL, 200, 530, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	HANDLE hThread;
	hThread = (HANDLE)_beginthreadex(NULL,	// Security attributes
		0,	// stack
		ReadThread,			// Thread proc
		this,	// Thread param
		0,	// creation mode
		NULL);	// Thread ID

	if ( NULL != hThread)
	{
		//SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
		ResumeThread( hThread );
		m_hThread = hThread;
	}

	FrameCount = 0;
	m_delay = 30;
	isStoped = -1;

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CNMsgrClntDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CNMsgrClntDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CNMsgrClntDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNMsgrClntDlg::OnBnClickedButtonConnect()
{
	CString str;

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	if (m_strIP=="") 
	{
		PrintStatus("Path ����");
		m_Edit_IP.SetFocus();
	}
	else if (m_strID=="") 
	{
		PrintStatus("���� �̸� ����");
		m_Edit_ID.SetFocus();
	}
	else if (m_strDelay=="")
	{
		PrintStatus("Delay Error!");
		m_Edit_Delay.SetFocus();
	}
	else {
		//File Open


		if(m_reader.isOpened())
			m_reader.CloseFile();

		FrameCount = 0;

		str.Format("===================[%s] start====================", m_strID.GetBuffer());
		PrintStatus(str);
		m_delay = atoi(m_strDelay.GetBuffer());
		m_reader.OpenFile(m_strIP.GetBuffer(), m_strID.GetBuffer(), 'r');
	}	
}

/*--------------------------------------------------------------
| PrintStatus : Ŭ���̾�Ʈ ���� ��� 
| argument	  : str - ���� ���� 
+-------------------------------------------------------------*/
void CNMsgrClntDlg::PrintStatus(CString str)
{
	if (str == "") return;

	CString prevStr, curStr;

	if(m_Edit_Status.GetLineCount() >= 100)
	{
		int nLength = 0;

		for(int i = 0; i < 100; i++)
			nLength += m_Edit_Status.LineLength(i);

		m_Edit_Status.SetSel( 0, nLength, FALSE);
		m_Edit_Status.ReplaceSel("", FALSE); 
	}

	m_Edit_Status.GetWindowTextA(prevStr);

	str += "\r\n";
	curStr = prevStr + str;

	m_Edit_Status.SetWindowTextA(curStr);

	m_Edit_Status.LineScroll(m_Edit_Status.GetLineCount());
}

unsigned __stdcall CNMsgrClntDlg::ReadThread(void* pArg)
{
	CNMsgrClntDlg* pDlg = (CNMsgrClntDlg*)pArg;

	StateVector tempVec;
	CString str;

	glBodyRenderer m_renderer;

	m_renderer.InitializeRenderer("StateVec", 200, 40);

	while(true){
		if(pDlg->isStoped != 1){

			if(pDlg->m_reader.isOpened()){
				pDlg->FrameCount++;
				str.Format("[%d] frame", pDlg->FrameCount);
				pDlg->PrintStatus(str);

				if(!pDlg->m_reader.ReadState(&tempVec))		pDlg->PrintStatus("Gesture End");

				/*pDlg->*/m_renderer.Display(&tempVec);
			}else
				/*pDlg->*/m_renderer.Display(NULL);
		
			Sleep(pDlg->m_delay);
		}
	}
	return 0;
}

afx_msg LRESULT CNMsgrClntDlg::OnGetKinectData(WPARAM wParam, LPARAM lParam)
{
	WORD wTotal=0, wData=SKELETON_DATA_SIZE;
	BYTE bySkeleton[SKELETON_DATA_SIZE], *bySend=NULL, *nAddress=NULL;  //byPass[12],
	memset(bySkeleton, 0x00, SKELETON_DATA_SIZE);
	//memset(byPass, 0x00, 12);

	wTotal = HEAD_SIZE + wData + EFCD_SIZE;

	bySend = new BYTE[wTotal];
	memset(bySend, 0x00, wTotal);

	memcpy(bySend, &wTotal, sizeof(WORD));
	nAddress=bySend+sizeof(WORD);	*nAddress = SKELETON_DATA;
	nAddress++;						memset(nAddress, 0x00, sizeof(WORD));
	nAddress+=sizeof(WORD);			memcpy(nAddress, &wData, sizeof(WORD));
	nAddress+=sizeof(WORD);			memcpy(nAddress, bySkeleton, SKELETON_DATA_SIZE);
	nAddress+=wData;				*nAddress = 0xef;
	nAddress++;						*nAddress = 0xcd;

	return 1; 	
}

void CNMsgrClntDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
}

void CNMsgrClntDlg::OnBnClickedButtonConnect2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	isStoped *= -1;
	if(isStoped == 1)
		PrintStatus("Stop");
	else if(isStoped == -1)
		PrintStatus("Start");
	else
		PrintStatus("Error");
}
