
#pragma once
#include <atlctrls.h>
#include "CustomDef.h"
#include "SocketHandler.h"
#include <assert.h>
class CDlgSendSocket : public CDialogImpl<CDlgSendSocket>
{
private:
    enum SENDTYPE
    {
        SEND_BY_NEWSOCKET,
        SEND_BY_RAWSOCKET
    } m_sendType;
    CSocketSendHandler* m_pSocketHandler;
    SOCKETDATA* m_pSocketData;
public:
    enum { IDD = IDD_DLG_SENDSOCKET };
    
    BEGIN_MSG_MAP( CDlgSendSocket )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_CLOSE, OnClose )
    COMMAND_ID_HANDLER( IDC_BUTTON1, OnSend )
    COMMAND_ID_HANDLER( IDC_BUTTON6, OnQuit )
    COMMAND_ID_HANDLER( IDC_RADIO1, OnRadioSendByContinue )
    COMMAND_ID_HANDLER( IDC_RADIO2, OnRadioSendByTime )
    COMMAND_ID_HANDLER( IDC_RADIO3, OnRadioSocketRaw )
    COMMAND_ID_HANDLER( IDC_RADIO4, OnRadioSocketNew )
    END_MSG_MAP()
    
    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    CDlgSendSocket( SOCKETDATA* pSocketData )
    {
        m_pSocketHandler = NULL;
        m_pSocketData = pSocketData;
    }
    
    virtual ~CDlgSendSocket()
    {
        if ( NULL != m_pSocketHandler )
        {
            delete m_pSocketHandler;
            m_pSocketHandler = NULL;
        }
        
    }
    
    // 设置"send"组下的按钮状态
    void OnSendCtrlStatus( UINT uRadioID )
    {
        CButton btn = GetDlgItem( uRadioID );
        btn.SetCheck( BST_CHECKED );
        
        CEdit editSendCount = GetDlgItem( IDC_EDIT2 );
        CEdit editSendTime = GetDlgItem( IDC_EDIT3 );
        if ( IDC_RADIO1 == uRadioID )
        {
            editSendCount.EnableWindow( FALSE );
            editSendTime.EnableWindow( FALSE );
        }
        else if ( IDC_RADIO2 == uRadioID )
        {
            editSendCount.EnableWindow( TRUE );
            editSendTime.EnableWindow( TRUE );
        }
    }
    
    void OnSocketType( UINT uRadioID )
    {
        CButton btn = GetDlgItem( uRadioID );
        btn.SetCheck( BST_CHECKED );
        
        CEdit editSocketID = GetDlgItem( IDC_EDIT4 );
        CIPAddressCtrl ipAddrCtrl = GetDlgItem( IDC_IPADDRESS1 );
        CEdit editPort = GetDlgItem( IDC_EDIT5 );
        
        if ( IDC_RADIO3 == uRadioID )
        {
            m_sendType = SEND_BY_RAWSOCKET;
            editSocketID.EnableWindow( TRUE );
            ipAddrCtrl.EnableWindow( FALSE );
            editPort.EnableWindow( FALSE );
        }
        else if ( IDC_RADIO4 == uRadioID )
        {
            m_sendType = SEND_BY_NEWSOCKET;
            editSocketID.EnableWindow( FALSE );
            ipAddrCtrl.EnableWindow( TRUE );
            editPort.EnableWindow( TRUE );
        }
    }
    
    // 持续发送
    LRESULT OnRadioSendByContinue( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendCtrlStatus( IDC_RADIO1 );
        return 0;
    }
    
    // 按次发送
    LRESULT OnRadioSendByTime( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendCtrlStatus( IDC_RADIO2 );
        return 0;
    }
    
    // 使用原始socket
    LRESULT OnRadioSocketRaw( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSocketType( IDC_RADIO3 );
        return 0;
    }
    
    // 新建socket
    LRESULT OnRadioSocketNew( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSocketType( IDC_RADIO4 );
        return 0;
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CenterWindow( GetParent() );
        
        OnSendCtrlStatus( IDC_RADIO2 );
        OnSocketType( IDC_RADIO3 );
        
        TCHAR sDestination[MAX_PATH] = {0};
        char* lpDestIP = inet_ntoa( m_pSocketData->destsockaddr.sin_addr );	// 目标地址
        USHORT nDestPort = htons( m_pSocketData->destsockaddr.sin_port );	// 目标端口号
        TCHAR sPort[8] = {0};
        _stprintf_s( sPort, 8, _T( "%u" ), nDestPort );
        TCHAR* pDestIP = AnsiToUnicode( lpDestIP );
        CIPAddressCtrl ipAddrCtrl = GetDlgItem( IDC_IPADDRESS1 );
        ipAddrCtrl.SetWindowText( pDestIP );
        CEdit editPort = GetDlgItem( IDC_EDIT5 );
        editPort.SetWindowText( sPort );
        
        TCHAR* sData = AnsiToUnicode( ( const char* )&m_pSocketData->lpData );
        CEdit editData = GetDlgItem( IDC_EDIT1 );
        editData.SetWindowText( sData );
        
        delete sData;
        sData = NULL;
        delete pDestIP;
        pDestIP = NULL;
        
        return TRUE;
    }
    
    LRESULT OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CloseDlg();
        return 0;
    }
    
    LRESULT OnSend( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
        wVersionRequested = MAKEWORD( 2, 2 );
        err = WSAStartup( wVersionRequested, &wsaData );
        assert( err == 0 );
        
        CEdit editData = GetDlgItem( IDC_EDIT1 );
        CString sData;
        editData.GetWindowText( sData );
        char* ansiData = UnicodeToAnsi( sData );
        int iDatalen = sData.GetLength() * sizeof( TCHAR );
        if ( SEND_BY_NEWSOCKET == m_sendType )
        {
            CIPAddressCtrl ipAddrCtrl = GetDlgItem( IDC_IPADDRESS1 );
            CString sIP;
            ipAddrCtrl.GetWindowText( sIP );
            char* sAnsiIP = UnicodeToAnsi( sIP );
            CEdit editPort = GetDlgItem( IDC_EDIT5 );
            CString sPort;
            editPort.GetWindowText( sPort );
            m_pSocketHandler = new CNewSocketHandler( m_pSocketData->s, sAnsiIP, _ttoi( sPort ), ( const char* )&m_pSocketData->lpData, m_pSocketData->dwDataLen );
        }
        else if ( SEND_BY_RAWSOCKET == m_sendType )
        {
            m_pSocketHandler = new COpenSocketHandler( &m_pSocketData->WSAProtocloInfo, ansiData, iDatalen );
        }
        
        if ( NULL != m_pSocketHandler )
        {
            if ( !m_pSocketHandler->HandleRequest() )
            {
                MessageBox( _T( "发送失败" ), _T( "发生错误" ), MB_OK );
            }
        }
        
        delete ansiData;
        ansiData = NULL;
        return 0;
    }
    
    void CloseDlg()
    {
        if ( NULL != m_pSocketHandler )
        {
            delete m_pSocketHandler;
            m_pSocketHandler = NULL;
        }
        
        EndDialog( IDCANCEL );
    }
    
    LRESULT OnQuit( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CloseDlg();
        return 0;
    }
};