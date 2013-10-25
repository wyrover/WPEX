// 流量控件
#pragma once
#include <atlwin.h>
#include <tchar.h>
#include <atltypes.h>
#include <atlgdi.h>

class CTrafficCtrl : public CWindowImpl<CTrafficCtrl>
{
private:
	COLORREF m_clrBK;
	CRect m_rcClient;
public:
    DECLARE_WND_CLASS( _T( "TrafficCtrl" ) )
    
    BEGIN_MSG_MAP( CTrafficCtrl )
    MESSAGE_HANDLER( WM_CREATE, OnCreate )
//    MESSAGE_HANDLER( WM_PAINT, OnPaint )
	MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkGnd)
    END_MSG_MAP()

	CTrafficCtrl()
	{
		m_clrBK=RGB(192,192,192);
	}
    
	virtual ~CTrafficCtrl()
	{

	}
    BOOL Create( HWND hParent, const CRect& rcMargin)
    {
        ::GetClientRect( hParent, &m_rcClient );
        if ( !rcMargin.IsRectNull() )
        {
            m_rcClient.DeflateRect( &rcMargin );
        }
        
        HWND hWnd = CWindowImpl::Create( hParent, m_rcClient,0,WS_CHILD|WS_VISIBLE );
        if ( NULL == hWnd )
        {
            ATLTRACE( _T( "流量组件创建失败" ) );
            return FALSE;
        }
        return TRUE;
    }
    
    LRESULT OnCreate( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        return 0;
    }
    
    LRESULT OnPaint( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
// 		CDCHandle dc=GetWindowDC();
// 		dc.SetBkMode(TRANSPARENT);
// 		dc.DrawText(_T("hello"),5,&m_rcClient,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		SetMsgHandled(FALSE);
        return 0;
    }

	LRESULT OnEraseBkGnd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		CDCHandle dc=GetWindowDC();
		dc.SetBkMode(TRANSPARENT);
		dc.FillSolidRect(&m_rcClient,m_clrBK);
		dc.DrawText(_T("hello"),5,&m_rcClient,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		bHandled=TRUE;
		return TRUE;
	}
};