#include <iostream>

#include "networkmanager.h"

NetworkManager::NetworkManager() {
    socket_.setBlocking(false);
}

NetworkManager::~NetworkManager() {
    disconnect();
}

bool NetworkManager::startServer(unsigned short port) {
    if (socket_.bind(port) != sf::Socket::Done) {
        std::cerr << "Failed to bind server socket to port " << port << std::endl;
        return false;
    }

    isServer_ = true;
    isConnected_ = true;

    clientAddress_ = sf::IpAddress::None;
    clientPort_ = 0;

    std::cout << "Server started on port " << port << std::endl;

    return true;
}

bool NetworkManager::connectClient(const sf::IpAddress &address, unsigned short port) {
    serverAddress_ = address;
    serverPort_ = port;

    isClient_ = true;
    isConnected_ = true;

    std::cout << "Client connecting to " << address << ":" << port << std::endl;
    return true;
}

void NetworkManager::disconnect() {
    socket_.unbind();
    isServer_ = false;
    isClient_ = false;
    isConnected_ = false;

    serverAddress_ = sf::IpAddress::None;
    serverPort_ = 0;

    clientAddress_ = sf::IpAddress::None;
    clientPort_ = 0;
    
    std::cout << "Disconnected" << std::endl;
}

void NetworkManager::sendGameState(const GameStatePacket &state) {
    if (!isServer_ || !isConnected_ || clientAddress_ == sf::IpAddress::None) return;

    sf::Packet packet;
    packet << state;

    socket_.send(packet, clientAddress_, clientPort_);
}

bool NetworkManager::receivePlayerInput(PlayerInputPacket &input) {
    if (!isServer_ || !isConnected_) return false;

    sf::Packet packet;
    sf::IpAddress senderAddress;
    unsigned short senderPort;

    if (socket_.receive(packet, senderAddress, senderPort) == sf::Socket::Done) {
        if (clientAddress_ == sf::IpAddress::None) {
            clientAddress_ = senderAddress;
            clientPort_ = senderPort;

            std::cout << "Client connected from " << senderAddress << ":" << senderPort << std::endl;
        }

        if (senderAddress == clientAddress_ && senderPort == clientPort_) {
            if (packet >> input) {
                return true;
            }
        }
    }

    return false;
}

void NetworkManager::sendPlayerInput(const PlayerInputPacket &input) {
    if (!isClient_ || !isConnected_) return;

    sf::Packet packet;
    packet << input;

    if (socket_.send(packet, serverAddress_, serverPort_) != sf::Socket::Done) {
        std::cerr << "Failed to send player input packet to server" << std::endl;
    }
}

bool NetworkManager::receiveGameState(GameStatePacket &state) {
    if (!isClient_ || !isConnected_) return false;

    sf::Packet packet;
    sf::IpAddress senderAddress;
    unsigned short senderPort;

    if (socket_.receive(packet, senderAddress, senderPort) == sf::Socket::Done) {
        if (packet >> state) {
            return true;
        }
    }

    return false;
}