#pragma once

static const int LEFTFRAMEWIDTH=270;
static const int FUNCNAMELEN=50;
static const TCHAR* CLIENT_WNDCLASSNAME=_T("WPEX_CLIENT_WNDCLASS_%u");
enum WPEX_STATUS{WPEX_START,WPEX_STOP};

static const int WM_DLLDATA=WM_USER+100;
static const int WM_STARTWPEX=WM_USER+101;
static const int WM_STOPWPEX=WM_USER+102;

struct SOCKETDATA 
{
	DWORD dwPID;
	SOCKET s;
	sockaddr_in srcsockaddr;
	sockaddr_in destsockaddr;
	TCHAR sFuncName[FUNCNAMELEN];
	DWORD dwDataLen;
	BYTE lpData[1];
};