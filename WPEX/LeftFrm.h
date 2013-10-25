#pragma once
#include <atlwin.h>
#include <atltypes.h>
#include "resource.h"
#include "DlgTrace.h"
#include "DlgFilter.h"
#include "CustomDef.h"

class CLeftFrame : public CDialogImpl<CLeftFrame>
{
private:
	CDlgTrace m_dlgTrace;
	CDlgFilter m_dlgFilter;
public:
    enum { IDD = IDD_LEFTFRAME };
    
    BEGIN_MSG_MAP( CLeftFrame )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    END_MSG_MAP()
    
    CLeftFrame()
    {
    
    }
    
    virtual ~CLeftFrame()
    {
    
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
		m_dlgTrace.Create(m_hWnd);
		m_dlgTrace.ShowWindow(SW_SHOWNORMAL);

		CRect rcDlgTrace;
		m_dlgTrace.GetClientRect(&rcDlgTrace);
		m_dlgFilter.Create(m_hWnd);
		m_dlgFilter.ShowWindow(SW_SHOWNORMAL);
		m_dlgFilter.SetWindowPos(NULL,rcDlgTrace.left,rcDlgTrace.bottom+3,0,0,SWP_NOSIZE|SWP_NOACTIVATE);

        return TRUE;
    }
};