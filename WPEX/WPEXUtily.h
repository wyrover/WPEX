#pragma once
#include <atlstr.h>
#include <TlHelp32.h>
#pragma comment(lib,"Ws2_32.lib")

struct NtCreateThreadExBuffer
{
    ULONG Size;
    ULONG Unknown1;
    ULONG Unknown2;
    PULONG Unknown3;
    ULONG Unknown4;
    ULONG Unknown5;
    ULONG Unknown6;
    PULONG Unknown7;
    ULONG Unknown8;
};

typedef LONG( WINAPI *PFNTCREATETHREADEX )(
    PHANDLE hThread, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter, BOOL CreateSuspended, ULONG StackZeroBits, ULONG SizeOfStackCommit, ULONG SizeOfStackReserve, NtCreateThreadExBuffer* lpBytesBuffer );


CString GetDllPath( )
{
    CString sDllPath;
    TCHAR sModulePath[MAX_PATH] = {0};
    GetModuleFileName( NULL, sModulePath, MAX_PATH );
    CString sModule = sModulePath;
    sModule.Truncate( sModule.ReverseFind( _T( '\\' ) ) + 1 );
    sDllPath = sModule +  _T( "WPEXPatch.dll" );
    return sDllPath;
}


BOOL IsVistaOrLater()
{
    OSVERSIONINFO osvi;
    ZeroMemory( &osvi, sizeof( OSVERSIONINFO ) );
    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &osvi );
    if ( osvi.dwMajorVersion >= 6 )
        return TRUE;
    return FALSE;
}

BOOL MyCreateRemoteThread( HANDLE hProcess, LPTHREAD_START_ROUTINE  lpThreadProc, LPVOID lpDllName )
{
    HANDLE hThread = NULL;
    FARPROC pFunc = NULL;
    if ( IsVistaOrLater() ) //NT6.0以后，有大牛用OD进行调试跟进发现最终调用的事ntdll.NtCreateThreadEx函数进行创建远程线程
    {
        pFunc = GetProcAddress( GetModuleHandle( _T( "ntdll.dll" ) ), "NtCreateThreadEx" );
        if ( pFunc == NULL )
        {
            OutputDebugString( _T( "获取NtCreateThreadEx地址失败!" ) );
            return FALSE;
        }
        __try
        {
            ( ( PFNTCREATETHREADEX )pFunc )( &hThread,
                                             0x1FFFFF,
                                             NULL,
                                             hProcess,
                                             lpThreadProc,
                                             lpDllName,
                                             FALSE,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL );
        }
        __finally
        {
        
        }
        if ( hThread == NULL )
        {
            OutputDebugString( _T( "调用NtCreateThreadEx失败!" ) );
            return FALSE;
        }
        
    }
    else
    {
        hThread = CreateRemoteThread( hProcess, NULL, NULL, lpThreadProc, lpDllName, 0, NULL );
        if ( hThread == NULL )
        {
            return FALSE;
        }
    }
    if ( WAIT_TIMEOUT == WaitForSingleObject( hThread, INFINITE ) )
    {
        OutputDebugString( _T( "等待超时!" ) );
        return FALSE;
    }
    CloseHandle( hThread );
    return TRUE;
}

BOOL EnablePrivilege( LPCTSTR PrivilegeName )
{
    HANDLE tokenhandle;
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenhandle ) )
    {
        return FALSE;
    }
    
    if ( !LookupPrivilegeValue( NULL, PrivilegeName, &tp.Privileges[0].Luid ) )
    {
        CloseHandle( tokenhandle );
        return FALSE;
    }
    
    if ( !AdjustTokenPrivileges( tokenhandle, FALSE, &tp, sizeof( tp ), NULL, NULL ) )
    {
        CloseHandle( tokenhandle );
        return FALSE;
    }
    
    CloseHandle( tokenhandle );
    return TRUE;
}

