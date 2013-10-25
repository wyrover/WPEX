// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "PatchUtily.h"
#include "WndNotify.h"

// #pragma data_seg(".SocketSpy")
// HWND g_hWndClient=NULL;
// #pragma data_seg()
// #pragma comment(linker, "/Section:.SocketSpy,RWS")

CPatchUtily patchUtily;
CWndNotify m_wndNotify;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			if (!patchUtily.DisableTLS(hModule))
			{
				return FALSE;
			}
			if (!patchUtily.HookFunc())
			{
				return FALSE;
			}
			if (!m_wndNotify.CreateThread())
			{
				return FALSE;
			}
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		{
			if (!patchUtily.UnHookFunc())
			{
				return FALSE;
			}
		}
		break;
	}
	return TRUE;
}

