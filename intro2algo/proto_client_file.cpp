#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include <commdlg.h>
#include <shlwapi.h>
#include <mswsock.h>

#include "common.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment (lib, "Comdlg32.lib")
#pragma comment (lib, "Shlwapi.lib")
#pragma comment (lib, "Mswsock.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


LPCTSTR GetFileName(TCHAR *szFile)
{
    OPENFILENAME ofn;       // common dialog box structure
    // HWND hwnd;              // owner window
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 260 * sizeof(TCHAR);
    ofn.lpstrFilter = _T("All\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    else
        return NULL;
}

int proto_client_file(int argc, TCHAR* argv[])
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2) {
        if (sizeof(TCHAR) == sizeof(char))
            printf("usage: %s server-name\n", reinterpret_cast<char*>(argv[0]));
        else
            printf("usage: %S server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port

    string&& name = utf8_encode(argv[1]);
    iResult = getaddrinfo(name.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Get the file name to be sent to the server
    TCHAR szFile[260];       // buffer for file name
    LPCTSTR pFullFileName = GetFileName(szFile);
    if (pFullFileName == NULL)
    {
        MessageBox(NULL, _T("GetFileName() failed"), _T("Error"), MB_OK);
        closesocket(ConnectSocket);
        return 1;
    }

    wstring file_name(PathFindFileName(pFullFileName));
    string&& sendstr = utf8_encode(file_name);

    // Send an initial buffer
    iResult = send(ConnectSocket, &sendstr[0], (int)sendstr.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    int nCount = 0;
    
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        printf("Bytes received: %d\n", iResult);
        nCount += iResult;
    }            
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());


    recvbuf[iResult] = '\0';

    wstring&& recvFileName = utf8_decode(recvbuf);

    if (recvFileName.compare(file_name) != 0)
    {
        MessageBox(NULL, _T("FileName check failed"), _T("Error"), MB_OK);
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    HANDLE hf;
    hf = CreateFile(pFullFileName, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

    BOOL bRet = TransmitFile(ConnectSocket, hf, 0, 0, NULL, NULL, 0);
    if (bRet != TRUE)
    {
        MessageBox(NULL, _T("Error during file transmission"), _T("Error"), MB_OK);
    }

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}