BOOL InjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
    if ( !PathFileExists( lpszDllName ) )
    {
        return FALSE;
    }
    
    if ( !EnablePrivilege( SE_DEBUG_NAME ) )
    {
        return FALSE;
    }
    
    BOOL bRet = FALSE;
    HANDLE hPro =::OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPID );
    
    if ( NULL == hPro )
    {
        return FALSE;
    }
    
    LPVOID pszLibFileRemote = NULL;
    HANDLE hRemoteThread = NULL;
    
    __try
    {
        int cb  = ( _tcslen( lpszDllName ) + 1 ) * sizeof( TCHAR );
        pszLibFileRemote = VirtualAllocEx( hPro, NULL, cb, MEM_COMMIT, PAGE_READWRITE );
        
        if ( pszLibFileRemote == NULL )
        {
            return FALSE;
        }
        
        if ( !WriteProcessMemory( hPro, pszLibFileRemote, ( PVOID ) lpszDllName, cb, NULL ) )
        {
            return FALSE;
        }
        
#ifdef _UNICODE
#define Libaray ("LoadLibraryW")
#else
#define Libaray ("LoadLibraryA")
#endif
        PTHREAD_START_ROUTINE pfnThreadRtn = ( PTHREAD_START_ROUTINE )GetProcAddress( GetModuleHandle( _T( "Kernel32" ) ), Libaray );
        
        if ( pfnThreadRtn == NULL )
        {
            return FALSE;
        }
        
        //        hRemoteThread = ::CreateRemoteThread( hPro, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL );
        
        BOOL bInject = MyCreateRemoteThread( hPro, pfnThreadRtn, pszLibFileRemote );
        
        if ( bInject == FALSE )
        {
            return FALSE;
        }
        
        WaitForSingleObject( hRemoteThread, INFINITE );
        return TRUE;
    }
    __finally
    {
        if ( hRemoteThread  != NULL )
        {
            CloseHandle( hRemoteThread );
            hRemoteThread = NULL;
        }
        
        if ( pszLibFileRemote != NULL )
        {
            VirtualFreeEx( hPro, pszLibFileRemote, 0, MEM_RELEASE );
            pszLibFileRemote = NULL;
        }
        
        if ( NULL != hPro )
        {
            ::CloseHandle( hPro );
            hPro = NULL;
        }
    }
    
    return FALSE;
}

BOOL UnInjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
    if ( !EnablePrivilege( SE_DEBUG_NAME ) )
    {
        return FALSE;
    }
    
    BOOL bRet = FALSE;
    HANDLE hPro =::OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPID );
    
    if ( NULL == hPro )
    {
        return FALSE;
    }
    
    HANDLE hthSnapshot = NULL;
    HANDLE hThread = NULL;
    
    __try
    {
        // Grab a new snapshot of the process
        hthSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
        
        if ( hthSnapshot == NULL )
        {
            return FALSE;
        }
        
        // Get the HMODULE of the desired library
        MODULEENTRY32 me = { sizeof( me ) };
        BOOL fFound = FALSE;
        BOOL fMoreMods = Module32First( hthSnapshot, &me );
        
        for ( ; fMoreMods; fMoreMods = Module32Next( hthSnapshot, &me ) )
        {
            fFound = ( _tcscmp( me.szModule,  lpszDllName ) == 0 ) ||
                     ( _tcscmp( me.szExePath, lpszDllName ) == 0 );
                     
            if ( fFound ) break;
        }
        
        if ( !fFound )
        {
            return FALSE;
        }
        
        // Get the real address of LoadLibraryW in Kernel32.dll
        PTHREAD_START_ROUTINE pfnThreadRtn = ( PTHREAD_START_ROUTINE )GetProcAddress( GetModuleHandle( _T( "Kernel32" ) ), "FreeLibrary" );
        
        if ( pfnThreadRtn == NULL )
        {
            return FALSE;
        }
        
        // Create a remote thread that calls LoadLibraryW(DLLPathname)
        BOOL bInject = MyCreateRemoteThread( hPro, pfnThreadRtn, me.modBaseAddr );
        if ( bInject == FALSE )
        {
            return FALSE;
        }
        //         hThread = ::CreateRemoteThread( hPro, NULL, 0, pfnThreadRtn, me.modBaseAddr, 0, NULL );
        //
        //         if ( hThread == NULL )
        //         {
        //             return FALSE;
        //         }
        
        // Wait for the remote thread to terminate
        WaitForSingleObject( hThread, INFINITE );
        return TRUE;
    }
    __finally   // Now we can clean everything up
    {
        if ( hThread != NULL )
        {
            CloseHandle( hThread );
            hThread = NULL;
        }
        
        if ( hthSnapshot != NULL )
        {
            CloseHandle( hthSnapshot );
            hThread = NULL;
        }
        
        if ( NULL != hPro )
        {
            ::CloseHandle( hPro );
            hPro = NULL;
        }
    }
    
    return FALSE;
}


wchar_t* AnsiToUnicode( const char* szStr )
{
    int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
    if ( nLen == 0 )
    {
        return NULL;
    }
    wchar_t* pResult = new wchar_t[nLen];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
    return pResult;
}
