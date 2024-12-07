#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;

volatile sig_atomic_t wasSigHup = 0;

// ф-я обработчика сигнала
void sigHupHandler(int r);

int main()
{
    // регистрация обработчика сигнала
    struct sigaction sa;
    sa.sa_handler = sigHupHandler;
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
    {
        cout << "SIGHUP failed" << endl;
        exit(EXIT_FAILURE);
    }

    // сокет для приема 
    int message_sock = -1, client_sock;

    // сервер для сокета
    int server_sock, port = 8080;
    struct sockaddr_in addr;
    socklen_t size_addr = sizeof(addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cout << "bind failed" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) < 0)
    {
        cout << "Listen failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Server is listening on port " << port << endl;

    // РАБОТА ОСНОВНОГО ЦИКЛА
    fd_set read_fds;		                // дескриптор для данных				
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &read_fds);     	//сервер_сокет адд в группу


    //БЛОКИРОВКА СИГНАЛА
    sigset_t blocked_mask, mask;
    sigemptyset(&blocked_mask);
    sigemptyset(&mask);
    sigaddset(&blocked_mask, SIGHUP);
    sigprocmask(SIG_BLOCK, &blocked_mask, &mask);

    struct timespec tv;	                    // ожидание данных при вызове pselect
    tv.tv_sec = 10;
    tv.tv_nsec = 0;

    while (true)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);     	//адд server_socket в группу

        if (wasSigHup == 1)
        {
            if (message_sock != -1)
            {
                close(message_sock);
                message_sock = -1;
                wasSigHup = 0;
            }
        }

        int result = pselect(server_sock + 1, &read_fds, NULL, NULL, &tv, &mask);

        if (result == -1)
        {
            cout << "PROBLEM with pselect" << endl;
        }
        else if (result)
        {
            if (FD_ISSET(server_sock, &read_fds))
            {
                client_sock = accept(server_sock, (struct sockaddr*)&addr, &size_addr);
                if (client_sock < 0)
                {
                    cout << "accept Failed" << endl;
                    continue;
                }
                cout << "Connection accepted" << endl;


                // 1. Оставляем одно соединение принятым, другие закрываем
                if (message_sock == -1)
                    message_sock = client_sock;
                else
                    close(client_sock);

                // 2. При появлении любых данных вывод мессаге
                char buff[1024];
                size_t bytes = recv(message_sock, buff, sizeof(buff), 0);

                if (bytes > 0)
                {
                    buff[bytes] = '\0';
                    cout << "get bytes: " << bytes << endl;
                }
                else if (bytes == 0)
                {
                    cout << "get 0 bytes." << endl;
                    raise(SIGHUP);
                }
                else
                {
                    cout << "PROBLEM! with get data" << endl;
                }
            }
        }
        else
        {
            cout << "No data" << endl;
        }
    }

    return 0;
}


void sigHupHandler(int r)
{
    cout << "get SIGHUP signal" << endl;
    wasSigHup = 1;
    exit(EXIT_FAILURE);
}









