#pragma once

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "pingpong.hpp"

enum PacketType {
    GameStateUpdate,
    PlayerInput,

    ConnectionRequest,
    ConnectionAccept,

    RoomConnectionReq,
    RoomConnectionAccept,
    RoomConnectionDenied,
    RoomsInfo,

    Goodbye
};

struct PlayerInfo {
    sf::IpAddress address;
    unsigned short port;
    int playerId;
    bool moveUp;
    bool moveDown;
};

struct RoomInfo {
    PongState roomGameState;
    int player1Id;
    int player2Id;
    bool isAvailable;
};

struct GameStatePacket {
    PacketType type = GameStateUpdate;
    sf::Vector2f ballPos;
    sf::Vector2f paddle1Pos;
    sf::Vector2f paddle2Pos;
    int score1;
    int score2;
};

struct PlayerInputPacket {
    PacketType type = PlayerInput;
    bool moveUp;
    bool moveDown;
    int playerId;
};

struct RoomsInfoPacket {
    PacketType type = RoomsInfo;
    std::vector<bool> roomAvailability;
};

struct RoomConnectionReqPacket {
    PacketType type = RoomConnectionReq;
    int roomId;
};

class ServerManager {
    public:
        ServerManager();
        ~ServerManager();
        
        void handleNetworkInput();
        void respondToConnectionReq(PlayerInfo &playerInfo);
        void handleRoomConnectionReq(int roomId, int playerId);
        void handleClientDisconnection(int roomId, int clientId);

        void runRooms();

        void sendGameState();

        void drawGameState(sf::RenderWindow &window) {
            rooms[0].roomGameState.draw(window);
            window.draw(serverInfoText);
        }

    private:
        sf::UdpSocket serverSocket;
        sf::IpAddress serverAddress;
        unsigned short serverPort;

        std::vector<PlayerInfo> players;
        std::vector<RoomInfo> rooms;
        
        sf::Clock networkClock;

        sf::Text serverInfoText;
        sf::Font font;
};

class ClientManager {
    public:
        ClientManager();
        ~ClientManager();

        void sendConnectionReq(sf::RenderWindow &window);
        void handleNetworkInput();

        void sendPlayerInput();

        void drawGameState(PongState pongState, sf::RenderWindow &window);
    private:

        sf::UdpSocket clientSocket;

        sf::IpAddress clientAddress;
        unsigned short clientPort;
        int clientId;
        int roomId;

        sf::IpAddress serverAddress;
        unsigned short serverPort;

        GameStatePacket gameState;

        sf::Font font;
};

inline sf::Packet& operator >> (sf::Packet &packet, PacketType &type) {
    sf::Int32 typeAsInt;
    if (packet >> typeAsInt) {
        type = static_cast<PacketType>(typeAsInt);
    }

    return packet;
}

inline sf::Packet& operator << (sf::Packet &packet, const PacketType &type) {
    return packet << static_cast<sf::Int32>(type);
}

inline sf::Packet& operator << (sf::Packet &packet, const GameStatePacket &state) {
    return packet   << state.type
                    << state.ballPos.x << state.ballPos.y
                    << state.paddle1Pos.x << state.paddle1Pos.y
                    << state.paddle2Pos.x << state.paddle2Pos.y
                    << state.score1 << state.score2;
}

inline sf::Packet& operator >> (sf::Packet &packet, GameStatePacket &state) {
    packet  >> state.ballPos.x >> state.ballPos.y
            >> state.paddle1Pos.x >> state.paddle1Pos.y
            >> state.paddle2Pos.x >> state.paddle2Pos.y
            >> state.score1 >> state.score2;

    return packet;
}

inline sf::Packet& operator << (sf::Packet &packet, const PlayerInputPacket &input) {
    return packet   << input.type 
                    << input.moveUp << input.moveDown
                    << input.playerId;
}

inline sf::Packet &operator >> (sf::Packet &packet, PlayerInputPacket &input) {
    packet  >> input.moveUp >> input.moveDown
            >> input.playerId;

    return packet;
}

inline sf::Packet& operator >> (sf::Packet &packet, RoomsInfoPacket &roomsInfo) {
    bool temp;
    while(!packet.endOfPacket()) {
        packet >> temp;
        roomsInfo.roomAvailability.push_back(temp);
    }

    return packet;
}

inline sf::Packet& operator << (sf::Packet &packet, std::vector<RoomInfo> rooms) {
    packet << RoomsInfo;
    for (size_t i = 0; i < rooms.size(); i++) {
        packet << rooms[i].isAvailable;
    }

    return packet;
}

inline sf::Packet& operator << (sf::Packet &packet, const RoomConnectionReqPacket &roomConnectionReq) {
    packet  << roomConnectionReq.type
            << roomConnectionReq.roomId;
    
    return packet;
}
