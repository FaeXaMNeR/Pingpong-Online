#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cassert>

#include "mainmenu.hpp"
#include "constants.hpp"
#include "networkmanager.hpp"

// Определение состояний игры (для управления режимами: меню, офлайн, сервер, клиент)
enum GameMode {
    MainMenu,
    OfflineGame,
    Server,
    Client
};

class PongState {
public:
    sf::RectangleShape paddle1;
    sf::RectangleShape paddle2;
    sf::RectangleShape topBorder;
    sf::RectangleShape botBorder;
    sf::Vertex line[2];
    int intScore1 = 0;
    int intScore2 = 0; 
    std::string strScore1;
    std::string strScore2;
    sf::Text textScore1;
    sf::Text textScore2;   
    sf::CircleShape ball;
    sf::Vector2f velocity;

    void setTheGame() {
        paddle1.setSize(PaddleSize);
        paddle1.setFillColor(sf::Color::White);
        paddle1.setPosition(PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y);

        paddle2.setSize(PaddleSize);
        paddle2.setFillColor(sf::Color::White);
        paddle2.setPosition(WINDOW_X - 2 * PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y);

        topBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        topBorder.setPosition(0, 0);

        botBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        botBorder.setPosition(0, WINDOW_Y * 89 / 90);

        textScore1.setCharacterSize(WINDOW_Y / 10);
        textScore1.setPosition(sf::Vector2f(WINDOW_X / 5, 0));

        textScore2.setCharacterSize(WINDOW_Y / 10);
        textScore2.setPosition(sf::Vector2f(WINDOW_X * 4 / 5, 0));

        convertScoreToText();

        line[0] = sf::Vertex(sf::Vector2f(WINDOW_X / 2 /*+ 1*/, 0));
        line[1] = sf::Vertex(sf::Vector2f(WINDOW_X / 2 /*+ 1*/, WINDOW_Y));

        ball.setRadius(BallRad);
        ball.setPointCount(10);
        ball.setFillColor(sf::Color::White);
        ball.setPosition(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));

