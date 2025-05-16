#include <iostream>
#include <cassert>

#include "networkmanager.hpp"

ServerManager::ServerManager() {
    serverSocket.setBlocking(false);
}

ServerManager::~ServerManager() {}

void ServerManager::startServer() {
    if (!isServer) {
        if (serverSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
            std::cerr << "Failed to bind server socket to port" << std::endl;
        }
        serverPort = serverSocket.getLocalPort();
        serverAddress = sf::IpAddress::getLocalAddress();
        isServer = true;

        std::cout << "Your local IP address is " << serverAddress << std::endl;
        std::cout << "Server started on port " << serverPort << std::endl;

        PlayerInfo host = {serverAddress, serverPort, 0};
        players.push_back(host);
    }
}

void ClientManager::sendConnectionReq() { // TODO проверки
    std::cout << "Enter server address: ";
    std::cin >> serverAddress;
    std::cout << "Enter server port: ";
    std::cin >> serverPort;
    
    sf::Packet packet;
    packet << ConnectionRequest;

    std::cout << "Sending connection request to " << serverAddress << " using port " << serverPort << std::endl;

    if (clientSocket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
        std::cerr << "Failed to send connection request" << std::endl;
    }
}

void ServerManager::handleNetworkInput(PlayerInputPacket &input) {
    sf::Packet inputPacket;
    sf::Packet outputPacket;
    PacketType packetType;
    sf::IpAddress clientAddress;
    unsigned short clientPort;

    if (serverSocket.receive(inputPacket, clientAddress, clientPort) == sf::Socket::Done) {
        inputPacket >> packetType;
        switch (packetType) {
            case ConnectionRequest: {
                PlayerInfo newPlayer = {clientAddress, clientPort, players.size()};
                std::cout << "New player!" << std::endl;
                std::cout << "New player address: " << newPlayer.address    << std::endl;
                std::cout << "New player port:    " << newPlayer.port       << std::endl;
                std::cout << "New player game id: " << newPlayer.playerId   << std::endl;
                players.push_back(newPlayer);
                outputPacket << ConnectionAccept;
                serverSocket.send(outputPacket, clientAddress, clientPort);
                break;
            }
            case PlayerInput: {
                inputPacket >> input;
                break;
            }
            default: {
                break;
            }
        }
        
        
    }
}

void ServerManager::disconnect() {
    serverSocket.unbind();
    isServer = false;

    players.clear();
    
    std::cout << "Disconnected" << std::endl;
}

void ClientManager::handleNetworkInput() {
    sf::Packet packet;
    PacketType packetType;

    if (clientSocket.receive(packet, serverAddress, serverPort) == sf::Socket::Done) {
        packet >> packetType;
        switch (packetType) {
            case ConnectionAccept: {
                std::cout << "The server accepted your connection request!" << std::endl;
            }
            default: {
                break;
            }
        }
    }
}

// void ServerManager::sendGameState(const GameStatePacket &state) {
//     if (!isServer_ || !isConnected_ || clientAddress_ == sf::IpAddress::None) return;

//     sf::Packet packet;
//     packet << state;

//     socket_.send(packet, clientAddress_, clientPort_);
// }

// bool ServerManager::receivePlayerInput(PlayerInputPacket &input) {
//     sf::Packet packet;
//     sf::IpAddress senderAddress;
//     unsigned short senderPort;

//     if (socket_.receive(packet, senderAddress, senderPort) == sf::Socket::Done) {
//         if (senderAddress == clientAddress_ && senderPort == clientPort_) {
//             if (packet >> input) {
//                 return true;
//             }
//         }
//     }

//     return false;
// }

// void ClientManager::sendPlayerInput(const PlayerInputPacket &input) {
//     if (!isClient_ || !isConnected_) return;

//     sf::Packet packet;
//     packet << input;

//     if (socket_.send(packet, serverAddress_, serverPort_) != sf::Socket::Done) {
//         std::cerr << "Failed to send player input packet to server" << std::endl;
//     }
// }

// bool ClientManager::receiveGameState(GameStatePacket &state) {
//     if (!isClient_ || !isConnected_) return false;

//     sf::Packet packet;
//     sf::IpAddress senderAddress;
//     unsigned short senderPort;

//     if (socket_.receive(packet, senderAddress, senderPort) == sf::Socket::Done) {
//         if (packet >> state) {
//             return true;
//         }
//     }

//     return false;
// }

ClientManager::ClientManager() {
    
}

ClientManager::~ClientManager() {
    
}
