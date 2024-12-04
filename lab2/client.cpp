#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
using namespace std;

#define PORT 8080
// dfvdfvfdvdffd
int main()
{
    struct sockaddr_in server_Address;
    int sock = 0;
    string message;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cout << "ERROR WITH SOCKET" << endl;
        exit(EXIT_FAILURE);
    }

    server_Address.sin_family = AF_INET;
    server_Address.sin_port = htons(PORT);

    // Converting address to the binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_Address.sin_addr) <= 0)
    {
        cout << "ERROR WITH ADDRESS" << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&server_Address, sizeof(server_Address)) < 0)
     {
        cout << "Connection failed!!!!!!!!!!!!!!!!!!!" << endl;
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        cout << "Enter the message: ";
        cin >> message;

        send(sock, message.c_str(), sizeof(message), 0);
        cout << "Message was send" << endl;

        message = "";
    }

    close(sock);

    return 0;
}