        velocity = initialVelocity;
    };

    void convertScoreToText() {
        strScore1 = std::to_string(intScore1);
        textScore1.setString(strScore1);

        strScore2 = std::to_string(intScore2);
        textScore2.setString(strScore2);
    };
    
    void reset() {
        ball.setPosition(sf::Vector2f(WINDOW_X/2.f, WINDOW_Y/2.f));
        paddle1.setPosition(PaddleSize.x,(WINDOW_Y/2) - PaddleSize.y);
        paddle2.setPosition(WINDOW_X-(PaddleSize.x*2), (WINDOW_Y/2)-PaddleSize.y);
        intScore1 = 0;
        intScore2 = 0;
        strScore1 = std::to_string(intScore1);
        textScore1.setString(strScore1);
        strScore2 = std::to_string(intScore2);
        textScore2.setString(strScore2);
        velocity = initialVelocity;
    };

    void draw(sf::RenderWindow * window) {
        window->draw(paddle1);
        window->draw(paddle2);
        window->draw(ball);
        window->draw(topBorder);
        window->draw(botBorder);
        window->draw(line, 2, sf::Lines);
        window->draw(textScore1);
        window->draw(textScore2);
    };
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "PingPong", sf::Style::Titlebar | sf::Style::Close);
    Menu menu(window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);

    PongState pongState;

    sf::Font font;
    font.loadFromFile("pong.ttf");
    pongState.textScore1.setFont(font);
    pongState.textScore2.setFont(font);

    pongState.setTheGame();

    sf::Clock clock;
    GameMode gameMode = MainMenu;
    NetworkManager networkManager;
    sf::Clock networkClock;
    sf::Clock clientInputClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        if (gameMode != MainMenu) {
                            gameMode = MainMenu;
                            networkManager.disconnect();
                        }
                    }
                    break;

                default:
                    if (gameMode == MainMenu) {
                        menu.handleInput(event);
                        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
                            switch (menu.getSelectedItem()) {
                                case 0: // Play Offline
                                    gameMode = OfflineGame;
                                    // Сброс состояния игры для новой офлайн-игры
                                    pongState.reset();

                                    clock.restart();
                                    break;
                                case 1: // Launch Server
                                    if (networkManager.startServer(PORT)) {
                                        gameMode = Server;
                                         // Сброс состояния игры для новой сетевой игры
                                        pongState.reset();

                                        clock.restart();
                                        networkClock.restart();
                                    } else {
                                        std::cerr << "Failed to start server!" << std::endl;
                                    }
                                    break;
                                case 2: // Join Server
                                    {
                                        sf::IpAddress serverAddress;
                                        std::string ipString;

                                        std::cout << "Enter server IP address:\n";
                                        std::cin >> ipString;

                                        serverAddress = sf::IpAddress(ipString);

                                        if (networkManager.connectClient(serverAddress, PORT)) {
                                            gameMode = Client;
                                            clock.restart();
                                            networkClock.restart();
                                        } else {
                                            std::cerr << "Failed to connect to server!" << std::endl;
                                        }
                                    }
                                    break;
                                case 3: // Exit
                                    window.close();
                                    break;
                            }
                        }
                    }
                    break;
            }
        }

        window.clear(sf::Color::Black);

        if (gameMode == MainMenu) {
            menu.draw(window);
        } else if (gameMode == OfflineGame) {
            // На одном экране
            float deltaTime = clock.restart().asSeconds();

            pongState.ball.move(pongState.velocity * deltaTime);

            if (pongState.ball.getGlobalBounds().intersects(pongState.paddle1.getGlobalBounds()) || 
                pongState.ball.getGlobalBounds().intersects(pongState.paddle2.getGlobalBounds())) 
            {
                pongState.velocity.x = -pongState.velocity.x * 1.05f;
                pongState.ball.move(pongState.velocity * deltaTime * 2.0f);
            }


            if (pongState.ball.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds()) || 
                pongState.ball.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())) 
            {
                pongState.velocity.y = - pongState.velocity.y;
            }

            
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds()))) 
                pongState.paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                pongState.paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
                pongState.paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                pongState.paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));

            
            if (pongState.ball.getPosition().x < 0) {
                pongState.intScore2++;
                pongState.strScore2 = std::to_string(pongState.intScore2);
                pongState.textScore2.setString(pongState.strScore2);
                pongState.ball.setPosition(WINDOW_X - BallRad - 2*PaddleSize.x, WINDOW_Y/2);
                pongState.velocity.x = -initialVelocity.x;
                pongState.velocity.y = initialVelocity.y;
            }
            else if (pongState.ball.getPosition().x > WINDOW_X) {
                pongState.intScore1++;
                pongState.strScore1 = std::to_string(pongState.intScore1);
                pongState.textScore1.setString(pongState.strScore1);
                pongState.ball.setPosition(BallRad + 2*PaddleSize.x, WINDOW_Y/2);
                pongState.velocity = initialVelocity;
            }

            // Отрисовка
            pongState.draw(&window);

        } 
        else if (gameMode == Server) {
            float deltaTime = clock.restart().asSeconds();

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
                pongState.paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                pongState.paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

            // Прием ввода от клиента
            PlayerInputPacket clientInput;
            networkManager.receivePlayerInput(clientInput);

            if (clientInput.moveUp && !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
                pongState.paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if (clientInput.moveDown && !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                pongState.paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));


            if (networkManager.hasClient()) {
                if (pongState.ball.getGlobalBounds().intersects(pongState.paddle1.getGlobalBounds()) || pongState.ball.getGlobalBounds().intersects(pongState.paddle2.getGlobalBounds())) {
                    pongState.velocity.x = -pongState.velocity.x * 1.05f;                  
                    
                    pongState.ball.move(pongState.velocity * deltaTime * 2.0f);
                }


                
                if (pongState.ball.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds()) || 
                    pongState.ball.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())) {
                    pongState.velocity.y = - pongState.velocity.y;
                }


                // Проверка на гол
                if (pongState.ball.getPosition().x > WINDOW_X) {
                    pongState.intScore1++;
                    pongState.strScore1 = std::to_string(pongState.intScore1);
                    pongState.textScore1.setString(pongState.strScore1);
                    pongState.ball.setPosition(0, WINDOW_Y/2);
                    pongState.velocity = initialVelocity;
                }

                if (pongState.ball.getPosition().x < 0) {
                    pongState.intScore2++;
                    pongState.strScore2 = std::to_string(pongState.intScore2);
                    pongState.textScore2.setString(pongState.strScore2);
                    pongState.ball.setPosition(WINDOW_X, WINDOW_Y/2);
                    pongState.velocity.x = -initialVelocity.x;
                    pongState.velocity.y = initialVelocity.y;
                }


                // Отправка состояния игры клиенту
                if (networkManager.isConnected() && networkClock.getElapsedTime().asSeconds() >= 1.0f / TICK_RATE) {
                    GameStatePacket currentState;
                    currentState.ballPos = pongState.ball.getPosition();
                    currentState.paddle1Pos = pongState.paddle1.getPosition();
                    currentState.paddle2Pos = pongState.paddle2.getPosition();
                    currentState.score1 = pongState.intScore1;
                    currentState.score2 = pongState.intScore2;
                    currentState.velocity = pongState.velocity;
                    networkManager.sendGameState(currentState);
                    networkClock.restart();
                }

                pongState.ball.move(pongState.velocity * deltaTime);
            }

            // Отрисовка игры
            pongState.draw(&window);
        } 
        else if (gameMode == Client) {
            if (networkManager.isConnected() && clientInputClock.getElapsedTime().asSeconds() >= 1.0f / TICK_RATE) {
                PlayerInputPacket clientInput;
                clientInput.moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
                clientInput.moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
                networkManager.sendPlayerInput(clientInput);
                clientInputClock.restart();
            }

            // Прием состояния игры от сервера
            GameStatePacket serverState;
            if (networkManager.receiveGameState(serverState)) {
                pongState.ball.setPosition(serverState.ballPos);
                pongState.paddle1.setPosition(serverState.paddle1Pos);
                pongState.paddle2.setPosition(serverState.paddle2Pos);

                pongState.intScore1 = serverState.score1;
                pongState.intScore2 = serverState.score2;
                pongState.strScore1 = std::to_string(pongState.intScore1);
                pongState.textScore1.setString(pongState.strScore1);
                pongState.strScore2 = std::to_string(pongState.intScore2);
                pongState.textScore2.setString(pongState.strScore2);

                pongState.velocity = serverState.velocity;
            }

            // Отрисовка игры
            pongState.draw(&window);
        }

        window.display();
    }

    return 0;
}
