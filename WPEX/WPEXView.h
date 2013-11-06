// WPEXView.h : interface of the CWPEXView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atltypes.h>
#include "CustomDef.h"
#include "WPEXUtily.h"
#include "DlgSendSocket.h"
#include "ListViewCtrlEx.h"

UINT g_viewIndex = -1;
class CWPEXView : public CDialogImpl<CWPEXView>
{
private:
    UINT m_viewIndex;
    CListViewCtrlEx m_listCtrl;
public:
    enum { IDD = IDD_WPEX_FORM };
    
    BOOL PreTranslateMessage( MSG* pMsg )
    {
        return CWindow::IsDialogMessage( pMsg );
    }
    
    BEGIN_MSG_MAP( CWPEXView )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_SIZE, OnSize )
    NOTIFY_HANDLER( IDC_LIST1, NM_DBLCLK, OnDbClickListCtrl )
    NOTIFY_HANDLER( IDC_LIST1, NM_RCLICK, OnRClickListCtrl )
    COMMAND_ID_HANDLER( ID_CLEAR, OnClear )
    NOTIFY_HANDLER( IDC_LIST1, LVN_COLUMNCLICK, OnLvnColumnClick )
    REFLECT_NOTIFICATIONS() //消息反射通知宏 必须要加的，这个东西折磨了我好多天了，哈哈
    END_MSG_MAP()
    
    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        //        m_listCtrl = GetDlgItem( IDC_LIST1 );
        m_listCtrl.SubclassWindow( GetDlgItem( IDC_LIST1 ) );
        m_listCtrl.ModifyStyle( 0, LVS_OWNERDRAWFIXED );
        m_listCtrl.SetItemHeight( 30 );
        m_listCtrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, 0 );
        m_listCtrl.InsertColumn( 0, _T( "Packet" ), LVCFMT_LEFT, 50 );
        m_listCtrl.InsertColumn( 1, _T( "Source" ), LVCFMT_LEFT, 150 );
        m_listCtrl.InsertColumn( 2, _T( "Destination" ), LVCFMT_LEFT, 150 );
        m_listCtrl.InsertColumn( 3, _T( "size" ), LVCFMT_LEFT, 50 );
        m_listCtrl.InsertColumn( 4, _T( "Function" ), LVCFMT_LEFT, 100 );
        m_listCtrl.InsertColumn( 5, _T( "Content" ), LVCFMT_LEFT, 600 );
        
        g_viewIndex++;
        m_viewIndex = g_viewIndex;
        
        // test
        // 		for (int i=0;i<20;i++)
        // 		{
        // 			CString s;
        // 			s.Format(_T("测试---------%d"),i);
        // 			int id=m_listCtrl.InsertItem(i,s);
        // 			m_listCtrl.SetItemText(id,1,_T("HElo"));
        // 			m_listCtrl.SetItemText(id,2,s);
        // 		}
        return 0;
    }
    
    LRESULT OnLvnColumnClick( int wParam, LPNMHDR lParam, BOOL& bHandled )
    {
        m_listCtrl.SortItems( &CWPEXView::CompareFunc, 0 );
        return 0;
    }
    
    static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
    {
        return FALSE;
    }
    
    UINT GetIndex()const
    {
        return m_viewIndex;
    }
    
    LRESULT OnSize( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        if ( m_listCtrl.IsWindow() )
        {
            CRect rcClient;
            GetClientRect( &rcClient );
            m_listCtrl.MoveWindow( &rcClient, TRUE );
        }
        
        return 0;
    }
    
    void OnSocketData( SOCKETDATA* lpSocketData )
    {
        SOCKET socket = lpSocketData->s;
        DWORD dwDataLen = lpSocketData->dwDataLen;
        BYTE* pData = ( BYTE* )lpSocketData->lpData;
        
        int packetIndex = m_listCtrl.GetItemCount();
        
        TCHAR sPacketIndex[8] = {0};
        _stprintf_s( sPacketIndex, _T( "%d" ), packetIndex + 1 );
        m_listCtrl.InsertItem( packetIndex, sPacketIndex );
        
        SOCKETDATA* pSocketData = ( SOCKETDATA* )new BYTE[lpSocketData->cbSize];
        memcpy_s( pSocketData, lpSocketData->cbSize, lpSocketData, lpSocketData->cbSize );
        m_listCtrl.SetItemData( packetIndex, ( DWORD_PTR )( pSocketData ) );
        
        TCHAR sSource[MAX_PATH] = {0};
        char* lpSrcIP = inet_ntoa( lpSocketData->srcsockaddr.sin_addr );
        USHORT nSrcPort = htons( lpSocketData->srcsockaddr.sin_port );
        TCHAR* pSrcIP = AnsiToUnicode( lpSrcIP );
        _stprintf_s( sSource, _T( "%s:%u" ), pSrcIP, nSrcPort );
        delete pSrcIP;
        pSrcIP = NULL;
        m_listCtrl.SetItemText( packetIndex, 1, sSource );
        
        TCHAR sDestination[MAX_PATH] = {0};
        char* lpDestIP = inet_ntoa( lpSocketData->destsockaddr.sin_addr );
        USHORT nDestPort = htons( lpSocketData->destsockaddr.sin_port );
        TCHAR* pDestIP = AnsiToUnicode( lpDestIP );
        _stprintf_s( sDestination, _T( "%s:%u" ), pDestIP, nDestPort );
        delete pDestIP;
        pDestIP = NULL;
        m_listCtrl.SetItemText( packetIndex, 2, sDestination );
        
        TCHAR sDataLen[10] = {0};
        _stprintf_s( sDataLen, _T( "%d" ), dwDataLen );
        m_listCtrl.SetItemText( packetIndex, 3, sDataLen );
        
        m_listCtrl.SetItemText( packetIndex, 4, lpSocketData->sFuncName );
        
        TCHAR* sData = AnsiToUnicode( ( const char* )pData );
        m_listCtrl.SetItemText( packetIndex, 5, sData );
        delete sData;
        sData = NULL;
    }
    
    LRESULT OnDbClickListCtrl( int idCtrl, LPNMHDR pnmh, BOOL& bHandled )
    {
        int selIndex = m_listCtrl.GetSelectedIndex();
        if ( -1 == selIndex )
        {
            return 0;
        }
        
        
        SOCKETDATA* pSocketData = ( SOCKETDATA* )m_listCtrl.GetItemData( selIndex );
        CDlgSendSocket dlgSendSocket( pSocketData );
        if ( IDOK == dlgSendSocket.DoModal() )
        {
        
        }
        return 0;
    }
    
    LRESULT OnRClickListCtrl( int idCtrl, LPNMHDR pnmh, BOOL& bHandled )
    {
        POINT ptCursor;
        ::GetCursorPos( &ptCursor );
        
        CMenu menu;
        menu.LoadMenu( IDR_RMENU );
        CMenu menuSub = menu.GetSubMenu( 0 );
        menuSub.TrackPopupMenu( TPM_LEFTALIGN, ptCursor.x, ptCursor.y, m_hWnd );
        return 0;
    }
    
    LRESULT OnClear( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        for ( int i = 0; i < m_listCtrl.GetItemCount(); i++ )
        {
            SOCKETDATA* pSocketData = ( SOCKETDATA* )m_listCtrl.GetItemData( i );
            if ( NULL != pSocketData )
            {
                delete pSocketData;
                pSocketData = NULL;
            }
        }
        m_listCtrl.DeleteAllItems();
        return 0;
    }
};
