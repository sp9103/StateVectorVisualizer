
// NMsgrClntDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

#include "StateVecRecord.h"
#include "glBodyRenderer.h"

// CNMsgrClntDlg ��ȭ ����
class CNMsgrClntDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CNMsgrClntDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	void PrintStatus(CString str);
	static unsigned __stdcall ReadThread(void* pArg);
	
	afx_msg LRESULT OnGetKinectData(WPARAM wParam, LPARAM lParam);

	SYSTEMTIME		m_st;

	int				m_nListCount;
	BOOL			CONNECT_STATUS;
	HANDLE			m_hThread;
	bool			m_bReceiveSkeletonMsg;

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NMSGRCLNT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
