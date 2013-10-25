#pragma once
#include <windows.h>
#include "mhook.h"
#pragma comment(lib,"mhookLib.lib")
#include <tchar.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include "..\WPEX\CustomDef.h"
#include "..\WPEX\WPEXUtily.h"
#include <tchar.h>
#include "WndNotify.h"


HWND FindClient()
{
    const TCHAR* sClassName = _T( "WPEXSOCKETSPY" );
    HWND hWnd =::FindWindow( sClassName, NULL );
    return hWnd;
}

void SendDataToClient( BOOL bSend, SOCKET s, LPVOID lpData, DWORD dwDataLen, LPCTSTR lpszFuncName )
{
    HWND hClient = FindClient();
    if ( ::IsWindow( hClient ) )
    {
        DWORD dwSocketSize = sizeof( SOCKETDATA ) + dwDataLen - 1;
        SOCKETDATA* pSocketData = ( SOCKETDATA* )new BYTE[dwSocketSize];
        memset( pSocketData, 0, dwSocketSize );
        pSocketData->s = s;
        pSocketData->dwDataLen = dwDataLen;
        memcpy( pSocketData->lpData, lpData, dwDataLen );
        _tcscpy_s( pSocketData->sFuncName, FUNCNAMELEN, lpszFuncName );
        pSocketData->dwPID = GetCurrentProcessId();
        
        int sockSrcAddrLen = sizeof( pSocketData->srcsockaddr );
        int sockDestAddrLen = sizeof( pSocketData->destsockaddr );
        if ( bSend )
        {
            getsockname( s, ( sockaddr* )&pSocketData->srcsockaddr, &sockSrcAddrLen );
            getpeername( s, ( sockaddr* )&pSocketData->destsockaddr, &sockDestAddrLen );
        }
        else
        {
            getpeername( s, ( sockaddr* )&pSocketData->srcsockaddr, &sockSrcAddrLen );
            getsockname( s, ( sockaddr* )&pSocketData->destsockaddr, &sockDestAddrLen );
        }
        
        
        COPYDATASTRUCT cds = { 0 };
        cds.dwData = WM_DLLDATA;
        cds.lpData = pSocketData;
        cds.cbData = dwSocketSize;
        ::SendMessage( hClient, WM_COPYDATA, ( WPARAM )hClient, ( LPARAM )&cds );
        delete pSocketData;
        pSocketData = NULL;
    }
}

typedef int ( WINAPI* _send )( SOCKET s, const char *buf, int len, int flags );
typedef int ( WINAPI* _recv )( SOCKET s, const char *buf, int len, int flags );
typedef int ( WINAPI* _sendto )( SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen );
typedef int ( WINAPI* _recvfrom )( SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen );

typedef int ( WINAPI* _WSASend )( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );
typedef int ( WINAPI* _WSARecv )( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );
typedef int ( WINAPI* _WSASendTo )( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );
typedef int ( WINAPI* _WSARecvFrom )( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );

_send Sys_send = ( _send )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "send" );
_recv Sys_recv = ( _recv )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "recv" );
_sendto Sys_sendto = ( _sendto )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "sendto" );
_recvfrom Sys_recvfrom = ( _recvfrom )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "recvfrom" );

_WSASend Sys_WSASend = ( _WSASend )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "WSASend" );
_WSARecv Sys_WSARecv = ( _WSARecv )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "WSARecv" );
_WSASendTo Sys_WSASendTo = ( _WSASendTo )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "WSASendTo" );
_WSARecvFrom Sys_WSARecvFrom = ( _WSARecvFrom )GetProcAddress( GetModuleHandle( _T( "Ws2_32" ) ), "WSARecvFrom" );

extern CWndNotify m_wndNotify;
int WINAPI HookSend( SOCKET s, const char *buf, int len, int flags )
{
    if ( WPEX_START == m_wndNotify.GetStatus() )
    {
        SendDataToClient( TRUE, s, ( LPVOID )buf, len, _T( "Send" ) );
    }
    
    return Sys_send( s, buf, len, flags );
}

int WINAPI HookRecv( SOCKET s, const char *buf, int len, int flags )
{
    int iRet = Sys_recv( s, buf, len, flags );
	if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( FALSE, s, ( LPVOID )buf, len, _T( "Recv" ) );
	}
    return iRet;
}

int WINAPI HookSendto( SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen )
{
	if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( TRUE, s, ( LPVOID )buf, len, _T( "Sendto" ) );
	}
    return Sys_sendto( s, buf, len, flags, to, tolen );
}

int WINAPI HookRecvfrom( SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen )
{
    int iRet = Sys_recvfrom( s, buf, len, flags, from, fromlen );
    if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( FALSE, s, ( LPVOID )buf, len, _T( "Recvfrom" ) );
	}
    return iRet;
}

int WINAPI HookWSASend( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( TRUE, s, ( LPVOID )lpBuffers->buf, lpBuffers->len, _T( "WSASend" ) );
	}
    return Sys_WSASend( s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine );
}

int WINAPI HookWSARecv( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    int iRet = Sys_WSARecv( s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine );
    if ( WPEX_START == m_wndNotify.GetStatus() )
    {
        SendDataToClient( FALSE, s, ( LPVOID )lpBuffers->buf, lpBuffers->len, _T( "WSARecv" ) );
    }
    return iRet;
}

int WINAPI HookWSASendTo( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( TRUE, s, ( LPVOID )lpBuffers->buf, lpBuffers->len, _T( "WSASendTo" ) );
	}
    return Sys_WSASendTo( s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine );
}

int WINAPI HookWSARecvFrom( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    int iRet = Sys_WSARecvFrom( s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine );
    if ( WPEX_START == m_wndNotify.GetStatus() )
	{
		SendDataToClient( FALSE, s, ( LPVOID )lpBuffers->buf, lpBuffers->len, _T( "WSARecvFrom" ) );
	}
    return iRet;
}


class CPatchUtily
{
public:
    CPatchUtily()
    {
    
    }
    
    virtual ~CPatchUtily()
    {
        UnHookFunc();
    }
    
    BOOL HookFunc()
    {
        if ( ( !Mhook_SetHook( ( PVOID* )&Sys_send, HookSend ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_recv, HookRecv ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_sendto, HookSendto ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_recvfrom, HookRecvfrom ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_WSASend, HookWSASend ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_WSARecv, HookWSARecv ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_WSASendTo, HookWSASendTo ) ) ||
                ( !Mhook_SetHook( ( PVOID* )&Sys_WSARecvFrom, HookWSARecvFrom ) ) )
        {
            return FALSE;
        }
        return TRUE;
    }
    
    BOOL UnHookFunc()
    {
        if ( ( !Mhook_Unhook( ( PVOID* )&Sys_send ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_recv ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_sendto ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_recvfrom ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_WSASend ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_WSARecv ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_WSASendTo ) ) ||
                ( !Mhook_Unhook( ( PVOID* )&Sys_WSARecvFrom ) ) )
        {
            return FALSE;
        }
        return TRUE;
    }
    
    BOOL DisableTLS( HMODULE hDll )
    {
        return DisableThreadLibraryCalls( hDll );
    }
};
