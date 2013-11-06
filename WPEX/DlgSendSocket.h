
#pragma once
#include <atlctrls.h>
#include "CustomDef.h"
#include "SocketHandler.h"
#include <assert.h>
class CDlgSendSocket : public CDialogImpl<CDlgSendSocket>
{
private:
    SENDTYPE m_sendType;
    SENDMODEL m_sendModel;
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
        m_pSocketHandler = new CSocketSendHandler();
        m_pSocketData = pSocketData;
        m_sendModel = SEND_BY_TIMES;
        m_sendType = SEND_BY_RAWSOCKET;
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
    void OnSendModel( UINT uRadioID )
    {
        CButton btn = GetDlgItem( uRadioID );
        btn.SetCheck( BST_CHECKED );
        
        CEdit editSendCount = GetDlgItem( IDC_EDIT2 );
        CEdit editSendTime = GetDlgItem( IDC_EDIT3 );
        if ( IDC_RADIO1 == uRadioID )
        {
            m_sendModel = SEND_BY_CONBINUOUSLY;
            editSendCount.EnableWindow( FALSE );
            editSendTime.EnableWindow( FALSE );
        }
        else if ( IDC_RADIO2 == uRadioID )
        {
            m_sendModel = SEND_BY_TIMES;
            editSendCount.EnableWindow( TRUE );
            editSendTime.EnableWindow( TRUE );
        }
    }
    
    void OnSendType( UINT uRadioID )
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
        OnSendModel( IDC_RADIO1 );
        return 0;
    }
    
    // 按次发送
    LRESULT OnRadioSendByTime( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendModel( IDC_RADIO2 );
        return 0;
    }
    
    // 使用原始socket
    LRESULT OnRadioSocketRaw( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendType( IDC_RADIO3 );
        return 0;
    }
    
    // 新建socket
    LRESULT OnRadioSocketNew( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendType( IDC_RADIO4 );
        return 0;
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CenterWindow( GetParent() );
        
        OnSendModel( IDC_RADIO2 );
        OnSendType( IDC_RADIO3 );
        
        CEdit editRawSID = GetDlgItem( IDC_EDIT4 );			// 原始SOCKET ID
        CString sSID;
        sSID.Format( _T( "%u" ), ( DWORD )m_pSocketData->s );
        editRawSID.SetWindowText( sSID );
        
        char* lpDestIP = inet_ntoa( m_pSocketData->destsockaddr.sin_addr );	// 目标地址
        TCHAR* pDestIP = AnsiToUnicode( lpDestIP );
        CString sDestIP = pDestIP;
        CIPAddressCtrl ipAddrCtrl = GetDlgItem( IDC_IPADDRESS1 );
        ipAddrCtrl.SetWindowText( pDestIP );
        delete pDestIP;
        pDestIP = NULL;
        
        USHORT nDestPort = htons( m_pSocketData->destsockaddr.sin_port );	// 目标端口号
        CString sDestPort;
        sDestPort.Format( _T( "%u" ), nDestPort );
        CEdit editPort = GetDlgItem( IDC_EDIT5 );
        editPort.SetWindowText( sDestPort );
        
        char* lpSrcIP = inet_ntoa( m_pSocketData->srcsockaddr.sin_addr );	// 源地址
        TCHAR* pSrcIP = AnsiToUnicode( lpSrcIP );
        CString sSrcIP = pSrcIP;
        delete pSrcIP;
        pSrcIP = NULL;
        
        USHORT nSrcPort = htons( m_pSocketData->srcsockaddr.sin_port );	// 源端口号
        CString sSrcPort;
        sSrcPort.Format( _T( "%u" ), nSrcPort );
        
        CString sTitle;						// 设置窗口标题
        GetWindowText( sTitle );
        if ( m_pSocketData->bSend )
        {
            sTitle.AppendFormat( _T( " %s:%s -> %s:%s" ), sSrcIP, sSrcPort, sDestIP, sDestPort );
        }
        else
            sTitle.AppendFormat( _T( " %s:%s -> %s:%s" ), sDestIP, sDestPort, sSrcIP, sSrcPort );
        SetWindowText( sTitle );
        
        TCHAR* sData = AnsiToUnicode( ( const char* )&m_pSocketData->lpData );	// 数据
        CEdit editData = GetDlgItem( IDC_EDIT1 );
        editData.SetWindowText( sData );
        delete sData;
        sData = NULL;
        
        m_pSocketHandler->SetProtocolInfo( &m_pSocketData->WSAProtocloInfo );
        return TRUE;
    }
    
    LRESULT OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        CloseDlg();
        return 0;
    }
    
    LRESULT OnSend( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CEdit editData = GetDlgItem( IDC_EDIT1 );
        CString sData;
        editData.GetWindowText( sData );
        char* ansiData = UnicodeToAnsi( sData );
        int iDatalen = strlen( ansiData );
        m_pSocketHandler->SetPacket( ansiData, iDatalen );
        delete ansiData;
        ansiData = NULL;
        
        CIPAddressCtrl ipAddrCtrl = GetDlgItem( IDC_IPADDRESS1 );
        CString sIP;
        ipAddrCtrl.GetWindowText( sIP );
        m_pSocketHandler->SetIP( sIP );
        
        CEdit editPort = GetDlgItem( IDC_EDIT5 );
        CString sPort;
        editPort.GetWindowText( sPort );
        m_pSocketHandler->SetPort( sPort );
        
        if ( !m_pSocketHandler->Send() )
        {
            MessageBox( _T( "发送失败" ), _T( "发生错误" ), MB_OK );
        }
        
        return 0;
    }
    
    void CloseDlg()
    {
        EndDialog( IDCANCEL );
    }
    
    LRESULT OnQuit( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CloseDlg();
        return 0;
    }
};