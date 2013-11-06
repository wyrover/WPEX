// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlsplit.h>
#include "LeftFrm.h"
#include "RightFrm.h"
#include "DlgProcessList.h"
#include "WPEXUtily.h"
#define WINDOW_MENU_POSITION	3

class CMainFrame :
    public CFrameWindowImpl<CMainFrame>,
    public CUpdateUI<CMainFrame>,
    public CMessageFilter, public CIdleHandler
{
private:
    CSplitterWindow m_splitter;
    
    CLeftFrame m_leftFrame;
    
    CRightFrame m_rightFrame;
    
public:
    DECLARE_FRAME_WND_CLASS( _T( "WPEXSOCKETSPY" ), IDR_MAINFRAME )
    
    virtual BOOL PreTranslateMessage( MSG* pMsg )
    {
        return CFrameWindowImpl<CMainFrame>::PreTranslateMessage( pMsg ) ;
    }
    
    virtual BOOL OnIdle()
    {
        UIUpdateToolBar();
        return FALSE;
    }
    
    BEGIN_UPDATE_UI_MAP( CMainFrame )
    UPDATE_ELEMENT( ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP )
    UPDATE_ELEMENT( ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP )
    END_UPDATE_UI_MAP()
    
    BEGIN_MSG_MAP( CMainFrame )
    MESSAGE_HANDLER( WM_COPYDATA, OnCopyData )
    COMMAND_ID_HANDLER( ID_OPEN_DLG_PROCESSLIST, OnOPenDlgProcessList )
    MESSAGE_HANDLER( WM_CREATE, OnCreate )
    MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
    COMMAND_ID_HANDLER( ID_START_WPEX, OnStart )
    COMMAND_ID_HANDLER( ID_STOP_WPEX, OnStop )
    COMMAND_ID_HANDLER( ID_VIEW_TOOLBAR, OnViewToolBar )
    COMMAND_ID_HANDLER( ID_VIEW_STATUS_BAR, OnViewStatusBar )
    COMMAND_ID_HANDLER( ID_APP_ABOUT, OnAppAbout )
    COMMAND_ID_HANDLER( ID_WINDOW_CLOSE, OnWindowClose )
    COMMAND_ID_HANDLER( ID_WINDOW_CLOSE_ALL, OnWindowCloseAll )
    COMMAND_RANGE_HANDLER( ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate )
    CHAIN_MSG_MAP( CUpdateUI<CMainFrame> )
    CHAIN_MSG_MAP( CFrameWindowImpl<CMainFrame> )
    END_MSG_MAP()
    
    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    void SetToolbarText()
    {
        CToolBarCtrl aToolBar = m_hWndToolBar;
        
        TBBUTTONINFO btnInfo = {0};
        btnInfo.cbSize = sizeof( TBBUTTONINFO );
        btnInfo.dwMask = TBIF_LPARAM | TBIF_BYINDEX | TBIF_SIZE | TBIF_TEXT;	//必须指定TBIF_LPARAM
        
        aToolBar.GetButtonInfo( 1, &btnInfo );		// 工具栏下标从1开始
        btnInfo.cx = 80;							// 索引为1的工具栏按钮高度
        btnInfo.pszText = _T( "选择进程" );				// 索引为1的工具栏按钮文字
        aToolBar.SetButtonInfo( 1, &btnInfo );
        
        aToolBar.GetButtonInfo( 2, &btnInfo );
        btnInfo.cx = 50;							// 索引为2的工具栏按钮高度
        btnInfo.pszText = _T( "启动" );				// 索引为2的工具栏按钮文字
        aToolBar.SetButtonInfo( 2, &btnInfo );
        
        aToolBar.GetButtonInfo( 3, &btnInfo );
        btnInfo.cx = 50;							// 索引为2的工具栏按钮高度
        btnInfo.pszText = _T( "暂停" );				// 索引为2的工具栏按钮文字
        aToolBar.SetButtonInfo( 3, &btnInfo );
        
        aToolBar.GetButtonInfo( 5, &btnInfo );
        btnInfo.cx = 50;							// 索引为2的工具栏按钮高度
        btnInfo.pszText = _T( "停止" );				// 索引为2的工具栏按钮文字
        aToolBar.SetButtonInfo( 5, &btnInfo );
        
        ::SendMessage( m_hWndToolBar, TB_SETBUTTONSIZE, 0, MAKELPARAM( 0, 50 ) );	// 设置按钮高度
    }
    
    LRESULT OnCreate( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CreateSimpleToolBar();
        SetToolbarText();
        CreateSimpleStatusBar();
        
        UIAddToolBar( m_hWndToolBar );
        UISetCheck( ID_VIEW_TOOLBAR, 1 );
        UISetCheck( ID_VIEW_STATUS_BAR, 1 );
        
        // register object for message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->AddMessageFilter( this );
        pLoop->AddIdleHandler( this );
        
        const DWORD dwSplitStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        const DWORD dwSplitExStyle = WS_EX_CLIENTEDGE;
        m_hWndClient = m_splitter.Create( m_hWnd, rcDefault, NULL, dwSplitStyle, dwSplitExStyle );
        m_splitter.SetSplitterExtendedStyle( SPLIT_FIXEDBARSIZE | SPLIT_NONINTERACTIVE );
        
        m_leftFrame.Create( m_splitter, 0 );
        m_leftFrame.ShowWindow( SW_SHOWNORMAL );
        
        CMenuHandle menuMain = GetMenu();
        HMENU hMenu = menuMain.GetSubMenu( WINDOW_MENU_POSITION );
        
        m_rightFrame.Create( m_splitter, ( LPARAM )hMenu );
        m_rightFrame.ShowWindow( SW_SHOWNORMAL );
        
        m_splitter.SetSplitterPanes( m_leftFrame, m_rightFrame );
        UpdateLayout();
        m_splitter.SetSplitterPos( LEFTFRAMEWIDTH );

//		m_rightFrame.AddPage(_T("Hello"),100);
        return 0;
    }
    
    LRESULT OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
    {
        // unregister message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->RemoveMessageFilter( this );
        pLoop->RemoveIdleHandler( this );
        		
        bHandled = FALSE;
        return 1;
    }
    
    LRESULT OnStart( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        m_rightFrame.OnStart();
        return 0;
    }
    
    LRESULT OnStop( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        m_rightFrame.OnStop();
        return 0;
    }
    
    LRESULT OnOPenDlgProcessList( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        //		m_rightFrame.AddPage(_T("fdsa"));
        CDlgProcessList dlgProcess;
        if ( IDOK == dlgProcess.DoModal() )
        {
            DWORD dwSelPID = dlgProcess.GetSelPID();
            if ( -1 != dwSelPID )
            {
                if ( IsInject( dwSelPID ) )
                {
                    return 0;
                }
                CString sDllPath = GetDllPath( );
                if ( InjectDll( dwSelPID, sDllPath ) )
                {
                    m_rightFrame.AddPage( dlgProcess.GetSelProName(), dlgProcess.GetSelPID() );
                }
                else
                {
                    MessageBox( _T( "注入失败!" ), _T( "错误" ), MB_ICONERROR );
                }
            }
        }
        return 0;
    }
    
    BOOL IsInject( DWORD dwPID )
    {
        return m_rightFrame.IsInject( dwPID );
    }
    
    LRESULT OnViewToolBar( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        BOOL bVisible = !::IsWindowVisible( m_hWndToolBar );
        ::ShowWindow( m_hWndToolBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE );
        UISetCheck( ID_VIEW_TOOLBAR, bVisible );
        UpdateLayout();
        return 0;
    }
    
    LRESULT OnViewStatusBar( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        BOOL bVisible = !::IsWindowVisible( m_hWndStatusBar );
        ::ShowWindow( m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE );
        UISetCheck( ID_VIEW_STATUS_BAR, bVisible );
        UpdateLayout();
        return 0;
    }
    
    LRESULT OnAppAbout( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CAboutDlg dlg;
        dlg.DoModal();
        return 0;
    }
    
    LRESULT OnWindowClose( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        m_rightFrame.OnWindowClose();
        return 0;
    }
    
    LRESULT OnWindowCloseAll( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        m_rightFrame.OnWindowCloseAll();
        
        return 0;
    }
    
    LRESULT OnWindowActivate( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        m_rightFrame.OnWindowActivate( wID );
        
        return 0;
    }
    
    LRESULT OnCopyData( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        COPYDATASTRUCT* pCopyDataStruct = ( COPYDATASTRUCT* )lParam;
        if ( NULL == pCopyDataStruct )
        {
            return 0;
        }
        if ( WM_DLLDATA == pCopyDataStruct->dwData )
        {
            SOCKETDATA* pDllData = ( SOCKETDATA* )pCopyDataStruct->lpData;
            m_rightFrame.OnSocketData( pDllData );
        }
        return 0;
    }
};
