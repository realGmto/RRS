#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

std::map<std::string, std::vector<int>> topicSubscribers;
std::mutex mtx;

void handleClient(int clientSocket, int clientID) {
    char buffer[BUFFER_SIZE];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            std::cout << "Client " << clientID << " disconnected." << std::endl;
            break;
        }

        std::string command(buffer);

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "SUBSCRIBE") {
            std::string topic;
            iss >> topic;
            
            std::lock_guard<std::mutex> lock(mtx);
            topicSubscribers[topic].push_back(clientID);
            std::cout << "Client " << clientID << " subscribed to " << topic << std::endl;
            std::string message = "Successfuly subscribed to: " + topic;
            send(clientID, message.c_str(), message.length(), 0);
        } 
        else if (cmd == "PUBLISH") {
            std::string topic, message;
            iss >> topic;
            std::getline(iss, message);

            std::lock_guard<std::mutex> lock(mtx);
            if (topicSubscribers.find(topic) != topicSubscribers.end()) {
                for (int subscriberID : topicSubscribers[topic]) {
                    std::cout << "Sending message to client " << subscriberID << ": " << message << std::endl;

                    std::string sending_message = "Successfully published message(" + topic + "): " + message;
                    send(clientID, sending_message.c_str(), sending_message.length(), 0);
                }
            } else {
                std::cout << "No subscribers for topic " << topic << std::endl;
            }
        }
        else if (cmd == "UNSUBSCRIBE") {
            std::string topic;
            iss >> topic;

            std::lock_guard<std::mutex> lock(mtx);
            topicSubscribers[topic].erase(std::find(topicSubscribers[topic].begin(), topicSubscribers[topic].end(), clientID));
            std::cout << "Client " << clientID << " unsubscribed from " << topic << std::endl;

            std::string message = "Successfully unsubscribed from: " + topic;
            send(clientID, message.c_str(), message.length(), 0);
        }
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error opening socket." << std::endl;
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket." << std::endl;
        return;
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port " << port << "..." << std::endl;

    int clientID = 0;
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        std::thread(handleClient, clientSocket, clientID++).detach();
    }
    return 0;
}
