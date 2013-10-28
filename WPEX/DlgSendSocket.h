
#pragma once
#include <atlctrls.h>
class CDlgSendSocket : public CDialogImpl<CDlgSendSocket>
{
private:

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
            editSocketID.EnableWindow( TRUE );
            ipAddrCtrl.EnableWindow( FALSE );
            editPort.EnableWindow( FALSE );
        }
        else if ( IDC_RADIO4 == uRadioID )
        {
            editSocketID.EnableWindow( FALSE );
            ipAddrCtrl.EnableWindow( TRUE );
            editPort.EnableWindow( TRUE );
        }
    }
    
    LRESULT OnRadioSendByContinue( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendCtrlStatus( IDC_RADIO1 );
        return 0;
    }
    
    LRESULT OnRadioSendByTime( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSendCtrlStatus( IDC_RADIO2 );
        return 0;
    }
    
    LRESULT OnRadioSocketRaw( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        OnSocketType( IDC_RADIO3 );
        return 0;
    }
    
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
        return TRUE;
    }
    
    LRESULT OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        EndDialog( IDCANCEL );
        return 0;
    }
    
    LRESULT OnSend( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        EndDialog( IDOK );
        return 0;
    }
    
    LRESULT OnQuit( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        EndDialog( IDCANCEL );
        return 0;
    }
};