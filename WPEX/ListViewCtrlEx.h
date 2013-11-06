#pragma once
#include <atlctrls.h>

class CListViewCtrlEx : public CWindowImpl<CListViewCtrlEx, CListViewCtrl>,
    public COwnerDraw<CListViewCtrlEx>
{
private:
    BOOL m_bTrackMouseHover;
    int m_iHoverIndex;
    int m_iPreHoverIndex;
    UINT m_uItemHeight;	// ITEM高度
    COLORREF m_clrItemSelected;	// ITEM被选中时的颜色
    COLORREF m_clrItemNormal;	// ITEM 默认颜色
    COLORREF m_clrItemHot;		// 鼠标划过时颜色
    CFontHandle m_font;		// 字体
    COLORREF m_clrText;	// 文本颜色
public:
    BEGIN_MSG_MAP( CListViewCtrlEx )
    CHAIN_MSG_MAP_ALT( COwnerDraw<CListViewCtrlEx>, 1 )
    MESSAGE_HANDLER( WM_MOUSEMOVE, OnMouseMove )
    MESSAGE_HANDLER( WM_MOUSEHOVER, OnMouseHover )
    MESSAGE_HANDLER( WM_MOUSELEAVE, OnMouseLeave )
    MESSAGE_HANDLER( WM_MOUSEWHEEL, OnMouseWheel )
    MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
    DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    
    CListViewCtrlEx()
    {
        m_bTrackMouseHover = FALSE;
        m_iHoverIndex = -1;
        m_iPreHoverIndex = -1;
        m_uItemHeight = 29;
        m_clrItemSelected =::GetSysColor( COLOR_HIGHLIGHT );
        m_clrItemNormal =::GetSysColor( COLOR_WINDOW );
        m_clrText =::GetSysColor( COLOR_BTNTEXT );
        m_clrItemHot = RGB( 192, 192, 100 );
        
        SetFont( 12, _T( "宋体" ) );
    }
    
    virtual ~CListViewCtrlEx()
    {
    
    }
    
    void SetFont( int iFontSize, LPCTSTR lpszFontName, BOOL bItalic = FALSE, BOOL bUnderline = FALSE )
    {
        LOGFONT lf;
        lf.lfWeight = FW_NORMAL;
        lf.lfHeight = iFontSize;
        lf.lfWidth = 0;
        lf.lfUnderline = bUnderline;
        lf.lfStrikeOut = FALSE;
        lf.lfItalic = bItalic;
        lf.lfEscapement = 0;
        lf.lfCharSet = DEFAULT_CHARSET;
        _tcscpy_s( lf.lfFaceName, lpszFontName );
        if ( !m_font.IsNull() )
        {
            m_font.DeleteObject();
        }
        m_font.CreateFontIndirect( &lf );
    }
    
    void OwnerDraw( )
    {
        ModifyStyle( 0, LVS_OWNERDRAWFIXED );
    }
    
    void SetItemHeight( UINT uHeight )
    {
        m_uItemHeight = uHeight;
        
        CRect rcWin;
        GetWindowRect( &rcWin );
        WINDOWPOS wp;
        wp.hwnd = m_hWnd;
        wp.cx = rcWin.Width();
        wp.cy = rcWin.Height();
        wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
        SendMessage( WM_WINDOWPOSCHANGED, 0, ( LPARAM )&wp );
    }
    
    LRESULT OnMouseMove( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        if ( !m_bTrackMouseHover )
        {
            TRACKMOUSEEVENT csTME;
            csTME.cbSize = sizeof( csTME );
            csTME.dwFlags = TME_LEAVE | TME_HOVER;
            csTME.hwndTrack = m_hWnd;
            csTME.dwHoverTime = 5;
            ::_TrackMouseEvent( &csTME );
        }
        
        return 0;
    }
    
    LRESULT OnMouseHover( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CPoint pt;
        GetCursorPos( &pt );
        ::ScreenToClient( m_hWnd, &pt );
        UINT uFlag = 0;
        m_iHoverIndex = HitTest( pt, &uFlag );
        if ( LVHT_NOWHERE != uFlag )
        {
            CRect rcItem;
            if ( m_iPreHoverIndex != m_iHoverIndex )
            {
                GetItemRect( m_iHoverIndex, &rcItem, LVIR_BOUNDS );
                InvalidateRect( &rcItem );
            }
            GetItemRect( m_iPreHoverIndex, &rcItem, LVIR_BOUNDS );
            InvalidateRect( &rcItem );
            m_iPreHoverIndex = m_iHoverIndex;
        }
        
        m_bTrackMouseHover = FALSE;
        return 0;
    }
    
    LRESULT OnMouseLeave( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CRect rcItem;
        GetItemRect( m_iHoverIndex, &rcItem, 3 );
        InvalidateRect( &rcItem );
        GetItemRect( m_iPreHoverIndex, &rcItem, 3 );
        InvalidateRect( &rcItem );
        m_iHoverIndex = -1;
        
        return 0;
    }
    
    LRESULT OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        OnMouseMove( uMsg, wParam, lParam, bHandled );
        bHandled = FALSE;
        return 0;
    }
    
    LRESULT OnEraseBkGnd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        return TRUE;
    }
    
    void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
    {
        CRect rcItem( lpDrawItemStruct->rcItem );
        CMemoryDC memDC( lpDrawItemStruct->hDC, rcItem );
        // 正常状态
        memDC.FillSolidRect( &rcItem, m_clrItemNormal );
        
        // 选择状态
        if ( ODA_DRAWENTIRE == ( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE ) )
        {
            if ( ODS_SELECTED == ( lpDrawItemStruct->itemState & ODS_SELECTED ) )
            {
                memDC.FillSolidRect( &rcItem, m_clrItemSelected );
            }
        }
        
        // 鼠标划过状态
        if ( lpDrawItemStruct->itemID == m_iHoverIndex && ( -1 != m_iHoverIndex ) )
        {
            memDC.FillSolidRect( &rcItem, m_clrItemHot );
        }
        // 绘制文本
        HFONT hOldFont = memDC.SelectFont( m_font.m_hFont );
        memDC.SetBkMode( TRANSPARENT );
        memDC.SetTextColor( m_clrText );
        int columnCount = GetHeader().GetItemCount();
        for ( int i = 0; i < columnCount; i++ )
        {
            RECT rcSubItem;
            GetSubItemRect( lpDrawItemStruct->itemID, i, LVIR_LABEL, &rcSubItem );
            rcSubItem.left += 3;
            BSTR sText = NULL;
            GetItemText( lpDrawItemStruct->itemID, i, sText );
            if ( NULL != sText )
            {
                memDC.DrawText( sText, _tcslen( sText ), &rcSubItem,  DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_LEFT | DT_VCENTER );
            }
        }
        memDC.SelectFont( hOldFont );
    }
    
    void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
    {
        if ( lpMeasureItemStruct->CtlType != ODT_MENU )
        {
            lpMeasureItemStruct->itemHeight = m_uItemHeight;
        }
        else
        {
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics( SM_CYMENU );
        }
    }
    
    int CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
    {
        // all items are equal
        return 0;
    }
    
    void DeleteItem( LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/ )
    {
        // default - nothing
    }
};