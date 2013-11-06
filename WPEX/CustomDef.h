#pragma once

static const int LEFTFRAMEWIDTH = 270;
static const int FUNCNAMELEN = 50;
static const TCHAR* CLIENT_WNDCLASSNAME = _T( "WPEX_CLIENT_WNDCLASS_%u" );
enum WPEX_STATUS {WPEX_START, WPEX_STOP};

static const int WM_DLLDATA = WM_USER + 100;
static const int WM_STARTWPEX = WM_USER + 101;
static const int WM_STOPWPEX = WM_USER + 102;

struct SOCKETDATA
{
	BOOL bSend;
    DWORD cbSize;
    DWORD dwPID;
    SOCKET s;
    WSAPROTOCOL_INFO WSAProtocloInfo;
    sockaddr_in srcsockaddr;
    sockaddr_in destsockaddr;
    TCHAR sFuncName[FUNCNAMELEN];
    DWORD dwDataLen;
    BYTE lpData[1];
};

enum SENDMODEL
{
	SEND_BY_CONBINUOUSLY,	// 持续发送
	SEND_BY_TIMES			// 按次发送
};

enum SENDTYPE
{
	SEND_BY_RAWSOCKET,		// 使用原始SOCKET发送
	SEND_BY_NEWSOCKET		// 使用新SOCKET发送
};