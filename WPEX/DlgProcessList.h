#pragma once
#include <TlHelp32.h>
#include <atlstr.h>

class CDlgProcessList : public CDialogImpl<CDlgProcessList>
{
private:
    CListViewCtrl m_listCtrl;
    DWORD m_dwSelPID;
    CString m_sProName;
public:
    enum {IDD = IDD_DLGPROCESSLIST};
    
    BEGIN_MSG_MAP( CDlgProcessList )
    MESSAGE_HANDLER( WM_CLOSE, OnClose )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    COMMAND_ID_HANDLER( IDC_BUTTON3, OnCancel )
    COMMAND_ID_HANDLER( IDC_BUTTON2, OnSelect )
    COMMAND_ID_HANDLER( IDC_BUTTON1, OnRefresh )
    NOTIFY_HANDLER( IDC_LIST1, NM_CLICK, OnNMClickList1 )
    END_MSG_MAP()
    
    CDlgProcessList()
    {
        m_dwSelPID = -1;
    }
    
    virtual ~CDlgProcessList()
    {
    
    }
    
    DWORD GetSelPID()const
    {
        return m_dwSelPID;
    }
    
    CString GetSelProName()const
    {
        return m_sProName;
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        m_listCtrl = GetDlgItem( IDC_LIST1 );
        m_listCtrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, 0 );
        m_listCtrl.InsertColumn( 0, _T( "进程名称" ), LVCFMT_LEFT, 80 );
        m_listCtrl.InsertColumn( 1, _T( "进程ID" ), LVCFMT_LEFT, 50 );
        
        RefreshProcess( m_listCtrl.m_hWnd );
        return TRUE;
    }
    
    LRESULT OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        EndDialog( 0 );
        return 0;
    }
    
    LRESULT OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        EndDialog( IDCANCEL );
        return 0;
    }
    
    LRESULT OnSelect( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        if ( -1 == m_dwSelPID )
        {
            return 0;
        }
        EndDialog( IDOK );
        return 0;
    }
    
    LRESULT OnRefresh( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        RefreshProcess( m_listCtrl.m_hWnd );
        
        return 0;
    }
    
    void RefreshProcess( HWND hWnd )
    {
        m_listCtrl.DeleteAllItems();
        
        HANDLE hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
        if ( NULL == hSnap )
        {
            return ;
        }
        PROCESSENTRY32 tr;
        tr.dwSize = sizeof( PROCESSENTRY32 );
        if ( Process32First( hSnap, &tr ) )
        {
            do
            {
                CString sName = tr.szExeFile;
                int iIndex = m_listCtrl.GetItemCount();
                int iCurIndex = m_listCtrl.InsertItem( iIndex, sName );
                
                CString sPID;
                sPID.Format( _T( "%u" ), tr.th32ProcessID );
                m_listCtrl.SetItemText( iCurIndex, 1, sPID );
            }
            while ( Process32Next( hSnap, &tr ) );
        }
        CloseHandle( hSnap );
    }
    
    LRESULT OnNMClickList1( int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/ )
    {
        int selIndex = m_listCtrl.GetSelectedIndex();
        
        if ( -1 != selIndex )
        {
            TCHAR sPName[100] = {0};
            m_listCtrl.GetItemText( selIndex, 0, sPName, 100 );
            m_sProName = sPName;
            
            TCHAR sPID[50] = {0};
            m_listCtrl.GetItemText( selIndex, 1, sPID, 50 );
            m_dwSelPID = _ttol( sPID );
        }
        return 0;
    }
    
protected:
private:
};