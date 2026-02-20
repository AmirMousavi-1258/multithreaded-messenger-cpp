#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <cstring>
#include <Windows.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

#define COLOR_RED      12
#define COLOR_GREEN    10
#define COLOR_YELLOW   14

int main() {
    WSADATA wsadata;
    SOCKET server = INVALID_SOCKET;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) == 0) {

        server = socket(AF_INET, SOCK_STREAM, 0);
        
        if (server != INVALID_SOCKET) {
        
            sockaddr_in serveraddr;
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(8888);
            inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);

            if (connect(server, (sockaddr*)&serveraddr, sizeof(serveraddr)) != SOCKET_ERROR) {
                bool Exit = false;
                while (!Exit) {
                    char rec[1024];
                    int recv_size = recv(server, rec, sizeof(rec), 0);
                    if (recv_size > 0) {
                        
                        rec[recv_size] = '\0';
                        //cout << rec[recv_size - 4] << endl;
                        if (rec[recv_size - 1] == ':') {
                            setColor(COLOR_YELLOW);
                            cout << rec << endl;
                            string Answer;
                            getline(cin, Answer);
                            send(server,Answer.c_str(),strlen(Answer.c_str())+1,0 );
                            if ((strcmp(rec, "quit") == 0) || (strcmp(Answer.c_str(), "quit") == 0)) {
                                Exit = true;
                            }
                        }
                        else {
                            setColor(COLOR_GREEN);
                            cout << rec << endl;
                        }
                    }
                    else {
                        setColor(COLOR_RED);
                        cout << "Error when receive!!" << endl;
                        Exit = true;
                    }
                }
            }
            else {
                setColor(COLOR_RED);
                cout << "Error when connect to server!!" << endl;
            }
        }
        else {
            setColor(COLOR_RED);
            cout << "Error when create Socket!!" << endl;
        }
    }
    else {
        setColor(COLOR_RED);
        cout << "Error when Start WSA!!" << endl;
    }
    closesocket(server);
    WSACleanup();

    return 0;
}
