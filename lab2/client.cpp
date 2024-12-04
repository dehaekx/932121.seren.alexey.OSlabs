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

int main() {
    struct sockaddr_in server_Address;
    int sock = 0;
    string message;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("ERROR WITH SOCKET\n");
        exit(EXIT_FAILURE);
    }

    server_Address.sin_family = AF_INET;
    server_Address.sin_port = htons(PORT);

    // Converting address to the binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_Address.sin_addr) <= 0) {
        printf("ERROR WITH ADDRESS \n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&server_Address, sizeof(server_Address)) < 0) {
        printf("Connection failed \n");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        cout << "Enter message: ";
        cin >> message;

        send(sock, message.c_str(), sizeof(str), 0);
        printf("Message was send\n");

        message = "";
    }
    close(sock);

    return 0;
}
