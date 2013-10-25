#pragma once
#include "TrafficCtrl.h"

class CDlgTrace : public CDialogImpl<CDlgTrace>
{
private:
	CTrafficCtrl m_trafficCtrl;
public:
	enum { IDD = IDD_DLGTRACE };

	BEGIN_MSG_MAP( CDlgTrace )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	END_MSG_MAP()

	CDlgTrace()
	{

	}

	virtual ~CDlgTrace()
	{

	}

	LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		m_trafficCtrl.Create(m_hWnd,CRect(0,0,0,0));
		return TRUE;
	}
protected:
private:
};