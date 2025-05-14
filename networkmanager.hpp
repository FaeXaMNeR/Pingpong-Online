#pragma once

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum PacketType {
    GameStateUpdate,
    PlayerInput,

    NewRoomReq,
    RoomConnectionReq,

    ConnectionRequest,
    ConnectionAccept,

    GameStart,
    PlayerDisconnect
};

struct PlayerInfo {
    sf::IpAddress address;
    unsigned short port;
    long unsigned playerId;
};

struct GameStatePacket {
    PacketType type = GameStateUpdate;
    sf::Vector2f ballPos;
    sf::Vector2f paddle1Pos;
    sf::Vector2f paddle2Pos;
    int score1;
    int score2;
    sf::Vector2f velocity;
};

struct PlayerInputPacket {
    PacketType type = PlayerInput;
    bool moveUp;
    bool moveDown;
};

struct AcceptionPacket {
    PacketType type = ConnectionAccept;
    
};

struct GameStartPacket {
    PacketType type = GameStart;
    int assignedPlayerId;
};

inline sf::Packet &operator >> (sf::Packet &packet, PacketType &type) {
    sf::Int32 typeAsInt;
    if (packet >> typeAsInt) {
        type = static_cast<PacketType>(typeAsInt);
    }

    return packet;
}

inline sf::Packet &operator << (sf::Packet &packet, const PacketType &type) {
    return packet << static_cast<sf::Int32>(type);
}

inline sf::Packet &operator << (sf::Packet &packet, const GameStatePacket &state) {
    return packet << state.type
                  << state.ballPos.x << state.ballPos.y
                  << state.paddle1Pos.x << state.paddle1Pos.y
                  << state.paddle2Pos.x << state.paddle2Pos.y
                  << state.score1 << state.score2
                  << state.velocity.x << state.velocity.y; 
}

inline sf::Packet &operator >> (sf::Packet &packet, GameStatePacket &state) {
    PacketType receivedType;
    if (packet >> receivedType && receivedType == GameStateUpdate) {
        packet >> state.ballPos.x >> state.ballPos.y
               >> state.paddle1Pos.x >> state.paddle1Pos.y
               >> state.paddle2Pos.x >> state.paddle2Pos.y
               >> state.score1 >> state.score2
               >> state.velocity.x << state.velocity.y;
    }

    return packet;
}

inline sf::Packet &operator << (sf::Packet &packet, const PlayerInputPacket &input) {
    return packet << input.type << input.moveUp << input.moveDown;
}

inline sf::Packet &operator >> (sf::Packet &packet, PlayerInputPacket &input) {
    PacketType receivedType;
    if (packet >> receivedType && receivedType == PlayerInput) {
        packet >> input.moveUp >> input.moveDown;
    }

    return packet;
}

class ServerManager {
    public:
        ServerManager();
        ~ServerManager();

        bool startServer();
        
        void handleConnectionReq();
        bool connectClient(const sf::IpAddress &address, unsigned short port);
        void disconnect();

        void sendGameState(const GameStatePacket &state);
        bool receivePlayerInput(PlayerInputPacket &input);

        // bool isServer() const {
        //     return isServer_;
        // }
        // bool isClient() const {
        //     return isClient_;
        // }
        // bool isConnected() const {
        //     return isConnected_;
        // }
        // bool hasClient() const {
        //     return clientAddress_ != sf::IpAddress::None;
        // }

    private:
        sf::UdpSocket serverSocket;
        sf::IpAddress serverAddress;
        unsigned short serverPort;

        bool isServer = false;

        std::vector<PlayerInfo> players;
};

class ClientManager {
    public:
        ClientManager();
        ~ClientManager();

        void sendConnectionReq();
        void sendPlayerInput(const PlayerInputPacket &input);
        bool receiveGameState(GameStatePacket &state);

        sf::UdpSocket clientSocket;
        sf::IpAddress serverAddress;
        unsigned short serverPort;

        bool isClient = false;
};
