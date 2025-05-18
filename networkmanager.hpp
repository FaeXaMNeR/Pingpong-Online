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

    GameStart,
    Goodbye
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
    return packet << input.type << input.moveUp << input.moveDown;
}

inline sf::Packet &operator >> (sf::Packet &packet, PlayerInputPacket &input) {
    PacketType receivedType;
    if (packet >> receivedType && receivedType == PlayerInput) {
        packet >> input.moveUp >> input.moveDown;
    }

    return packet;
}       // WARNING Может сломаться

class ServerManager {
    public:
        ServerManager();
        ~ServerManager();
        
        void handleNetworkInput(PlayerInputPacket &input);
        bool connectClient(const sf::IpAddress &address, unsigned short port);

        void sendGameState(const PongState &pongState);
        bool receivePlayerInput(PlayerInputPacket &input);

        void drawServerInfo(sf::RenderWindow &window) {
            window.draw(serverInfoText);
        }

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

        std::vector<PlayerInfo> players;

        sf::Text serverInfoText;
        sf::Font font;
};

class ClientManager {
    public:
        ClientManager();
        ~ClientManager();

        void sendConnectionReq();
        void handleLobby(sf::RenderWindow &window, sf::Event &event);
        void lobbyReadIp(sf::RenderWindow &window, sf::Event &event);
        void lobbyReadPort(sf::RenderWindow &window, sf::Event &event);
        void handleNetworkInput();

        void sendPlayerInput(const PlayerInputPacket &input);
        bool receiveGameState(GameStatePacket &state);

        void drawGameState(PongState pongState, sf::RenderWindow &window);

        sf::UdpSocket clientSocket;

        sf::IpAddress clientAddress;
        unsigned short clientPort;

        sf::IpAddress serverAddress;
        unsigned short serverPort = 0;
        bool isConnected = false;

        GameStatePacket gameState;

        sf::Text lobbyIpText;
        sf::Text lobbyPortText;
        sf::String serverIpString;
        sf::String serverPortString;
        sf::Font font;
};
