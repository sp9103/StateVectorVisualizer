
// NMsgrClnt.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CNMsgrClntApp:
// �� Ŭ������ ������ ���ؼ��� NMsgrClnt.cpp�� �����Ͻʽÿ�.
//

class CNMsgrClntApp : public CWinApp
{
public:
	CNMsgrClntApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CNMsgrClntApp theApp;