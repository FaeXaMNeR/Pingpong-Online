#include <iostream>
#include <cassert>
#include <SFML/Graphics.hpp>

#include "networkmanager.hpp"
#include "constants.hpp"

ServerManager::ServerManager() {
    serverSocket.setBlocking(false);
    
    while (serverSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "Failed to bind server socket to port" << std::endl;
    }

    serverAddress = sf::IpAddress::getLocalAddress();
    serverPort = serverSocket.getLocalPort();

    std::cout << "Your local IP address is " << serverAddress << std::endl;
    std::cout << "Server started on port " << serverPort << std::endl;

    PlayerInfo host = {serverAddress, serverPort, 0};
    players.push_back(host);

    for (size_t i = 0; i < NUM_OF_ROOMS; i++) {
        RoomInfo room;
        room.player1Id = -1;
        room.player2Id = -1;
        room.isAvailable = true;
        rooms.push_back(room);
    }
    rooms[0].player1Id = 0;

    font.loadFromFile("pong.ttf");    
    serverInfoText.setFont(font);
    serverInfoText.setFillColor(sf::Color::White);
    serverInfoText.setCharacterSize(WINDOW_Y / 30);
    serverInfoText.setPosition(sf::Vector2f(WINDOW_X / 2 + 20, WINDOW_Y / 60));
    serverInfoText.setString("Server IP: " + serverAddress.toString() + 
                             "\nServer port: " + std::to_string(serverPort));

    networkClock.restart();
}

ServerManager::~ServerManager() {
    sf::Packet packet;
    packet << Goodbye;
    for (size_t i = 1; i < players.size(); i++) {
        serverSocket.send(packet, players[i].address, players[i].port);
    }
    
    serverSocket.unbind();
    serverAddress = sf::IpAddress::None;
    serverPort = 0;
    players.clear();
    
    std::cout << "Disconnected" << std::endl;
}

void ServerManager::handleNetworkInput(PlayerInputPacket &input) {
    sf::Packet inputPacket;
    sf::Packet responsePacket;
    PacketType packetType;
    sf::IpAddress clientAddress;
    unsigned short clientPort;

    if (serverSocket.receive(inputPacket, clientAddress, clientPort) == sf::Socket::Done) {
        inputPacket >> packetType;
        std::cout << packetType << std::endl;
        switch (packetType) {
            case ConnectionRequest: {
                PlayerInfo newPlayer = {clientAddress, clientPort, static_cast<int>(players.size())};
                std::cout << "New player!" << std::endl;
                std::cout << "New player address: " << newPlayer.address    << std::endl;
                std::cout << "New player port:    " << newPlayer.port       << std::endl;
                std::cout << "New player game id: " << newPlayer.playerId   << std::endl;
                players.push_back(newPlayer);

                responsePacket << ConnectionAccept << newPlayer.playerId;
                serverSocket.send(responsePacket, clientAddress, clientPort);

                responsePacket.clear();
                responsePacket << rooms;
                serverSocket.send(responsePacket, clientAddress, clientPort);
                break;
            }
            case PlayerInput: {
                inputPacket >> input;
                break;
            }
            case RoomConnectionReq: {
                int numOfRoom;
                inputPacket >> numOfRoom;

                if (rooms[numOfRoom].isAvailable) {
                    responsePacket << RoomConnectionAccept;

                    if (rooms[numOfRoom].player1Id == -1) {
                        inputPacket >> rooms[numOfRoom].player1Id;
                    } else {
                        inputPacket >> rooms[numOfRoom].player2Id;
                        rooms[numOfRoom].isAvailable = false;
                    }
                } else {
                    responsePacket << RoomConnectionDenied;
                }

                serverSocket.send(responsePacket, clientAddress, clientPort);

                std::cout << "Room connection request!" << std::endl;
                std::cout << "Num of the room: " << numOfRoom << std::endl;
                std::cout << "Player1Id: " << rooms[numOfRoom].player1Id << std::endl;
                std::cout << "Player2Id: " << rooms[numOfRoom].player2Id << std::endl;
                std::cout << "rooms[numOfRoom].isAvailable: " << rooms[numOfRoom].isAvailable << std::endl;
            }
            default: {
                break;
            }
        }
    }
}

