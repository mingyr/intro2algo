#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <shellapi.h>

#include "common.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "Shell32.lib")


#define DEFAULT_BUFLEN 512
#define BUFSIZE 1024
#define DEFAULT_PORT "27015"


LPCTSTR ErrorMessage(DWORD error)
{
    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

    return((LPCTSTR)lpMsgBuf);
}

//  PrintError support function.
//  Simple wrapper function for error output.
void PrintError(LPCTSTR errDesc)
{
    LPCTSTR errMsg = ErrorMessage(GetLastError());
    _tprintf(TEXT("\n** ERROR ** %s: %s\n"), errDesc, errMsg);
    LocalFree((LPVOID)errMsg);
}

int proto_server_file(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    HANDLE hTempFile = INVALID_HANDLE_VALUE;

    BOOL fSuccess = FALSE;
    DWORD dwRetVal = 0;
    UINT uRetVal = 0;

    DWORD dwBytesRead = 0;
    DWORD dwBytesWritten = 0;

    TCHAR szTempFileName[MAX_PATH];
    TCHAR lpTempPathBuffer[MAX_PATH];
    char  chBuffer[BUFSIZE];

    LPCTSTR errMsg;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Retrieve the file name
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        printf("Bytes received: %d\n", iResult);

        // Create tmp file to hold the content of the file

        //  Gets the temp path env string (no guarantee it's a valid path).
        dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
        if (dwRetVal > MAX_PATH || (dwRetVal == 0))
        {
            PrintError(TEXT("GetTempPath failed"));
            return (2);
        }

        //  Generates a temporary file name. 
        recvbuf[iResult] = '\0';
        wstring&& recvFileName = utf8_decode(recvbuf);

        memset(szTempFileName, 0, sizeof(szTempFileName));
        _sntprintf_s(szTempFileName, _countof(szTempFileName), _T("%s%s"), lpTempPathBuffer, recvFileName.c_str());
        _tprintf(_T("Create TMP file: %s\n"), szTempFileName);

        //  Creates the new file to write to for the upper-case version.
        hTempFile = CreateFile((LPTSTR)szTempFileName, GENERIC_WRITE, 0, NULL, 
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
        if (hTempFile == INVALID_HANDLE_VALUE)
        {
            PrintError(TEXT("Second CreateFile failed"));
            return (4);
        }


        iSendResult = send(ClientSocket, recvbuf, iResult, 0);
        if (iSendResult == SOCKET_ERROR) 
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        printf("Bytes sent: %d\n", iSendResult);
    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else 
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // Echo the buffer back to the sender

                    // Write to the buffer
            uRetVal = WriteFile(hTempFile, recvbuf, iResult, NULL, NULL);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            CloseHandle(hTempFile);
            DeleteFile(szTempFileName);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);


    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    CloseHandle(hTempFile);
    closesocket(ClientSocket);
    WSACleanup();

    _tprintf(_T("Checking transferred file: %s\n"), szTempFileName);

    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = _T("explore");
    ShExecInfo.lpFile = lpTempPathBuffer;
    ShExecInfo.lpParameters = NULL;
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    CloseHandle(ShExecInfo.hProcess);
    // DeleteFile(szTempFileName);

    
    return 0;
}
