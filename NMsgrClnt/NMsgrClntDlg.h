
// NMsgrClntDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#include "StateVecRecord.h"
#include "glBodyRenderer.h"

// CNMsgrClntDlg 대화 상자
class CNMsgrClntDlg : public CDialogEx
{
// 생성입니다.
public:
	CNMsgrClntDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	void PrintStatus(CString str);
	static unsigned __stdcall ReadThread(void* pArg);
	
	afx_msg LRESULT OnGetKinectData(WPARAM wParam, LPARAM lParam);

	SYSTEMTIME		m_st;

	int				m_nListCount;
	BOOL			CONNECT_STATUS;
	HANDLE			m_hThread;
	bool			m_bReceiveSkeletonMsg;

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_NMSGRCLNT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit_Status;
	afx_msg void OnBnClickedButtonConnect();
	CEdit m_Edit_IP;
	CEdit m_Edit_ID;
	CEdit m_Edit_Delay;
	CString m_strStatus;
	CString m_strIP;
	CString m_strID;
	CString m_strDelay;
	CButton m_Btn_Connect;
	afx_msg void OnDestroy();

private:
	StateVecRecord m_reader;
	int FrameCount;
	int m_delay;
	int isStoped;
public:
	afx_msg void OnBnClickedButtonConnect2();
};