void ServerManager::sendGameState(PongState &pongState) {
    sf::Packet packet;
    GameStatePacket gameState;
    static int counter = 0;
    static int PPScounter = 0;
    static sf::Clock clock;
    //std::cout << networkClock.getElapsedTime().asSeconds() << std::endl;
    if (counter >= FRAMES_PER_PACKET) {
        // pongState.ball.move(pongState.velocity * pongState.getDeltaTime());

        // pongState.handleBallCollisions();

        // if (pongState.ball.getPosition().x < 0) {
        //     pongState.gooool(Right);
        // } else if (pongState.ball.getPosition().x > WINDOW_X) {
        //     pongState.gooool(Left);
        // }

        //std::cout << networkClock.getElapsedTime().asSeconds() << std::endl;

        PPScounter++;
        if (clock.getElapsedTime().asSeconds() >= 1) {
            std::cout << "PPS: " << PPScounter << std::endl;

            PPScounter = 0;
            clock.restart();
        }

        gameState.type = GameStateUpdate;
        gameState.ballPos = pongState.ball.getPosition();
        gameState.paddle1Pos = pongState.paddle1.getPosition();
        gameState.paddle2Pos = pongState.paddle2.getPosition();
        gameState.score1 = pongState.intScore1;
        gameState.score2 = pongState.intScore2;

        std::cout << gameState.ballPos.x << " " << gameState.ballPos.y << std::endl;
        std::cout << gameState.paddle1Pos.x << " " << gameState.paddle1Pos.y << std::endl;
        std::cout << gameState.paddle2Pos.x << " " << gameState.paddle2Pos.y << std::endl;
        std::cout << gameState.score1 << std::endl;
        std::cout << gameState.score2 << std::endl;

        packet << gameState;

        for (size_t i = 1; i < players.size(); i++) {
            serverSocket.send(packet, players[i].address, players[i].port);
        }

        counter = 0;
        //networkClock.restart();
    }  
    counter++;  
}

// void ServerManager::runRooms() {
//     for (size_t i = 0; i < rooms.size(); i++) {
//         if (rooms[i].player1Id != -1 && rooms[i].player2Id != -1) {

//         }
//     }
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

//     return false;counter++;
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
    clientSocket.setBlocking(false);
}

ClientManager::~ClientManager() {
    
}

void ClientManager::drawGameState(PongState pongState, sf::RenderWindow &window) {
    pongState.ball.setPosition(gameState.ballPos); 
    pongState.paddle1.setPosition(gameState.paddle1Pos);
    pongState.paddle2.setPosition(gameState.paddle2Pos);
    pongState.intScore1 = gameState.score1;
    pongState.intScore2 = gameState.score2;
    pongState.convertScoreToText();
    
    pongState.draw(window);
}

void ClientManager::sendConnectionReq(sf::RenderWindow &window) { // TODO проверки   
    sf::Text checkTerminalText;
    font.loadFromFile("pong.ttf");
    checkTerminalText.setString("Please, check your terminal");
    checkTerminalText.setFont(font);
    checkTerminalText.setFillColor(sf::Color::White);
    checkTerminalText.setPosition(WINDOW_X / 10, WINDOW_Y / 2 - WINDOW_Y / 15);
    checkTerminalText.setCharacterSize(WINDOW_Y / 15);

    window.clear(sf::Color::Black);
    window.draw(checkTerminalText);
    window.display();
    
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

void ClientManager::handleNetworkInput() {
    sf::Packet inputPacket;
    sf::Packet responsePacket;
    PacketType packetType;
    static int counter = 0;
    static sf::Clock clock;

    if (clientSocket.receive(inputPacket, serverAddress, serverPort) == sf::Socket::Done) {
        inputPacket >> packetType;
        switch (packetType) {
            case GameStateUpdate: {
                inputPacket >> gameState;

                counter++;
                if (clock.getElapsedTime().asSeconds() >= 1) {
                    std::cout << "PPS: " << counter << std::endl;
                    counter = 0;
                    clock.restart();
                }
                std::cout << gameState.ballPos.x << " " << gameState.ballPos.y << std::endl;
                std::cout << gameState.paddle1Pos.x << " " << gameState.paddle1Pos.y << std::endl;
                std::cout << gameState.paddle2Pos.x << " " << gameState.paddle2Pos.y << std::endl;
                std::cout << gameState.score1 << std::endl;
                std::cout << gameState.score2 << std::endl;
                break;
            }
            case ConnectionAccept: {
                inputPacket >> clientId;
                std::cout << "The server accepted your connection request!" << std::endl;
                std::cout << "Your game ID is " << clientId << std::endl;
                break;
            }
            case RoomsInfo: {
                RoomsInfoPacket roomsInformation;
                inputPacket >> roomsInformation;
                std::cout << "Here are all the rooms: " << std::endl;
                for (size_t i = 0; i < roomsInformation.roomAvailability.size(); i++) {
                    if (roomsInformation.roomAvailability[i]) {
                        std::cout << i << ": Available" << std::endl;
                    } else {
                        std::cout << i << ": Occupied" << std::endl;
                    }
                }
                std::cout << "Please, enter number of the room you want to join : ";
                RoomConnectionReqPacket roomConnectionReq;

                std::cin >> roomConnectionReq.numOfRoom;
                while (!roomsInformation.roomAvailability[roomConnectionReq.numOfRoom]) {
                    std::cout << "Sorry, the room is occupied" << std::endl;
                    std::cout << "Please enter number of the room you want to join : ";
                    std::cin >> roomConnectionReq.numOfRoom;
                }

                responsePacket << roomConnectionReq << clientId;
                clientSocket.send(responsePacket, serverAddress, serverPort);
                break;
            }
            case Goodbye: {
                std::cout << "The server stopped working! Thanks for playing!" << std::endl;
                break;
            }
            default: {
                break;
            }
        }
    }
}
