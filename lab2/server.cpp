#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

volatile sig_atomic_t wasSigHup = 0;

/*ôóíêöèÿ îáðàáîò÷èêà ñèãíàëà*/
void sigHupHandler(int r);

int main()
{
    /*óñòàíîâêà îáðàáîò÷èêà ñèãíàëà*/
    struct sigaction sa;
    sa.sa_handler = sigHupHandler;
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
    {
        printf("Error: SIGHUP failed");
        exit(EXIT_FAILURE);
    }

    /*ñîçäàíèå ñîêåòà äëÿ ïðèžìà */
    int message_sock = -1, client_sock;

    /*ñîçäàíèå ñîêåòà äëÿ ñåðâåðà*/
    int server_sock, port = 8080;
    struct sockaddr_in addr;
    socklen_t size_addr = sizeof(addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Error:  socket failed\n");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Error: bind failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) < 0)
    {
        perror("Error: listen failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", port);


    fd_set read_fds;		                //ôàéëîâûé äåñêðèïòîð äëÿ ñëåæêè çà ïîÿâëåíèåì äàííûõ				
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &read_fds);     	//äîáàâëÿåì server_socket â ãðóïïó

    sigset_t blocked_mask, mask;
    sigemptyset(&blocked_mask);
    sigemptyset(&mask);
    sigaddset(&blocked_mask, SIGHUP);
    sigprocmask(SIG_BLOCK, &blocked_mask, &mask);

    struct timespec tv;	                    // îæèäàíèå äàííûõ ïðè âûçîâå pselect
    tv.tv_sec = 10;
    tv.tv_nsec = 0;

    while (true)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);     	//äîáàâëÿåì server_socket â ãðóïïó

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
            perror("Error in pselect.\n");
        }
        else if (result)
        {
            if (FD_ISSET(server_sock, &read_fds))
            {
                client_sock = accept(server_sock, (struct sockaddr*)&addr, &size_addr);
                if (client_sock < 0)
                {
                    perror("Error: accept failed\n");
                    continue;
                }
                printf("Connection accepted\n");


                // 1. Îñòàâëÿåì îäíî ñîåäèíåíèå ïðèíÿòûì, îñòàëüíûå çàêðûâàåì
                if (message_sock == -1)
                    message_sock = client_sock;
                else
                    close(client_sock);

                // 2. Ïðè ïîÿâëåíèè ëþáûõ äàííûõ âûâîäèì ñîîáùåíèå
                char buf[1024];
                size_t bytes_rec = recv(message_sock, buf, sizeof(buf), 0);

                if (bytes_rec > 0)
                {
                    buf[bytes_rec] = '\0';
                    printf("Received %zu bytes.\n", bytes_rec);
                }
                else if (bytes_rec == 0)
                {
                    printf("Received 0 bytes.\n");
                    raise(SIGHUP);
                }
                else
                    perror("Error: receiving data\n");
            }
        }
        else
            printf("No data within ten second.\n");
    }

    return 0;
}


void sigHupHandler(int r)
{
    if (r == SIGHUP)
    {
        printf("Received SIGHUP signal\n");
        wasSigHup = 1;
    }
    else
    {
        printf("Error: handler failed");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_FAILURE);
}









