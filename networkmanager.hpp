#pragma once

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum PacketType {
    GameStateUpdate,
    PlayerInput,
    ConnectionRequest,
    ConnectionAccept,
    GameStart,
    PlayerDisconnect
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

struct ConnectionPacket {
    PacketType type = ConnectionRequest;
    std::string playerName;
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

class NetworkManager {
    public:
        NetworkManager();
        ~NetworkManager();

        bool startServer(unsigned short port);
        bool connectClient(const sf::IpAddress &address, unsigned short port);
        void disconnect();

        void sendGameState(const GameStatePacket &state);
        bool receivePlayerInput(PlayerInputPacket &input);

        void sendPlayerInput(const PlayerInputPacket &input);
        bool receiveGameState(GameStatePacket &state);

        bool isServer() const {
            return isServer_;
        }
        bool isClient() const {
            return isClient_;
        }
        bool isConnected() const {
            return isConnected_;
        }
        bool hasClient() const {
            return clientAddress_ != sf::IpAddress::None;
        }

    private:
        sf::UdpSocket socket_;
        sf::IpAddress serverAddress_;
        unsigned short serverPort_;

        sf::IpAddress clientAddress_;
        unsigned short clientPort_;

        bool isServer_ = false;
        bool isClient_ = false;
        bool isConnected_ = false;

        struct ClientInfo {
            sf::IpAddress address;
            unsigned short port;
            bool isReady;
            int playerId;
        };

        std::vector<ClientInfo> clients;
};
