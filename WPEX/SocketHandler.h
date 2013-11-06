#pragma once
#include <winsock2.h>

class CSocketSendHandler
{
private:
    SENDMODEL m_sendModel;
    SENDTYPE m_sendType;
    CString m_sIP;
    CString m_sPort;
    CString m_sRawSocketID;
    char* m_packetData;
    DWORD m_dwPacketSize;
    WSAPROTOCOL_INFO* m_pPtotocol;
public:
    CSocketSendHandler()
    {
        m_sendModel = SEND_BY_TIMES;
        m_sendType = SEND_BY_RAWSOCKET;
        m_packetData = NULL;
        m_pPtotocol = NULL;
        m_dwPacketSize = 0;
        
        WSADATA wsaData;
        WSAStartup( 0x202, &wsaData );
    }
    
    virtual ~CSocketSendHandler()
    {
        if ( NULL != m_packetData )
        {
            delete m_packetData;
            m_packetData = NULL;
        }
        
        if ( NULL != m_pPtotocol )
        {
            delete m_pPtotocol;
            m_pPtotocol = NULL;
        }
        
        //绝对不能调用WSACleanup(),会使原始进程的远程主机关闭连接
    }
    
    void SetSendModel( SENDMODEL sendModel )
    {
        m_sendModel = sendModel;
    }
    
    void SendType( SENDTYPE sendType )
    {
        m_sendType = sendType;
    }
    
    void SetIP( CString sIP )
    {
        m_sIP = sIP;
    }
    
    void SetPort( CString sPort )
    {
        m_sPort = sPort;
    }
    
    void SetRawSocketID( CString sRawSocketID )
    {
        m_sRawSocketID = sRawSocketID;
    }
    
    void SetPacket( char* sPacket , DWORD dwPacketSize )
    {
        if ( NULL != m_packetData )
        {
            delete m_packetData;
            m_packetData = NULL;
        }
        if ( dwPacketSize > 0 )
        {
            m_dwPacketSize = dwPacketSize;
            m_packetData = new char[dwPacketSize];
            memcpy_s( m_packetData, dwPacketSize, sPacket, dwPacketSize );
        }
    }
    
    void SetProtocolInfo( WSAPROTOCOL_INFO* pInfo )
    {
        if ( NULL != m_pPtotocol )
        {
            delete m_pPtotocol;
            m_pPtotocol = NULL;
        }
        m_pPtotocol = new WSAPROTOCOL_INFO();
        memcpy_s( m_pPtotocol, sizeof( WSAPROTOCOL_INFO ), pInfo, sizeof( WSAPROTOCOL_INFO ) );
    }
    
    BOOL Send()
    {
        switch ( m_sendType )
        {
            case SEND_BY_NEWSOCKET:
            {
                // 				sockaddr_in sa ;
                //
                // 				sa.sin_family           = AF_INET ;
                //
                // 				sa.sin_port             = htons( m_uPort ) ;
                //
                // 				sa.sin_addr.S_un.S_addr = inet_addr( m_szIPAddr ) ;
                //
                // 				connect( m_socket, ( SOCKADDR * ) &sa, sizeof( sa ) ) ;
                // 				int iRet = send( m_socket, m_buf, m_iBufSize, 0 );
                break;
            }
            case SEND_BY_RAWSOCKET:
            {
                // 		WSABUF wsaBuf;
                // 		wsaBuf.buf=(char*)m_buf;
                // 		wsaBuf.len=m_iLen;
                // 		DWORD NumberOfBytesSent=0;
                // 		int iRet=WSASend(m_socket,&wsaBuf,1,&NumberOfBytesSent,0,0,0);
                
                // 以上代码也可用
                SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, m_pPtotocol, 0, 0 );
                int iRet = send( socket, m_packetData, m_dwPacketSize, 0 );
                return ( iRet != SOCKET_ERROR );
                break;
            }
            default:
                break;
        }
        return FALSE;
    }
};