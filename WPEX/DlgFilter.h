#pragma once

class CDlgFilter : public CDialogImpl<CDlgFilter>
{
    CListViewCtrl m_listFilter;
public:
    enum { IDD = IDD_DLGFILTER };
    
    BEGIN_MSG_MAP( CDlgFilter )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    END_MSG_MAP()
    
    CDlgFilter()
    {
    
    }
    
    virtual ~CDlgFilter()
    {
    
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        m_listFilter = GetDlgItem( IDC_LIST1 );
        m_listFilter.SetExtendedListViewStyle( LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, 0 );
        m_listFilter.InsertItem( 0, _T( "Filter 1" ) );
        m_listFilter.InsertItem( 1, _T( "Filter 2" ) );
        m_listFilter.InsertItem( 2, _T( "Filter 3" ) );
        return TRUE;
    }
protected:
private:
};