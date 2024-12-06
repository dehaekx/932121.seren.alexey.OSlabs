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

int main()
{
    struct sockaddr_in server_Address;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    string message;

    server_Address.sin_family = AF_INET;
    server_Address.sin_port = htons(PORT);

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
    return 0;
}
