// stdafx.cpp : source file that includes just the standard includes
//	WPEX.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)


/*

DWORD dwTlsIndex=-1;
DWORD dwTlsIndex1=-1;
CRITICAL_SECTION g_cs;
CRITICAL_SECTION g_cs1;
LONG volatile g_threadRef=0;
HWND g_NotifyWnd=NULL;
void ProcessAttach(HMODULE hDll)
{
	dwTlsIndex=TlsAlloc();
	dwTlsIndex1=TlsAlloc();
	ThreadAttach(hDll);
	HookWinsockFunc();
	CreateNotifyThread();
}

void ThreadAttach(HMODULE hDll)
{
	if (dwTlsIndex>0)
	{
		TlsSetValue(dwTlsIndex,0);
	}
	if (dwTlsIndex1>0)
	{
		LONG threadRef=InterlockedIncrement(&g_threadRef);
		TlsSetValue(dwTlsIndex1,&threadRef);
	}
}

void HookWinsockFunc()
{

}

static UINT ThreadProc(LPVOID pParam)
{
	const TCHAR* sWndName=_T("WinsockSpy.Send");
	const TCHAR* sWndClassName=_T("WinsockSpy.SendClass");
	HMODULE hDll=GetModuleHandle(NULL);
	WNDCLASS wndClass={0};
	wndClass.hInstance=hDll;
	wndClass.lpszClassName=sWndClassName;
	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL,_T("RegisterClass"),_T("Error"),MB_OK);
	}
	g_NotifyWnd=CreateWindowEx(0,sWndClassName,sWndName,0,0,0,0,0,0,0,hDll,NULL);
	MSG msg;
	while (TRUE)
	{
		if (GetMessage(&msg,0,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CreateNotifyThread()
{
	DWORD dwThreadID=0;
	LPVOID pThreadParam=NULL;
	InitializeCriticalSection(&g_cs);
	InitializeCriticalSection(&g_cs1);
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProc,pThreadParam,0,&dwThreadID);
}

*/