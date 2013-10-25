#pragma once
#include <windows.h>
#include <assert.h>
#include <tchar.h>
WPEX_STATUS g_wpex_Status=WPEX_STOP;
class CWndNotify
{
private:
	WPEX_STATUS m_wpex_Status;

	HANDLE m_hThread;
public:
    CWndNotify()
    {
		g_wpex_Status=WPEX_STOP;
		m_hThread=NULL;
    }
    
    virtual ~CWndNotify()
    {
		TerminateThread();
    }

	static void Start()
	{
		g_wpex_Status=WPEX_START;
	}

	static void Stop()
	{
		g_wpex_Status=WPEX_STOP;
	}

	static WPEX_STATUS GetStatus()
	{
		return g_wpex_Status;
	}

	BOOL CreateThread()
	{
		if (NULL==m_hThread)
		{
			m_hThread=(HANDLE)::_beginthreadex(NULL,0,ThreadProc,this,0,0);
		}
		return (m_hThread!=NULL);
	}

	void TerminateThread()
	{
		DWORD dwRet=::WaitForSingleObject(m_hThread,INFINITE);
		if (WAIT_OBJECT_0!=dwRet)
		{
			::TerminateThread(m_hThread,0);
		}
		m_hThread=NULL;
	}

	static UINT __stdcall ThreadProc(LPVOID pParam)
	{
		CWndNotify* pThis=(CWndNotify*)pParam;
		pThis->Create(GetModuleHandle(NULL));
		return 0;
	}
    
    BOOL Create(HMODULE hDll)
    {
		TCHAR szAppName[MAX_PATH] = {0};
		_stprintf_s(szAppName,MAX_PATH,CLIENT_WNDCLASSNAME,GetCurrentProcessId());
        HWND         hwnd;
        MSG          msg;
        WNDCLASSEX   wndclassex = {0};
        wndclassex.cbSize        = sizeof( WNDCLASSEX );
        wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
        wndclassex.lpfnWndProc   = WndProc;
        wndclassex.cbClsExtra    = 0;
        wndclassex.cbWndExtra    = 0;
        wndclassex.hInstance     = hDll;
        wndclassex.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
        wndclassex.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wndclassex.hbrBackground = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
        wndclassex.lpszMenuName  = NULL;
        wndclassex.lpszClassName = szAppName;
        wndclassex.hIconSm       = wndclassex.hIcon;
        
        if ( !RegisterClassEx( &wndclassex ) )
        {
            MessageBox( NULL, TEXT( "RegisterClassEx failed!" ), szAppName, MB_ICONERROR );
            return 0;
        }
        hwnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                               szAppName,
                               TEXT( "WindowTitle" ),
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               NULL,
                               NULL,
                               hDll,
                               NULL );
                               
        ShowWindow( hwnd, SW_HIDE );
        UpdateWindow( hwnd );
        
        while ( GetMessage( &msg, NULL, 0, 0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        return msg.wParam;
    }
    
	static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch ( message )
		{
		case WM_STARTWPEX:
			{
				Start();
				break;
			}
		case WM_STOPWPEX:
			{
				Stop();
				break;
			}
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return ( 0 );
		}
		return DefWindowProc( hwnd, message, wParam, lParam );
	}
};