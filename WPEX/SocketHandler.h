#pragma once
#include <winsock2.h>

class CSocketSendHandler
{
public:
    virtual BOOL HandleRequest() = 0;
};

class COpenSocketHandler : public CSocketSendHandler
{
private:
    WSAPROTOCOL_INFO* m_pPtotocol;
    const char* m_buf;
    int m_iLen;
public:
    COpenSocketHandler( WSAPROTOCOL_INFO* pProtocol, const char* buf, int len, DWORD dwTimeout = 0 )
    {
        m_pPtotocol = pProtocol;
        m_buf = buf;
        m_iLen = len;
    }
    virtual ~COpenSocketHandler()
    {
    
    }
    
    BOOL HandleRequest()
    {
        // 		WSABUF wsaBuf;
        // 		wsaBuf.buf=(char*)m_buf;
        // 		wsaBuf.len=m_iLen;
        // 		DWORD NumberOfBytesSent=0;
        // 		int iRet=WSASend(m_socket,&wsaBuf,1,&NumberOfBytesSent,0,0,0);
        
        // 以上代码也可用
        SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, m_pPtotocol, 0, WSA_FLAG_OVERLAPPED );
        int iRet = send( socket, m_buf, m_iLen, 0 );
        return ( iRet != SOCKET_ERROR );
    }
};

class CNewSocketHandler: public CSocketSendHandler
{
private:
    USHORT m_uPort;
    char m_szIPAddr[30];
    SOCKET m_socket;
    char* m_buf;
    int m_iBufSize;
    SOCKADDR * m_destAddr;
public:
    CNewSocketHandler( SOCKET socket, const char* sIP, USHORT uPort, const char* buf, int bufLen )
    {
        m_socket = socket;
        strcpy_s( m_szIPAddr, 30, sIP );
        m_uPort = uPort;
        m_iBufSize = bufLen;
        m_buf = new char[m_iBufSize];
        memcpy_s( m_buf, m_iBufSize, buf, bufLen );
    }
    
    virtual ~CNewSocketHandler()
    {
    
    }
    
    BOOL HandleRequest()
    {
        sockaddr_in sa ;
        
        sa.sin_family           = AF_INET ;
        
        sa.sin_port             = htons( m_uPort ) ;
        
        sa.sin_addr.S_un.S_addr = inet_addr( m_szIPAddr ) ;
        
        connect( m_socket, ( SOCKADDR * ) &sa, sizeof( sa ) ) ;
        int iRet = send( m_socket, m_buf, m_iBufSize, 0 );
        return FALSE;
    }
};