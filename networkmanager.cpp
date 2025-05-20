#include <iostream>
#include <cassert>
#include <SFML/Graphics.hpp>

#include "networkmanager.hpp"
#include "constants.hpp"

ServerManager::ServerManager() {
    while (serverSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "Failed to bind server socket to port" << std::endl;
    }
    
    serverSocket.setBlocking(false);

    serverAddress = sf::IpAddress::getLocalAddress();
    serverPort = serverSocket.getLocalPort();

    std::cout << "Your local IP address is " << serverAddress << std::endl;
    std::cout << "Server started on port " << serverPort << std::endl;

    PlayerInfo host = {serverAddress, serverPort, 0, false, false};
    players.push_back(host);

    for (size_t i = 0; i < NUM_OF_ROOMS; i++) {
        RoomInfo room;
        room.player1Id = -1;
        room.player2Id = -1;
        room.isAvailable = true;
        room.roomGameState.reset();
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
    rooms.clear();
    serverInfoText.setString("");

    std::cout << "Disconnected" << std::endl;
}

void ServerManager::handleNetworkInput() {
    sf::Packet inputPacket;
    sf::Packet responsePacket;
    PacketType packetType;
    sf::IpAddress clientAddress;
    unsigned short clientPort;

    for (size_t i = 0; i < PACKETS_PER_FRAME * players.size(); i++) {
        if (serverSocket.receive(inputPacket, clientAddress, clientPort) == sf::Socket::Done) {
            inputPacket >> packetType;
            std::cout << packetType << std::endl;
            switch (packetType) {
                case ConnectionRequest: {
                    PlayerInfo newPlayer = {clientAddress, clientPort, static_cast<int>(players.size()), false, false};
                    std::cout << "New player!" << std::endl;
                    std::cout << "New player address: " << newPlayer.address    << std::endl;
                    std::cout << "New player port:    " << newPlayer.port       << std::endl;
                    std::cout << "New player game id: " << newPlayer.playerId   << std::endl;
                    players.push_back(newPlayer);

                    respondToConnectionReq(newPlayer);
                    break;
                }
                case PlayerInput: {
                    PlayerInputPacket input;
                    inputPacket >> input;
                    players[input.playerId].moveUp = input.moveUp;
                    players[input.playerId].moveDown = input.moveDown;

                    std::cout << "Player Input packet!!!!" << std::endl;
                    std::cout << "Player up : " << input.moveUp << std::endl;
                    std::cout << "Player down : " << input.moveDown << std::endl;

                    break;
                }
                case RoomConnectionReq: {
                    int roomId;
                    int playerId;
                    inputPacket >> roomId >> playerId;

                    handleRoomConnectionReq(roomId, playerId);

                    break;
                }
                case Goodbye: {
                    int roomId;
                    int clientId;
                    inputPacket >> roomId >> clientId;

                    handleClientDisconnection(roomId, clientId);
                    
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void ServerManager::respondToConnectionReq(PlayerInfo &playerInfo) {
    sf::Packet responsePacket;

    responsePacket << ConnectionAccept << playerInfo.playerId;
    serverSocket.send(responsePacket, playerInfo.address, playerInfo.port);

    responsePacket.clear();
    responsePacket << rooms;
    serverSocket.send(responsePacket, playerInfo.address, playerInfo.port);
}

void ServerManager::handleRoomConnectionReq(int roomId, int playerId) {
    sf::Packet responsePacket;
    if (rooms[roomId].isAvailable) {
        responsePacket << RoomConnectionAccept << roomId;

        if (rooms[roomId].player1Id == -1) {
            rooms[roomId].player1Id = playerId;
        } else {
            rooms[roomId].player2Id = playerId;
        }
        if (rooms[roomId].player1Id != -1 && rooms[roomId].player2Id != -1) {
            rooms[roomId].isAvailable = false;
        }
    } else {
        responsePacket << RoomConnectionDenied;
    }

    serverSocket.send(responsePacket, players[playerId].address, players[playerId].port);

    std::cout << "Room connection request!" << std::endl;
    std::cout << "Num of the room: " << roomId << std::endl;
    std::cout << "Player1Id: " << rooms[roomId].player1Id << std::endl;
    std::cout << "Player2Id: " << rooms[roomId].player2Id << std::endl;
    std::cout << "rooms[roomId].isAvailable: " << rooms[roomId].isAvailable << std::endl;
}

void ServerManager::handleClientDisconnection(int roomId, int clientId){
    if (!rooms[roomId].isAvailable) {
        rooms[roomId].isAvailable = true;
    } 
    if (rooms[roomId].player1Id == clientId) {
        rooms[roomId].player1Id = -1;
    } else {
        rooms[roomId].player2Id = -1;
    } 

    std::cout << "Goodbye from clientId " << clientId << " in room " << roomId << std::endl;
    std::cout << "Num of the room: " << roomId << std::endl;
    std::cout << "Player1Id: " << rooms[roomId].player1Id << std::endl;
    std::cout << "Player2Id: " << rooms[roomId].player2Id << std::endl;
    std::cout << "rooms[roomId].isAvailable: " << rooms[roomId].isAvailable << std::endl;
}

void ServerManager::sendGameState() {
    sf::Packet packet;
    GameStatePacket gameState;

    for (size_t i = 0; i < rooms.size(); i++) {
        if (!rooms[i].isAvailable) {
            gameState.type = GameStateUpdate;
            gameState.ballPos = rooms[i].roomGameState.ball.getPosition();
            gameState.paddle1Pos = rooms[i].roomGameState.paddle1.getPosition();
            gameState.paddle2Pos = rooms[i].roomGameState.paddle2.getPosition();
            gameState.score1 = rooms[i].roomGameState.intScore1;
            gameState.score2 = rooms[i].roomGameState.intScore2;
            packet << gameState;

            if (rooms[i].player1Id != 0) {
                serverSocket.send(packet, players[rooms[i].player1Id].address, players[rooms[i].player1Id].port);
            }
            serverSocket.send(packet, players[rooms[i].player2Id].address, players[rooms[i].player2Id].port); 
        }
        packet.clear();
    }    
}

void ServerManager::runRooms() {
    players[0].moveUp   = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    players[0].moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    for (size_t i = 0; i < rooms.size(); i++) {
        if (!rooms[i].isAvailable) {
            rooms[i].roomGameState.moveBall();

            rooms[i].roomGameState.handleBallCollisions();

            if (players[rooms[i].player1Id].moveUp && 
                !(rooms[i].roomGameState.paddle1.getGlobalBounds().intersects(rooms[i].roomGameState.topBorder.getGlobalBounds()))) {
                    rooms[i].roomGameState.paddle1.move(sf::Vector2f(0, -(PADDLE_X / 2)));
            }

            if (players[rooms[i].player1Id].moveDown && 
                    !(rooms[i].roomGameState.paddle1.getGlobalBounds().intersects(rooms[i].roomGameState.botBorder.getGlobalBounds()))) {
                    rooms[i].roomGameState.paddle1.move(sf::Vector2f(0, PADDLE_X / 2));
            }

            if (players[rooms[i].player2Id].moveUp && 
                !(rooms[i].roomGameState.paddle2.getGlobalBounds().intersects(rooms[i].roomGameState.topBorder.getGlobalBounds()))) {
                    rooms[i].roomGameState.paddle2.move(sf::Vector2f(0, -(PADDLE_X / 2)));
            }

            if (players[rooms[i].player2Id].moveDown && 
                !(rooms[i].roomGameState.paddle2.getGlobalBounds().intersects(rooms[i].roomGameState.botBorder.getGlobalBounds()))) {
                    rooms[i].roomGameState.paddle2.move(sf::Vector2f(0, PADDLE_X / 2));
            }

            if (rooms[i].roomGameState.ball.getPosition().x < 0) {
                rooms[i].roomGameState.gooool(Right);
            } else if (rooms[i].roomGameState.ball.getPosition().x > WINDOW_X) {
                rooms[i].roomGameState.gooool(Left);
            }
        }
    }
}

ClientManager::ClientManager() {
    while (clientSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cerr << "Failed to bind server socket to port" << std::endl;
    }
    
    clientSocket.setBlocking(false);
}

ClientManager::~ClientManager() {
    sf::Packet packet;
    packet << Goodbye << roomId << clientId;

    clientSocket.send(packet, serverAddress, serverPort);
    
    clientSocket.unbind();
    serverAddress = sf::IpAddress::None;
    serverPort = 0;
    clientAddress = sf::IpAddress::None;
    clientPort = 0;

    std::cout << "Disconnected" << std::endl;
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

    std::cout << serverAddress << std::endl;
    std::cout << serverPort << std::endl;
}

void ClientManager::handleNetworkInput() {
    sf::Packet inputPacket;
    sf::Packet responsePacket;
    sf::IpAddress tempAddress;
    unsigned short tempPort;
    PacketType packetType;
    std::cout << "handleNetworkInput" << std::endl;
    std::cout << serverAddress << std::endl;
    std::cout << serverPort << std::endl;

    for (size_t i = 0; i < PACKETS_PER_FRAME; i++) {
        if (clientSocket.receive(inputPacket, tempAddress, tempPort) == sf::Socket::Done) {
            inputPacket >> packetType;
            switch (packetType) {
                case GameStateUpdate: {
                    inputPacket >> gameState;
                    // std::cout << gameState.ballPos.x << " " << gameState.ballPos.y << std::endl;
                    // std::cout << gameState.paddle1Pos.x << " " << gameState.paddle1Pos.y << std::endl;
                    // std::cout << gameState.paddle2Pos.x << " " << gameState.paddle2Pos.y << std::endl;
                    // std::cout << gameState.score1 << std::endl;
                    // std::cout << gameState.score2 << std::endl;
                    break;
                }
                case ConnectionAccept: {
                    std::cout << "Connection accept" << std::endl;
                    std::cout << serverAddress << std::endl;
                    std::cout << serverPort << std::endl;
                    inputPacket >> clientId;
                    std::cout << "The server accepted your connection request!" << std::endl;
                    std::cout << "Your game ID is " << clientId << std::endl;
                    break;
                }
                case RoomsInfo: {
                    std::cout << "RoomsInfo" << std::endl;
                    std::cout << serverAddress << std::endl;
                    std::cout << serverPort << std::endl;
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

                    std::cin >> roomConnectionReq.roomId;
                    while (!roomsInformation.roomAvailability[roomConnectionReq.roomId]) {
                        std::cout << "Sorry, the room is occupied" << std::endl;
                        std::cout << "Please enter number of the room you want to join : ";
                        std::cin >> roomConnectionReq.roomId;
                    }
                    
                    responsePacket << roomConnectionReq << clientId;
                    clientSocket.send(responsePacket, serverAddress, serverPort);
                    break;
                }
                case RoomConnectionAccept: {
                    inputPacket >> roomId;
                    std::cout << "Connected to room " << roomId << std::endl;
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

    std::cout << serverAddress << std::endl;
    std::cout << serverPort << std::endl;
}

void ClientManager::sendPlayerInput() {
    PlayerInputPacket input;
    sf::Packet packet;

    input.moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    input.moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    input.playerId = clientId;

    packet << input;

    if (clientSocket.send(packet, serverAddress, serverPort) == sf::Socket::Status::Error) {
        // std::cout << packet << std::endl;
        // std::cout << serverAddress << std::endl;
        // std::cout << serverPort << std::endl;
    }
}
    