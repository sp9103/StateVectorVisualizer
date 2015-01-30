
// NMsgrClntDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "NMsgrClnt.h"
#include "NMsgrClntDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
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


// CNMsgrClntDlg 대화 상자




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


// CNMsgrClntDlg 메시지 처리기

BOOL CNMsgrClntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	ShowWindow(SW_SHOW);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNMsgrClntDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNMsgrClntDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNMsgrClntDlg::OnBnClickedButtonConnect()
{
	CString str;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_strIP=="") 
	{
		PrintStatus("Path 공란");
		m_Edit_IP.SetFocus();
	}
	else if (m_strID=="") 
	{
		PrintStatus("파일 이름 공란");
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
| PrintStatus : 클라이언트 정보 출력 
| argument	  : str - 정보 내용 
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	isStoped *= -1;
	if(isStoped == 1)
		PrintStatus("Stop");
	else if(isStoped == -1)
		PrintStatus("Start");
	else
		PrintStatus("Error");
}
