#pragma once
#include "resource.h"
#include "MainFrm.h"
#include <map>
#include <utility>
#include "WPEXUtily.h"

class CRightFrame : public CDialogImpl<CRightFrame>
{
private:
    CTabView m_view;
    BOOL m_bTabViewCreated;
    std::map<DWORD, CWPEXView*> m_map;
public:
    enum { IDD = IDD_RIGHTFRAME };
    
    BEGIN_MSG_MAP( CRightFrame )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_SIZE, OnSize )
    END_MSG_MAP()
    
    CRightFrame()
    {
        m_bTabViewCreated = FALSE;
    }
    
    virtual ~CRightFrame()
    {
    
    }
    
    void CreateTabView( HMENU hMenu )
    {
        if ( !m_bTabViewCreated )
        {
            m_view.Create( m_hWnd, rcDefault, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE );
            m_view.SetWindowMenu( hMenu );
            m_bTabViewCreated = TRUE;
        }
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
    {
        HMENU hMenu = ( HMENU )lParam;
        CreateTabView( hMenu );
        
        return TRUE;
    }
    
    LRESULT OnSize( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        if ( m_view.IsWindow() )
        {
            CRect rcClient;
            GetClientRect( &rcClient );
            m_view.MoveWindow( &rcClient );
        }
        
        return 0;
    }
    
    void AddPage( LPCTSTR lpCaption , DWORD dwPID )
    {
        CWPEXView* pView = new CWPEXView;
        pView->Create( m_view );
        m_view.AddPage( pView->m_hWnd, lpCaption, -1, &dwPID );
        m_view.SetPageData( m_view.GetActivePage(), new DWORD( dwPID ) );
        m_map.insert( std::make_pair( dwPID, pView ) );
    }
    
    void OnWindowClose()
    {
        if ( 0 == m_view.GetPageCount() )
        {
            m_view.ShowWindow( SW_HIDE );
        }
        
        int nActivePage = m_view.GetActivePage();
        if ( nActivePage != -1 )
        {
            DWORD* dwPID = ( DWORD* )m_view.GetPageData( nActivePage );
            UnInjectDll( *dwPID, GetDllPath() );
            delete dwPID;
            dwPID = NULL;
            
            m_view.RemovePage( nActivePage );
        }
        else
            ::MessageBeep( ( UINT ) - 1 );
    }
    
    void OnWindowCloseAll()
    {
        m_view.RemoveAllPages();
        m_view.ShowWindow( SW_HIDE );
    }
    
    void OnWindowActivate( WORD wID )
    {
        int nPage = wID - ID_WINDOW_TABFIRST;
        m_view.SetActivePage( nPage );
    }
    
    void OnSocketData( SOCKETDATA* pData )
    {
        if ( NULL == pData )
        {
            return ;
        }
        DWORD dwPID = pData->dwPID;
        CWPEXView* pView = GetViewByPID( dwPID );
        if ( NULL != pView )
        {
            pView->OnSocketData( pData );
        }
    }
    
    CWPEXView* GetViewByPID( DWORD dwPID )
    {
        std::map<DWORD, CWPEXView*>::const_iterator cit = m_map.find( dwPID );
        if ( m_map.end() != cit )
        {
            CWPEXView* pView = cit->second;
            return pView;
        }
        return NULL;
    }
    
    CWPEXView* GetViewByTabID( DWORD dwTabID )
    {
        DWORD* dwPID = ( DWORD* )m_view.GetPageData( dwTabID );
        return GetViewByPID( *dwPID );
    }
    
    void OnStart()
    {
        int iActivePate = m_view.GetActivePage();
        if ( -1 == iActivePate )
        {
            return ;
        }
        
        DWORD* pPID = ( DWORD* )m_view.GetPageData( iActivePate );
        TCHAR sPathWndClassName[MAX_PATH] = {0};
        _stprintf_s( sPathWndClassName, MAX_PATH, CLIENT_WNDCLASSNAME, *pPID );
        HWND hWndPatchDll =::FindWindow( sPathWndClassName, NULL );
        if ( ::IsWindow( hWndPatchDll ) )
        {
            ::SendMessage( hWndPatchDll, WM_STARTWPEX, 0, 0 );
        }
    }
    
    void OnStop()
    {
        int iActivePate = m_view.GetActivePage();
        if ( -1 == iActivePate )
        {
            return ;
        }
        DWORD* pPID = ( DWORD* )m_view.GetPageData( iActivePate );
        TCHAR sPathWndClassName[MAX_PATH] = {0};
        _stprintf_s( sPathWndClassName, MAX_PATH, CLIENT_WNDCLASSNAME, *pPID );
        HWND hWndPatchDll =::FindWindow( sPathWndClassName, NULL );
        if ( ::IsWindow( hWndPatchDll ) )
        {
            ::SendMessage( hWndPatchDll, WM_STOPWPEX, 0, 0 );
        }
    }
    
    BOOL IsInject( DWORD dwPID )
    {
        CWPEXView* pView = GetViewByPID( dwPID );
        if ( NULL != pView )
        {
            m_view.SetActivePage( pView->GetIndex() );
            return TRUE;
        }
        return FALSE;
    }
};