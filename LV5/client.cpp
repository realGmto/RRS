#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

void receiveMessages(int serverSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int n = read(serverSocket, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "Received: " << buffer << std::endl;
        }
        else{
            std::cout << "Error in receiving message" << std::endl;
        }
    }
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error opening socket." << std::endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error connecting to server." << std::endl;
        return -1;
    }

    std::thread receiver(receiveMessages, serverSocket);  // Listen for incoming messages asynchronously
    receiver.detach();

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        
        if (command == "exit") {
            break;
        }

        send(serverSocket, command.c_str(), command.size(), 0);
    }

    close(serverSocket);
    return 0;
}
