#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "mainmenu.hpp"
#include "constants.hpp"
#include "networkmanager.hpp"

// Определение состояний игры (для управления режимами: меню, офлайн, сервер, клиент)
enum GameState {
    MainMenu,
    OfflineGame,
    Server,
    Client
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "PingPong", sf::Style::Titlebar | sf::Style::Close);
    Menu menu(window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);

    sf::RectangleShape paddle1;
    paddle1.setSize(PaddleSize);
    paddle1.setFillColor(sf::Color::White);
    paddle1.setPosition(PaddleSize.x,(WINDOW_Y/2) - PaddleSize.y);

    sf::RectangleShape paddle2;
    paddle2.setSize(PaddleSize);
    paddle2.setFillColor(sf::Color::White);
    paddle2.setPosition(WINDOW_X-(PaddleSize.x*2), (WINDOW_Y/2)-PaddleSize.y);

    sf::RectangleShape topBorder;
    topBorder.setSize(sf::Vector2f(WINDOW_X,PaddleSize.x));
    topBorder.setPosition(0, 0);

    sf::RectangleShape botBorder;
    botBorder.setSize(sf::Vector2f(WINDOW_X, PaddleSize.x));
    botBorder.setPosition(0, WINDOW_Y-PaddleSize.x);

    sf::Font font;
    font.loadFromFile("pong.ttf");

    int intScore1 = 0;
    sf::Text textScore1;
    std::string strScore1 = std::to_string(intScore1);
    textScore1.setString(strScore1);
    textScore1.setFont(font);
    textScore1.setCharacterSize(PaddleSize.y*1.5f);
    textScore1.setPosition(sf::Vector2f(WINDOW_X/5.f, 0.f));

    int intScore2 = 0;
    sf::Text textScore2;
    std::string strScore2 = std::to_string(intScore2);
    textScore2.setString(strScore2);
    textScore2.setFont(font);
    textScore2.setCharacterSize(PaddleSize.y*1.5f);
    textScore2.setPosition(sf::Vector2f(WINDOW_X/5.f*4.f, 0.f));

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(WINDOW_X/2+1,0)),
        sf::Vertex(sf::Vector2f(WINDOW_X/2+1,WINDOW_Y))
    };

    sf::CircleShape ball(BallRad);
    ball.setPointCount(10);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(sf::Vector2f(WINDOW_X/2.f, WINDOW_Y/2.f));

    sf::Clock clock;
    GameState gameState = MainMenu;
    NetworkManager networkManager;
    sf::Clock networkClock;
    sf::Clock clientInputClock;

    // Инициализация скорости мяча при старте
    velocity = initialVelocity;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        if (gameState != MainMenu) {
                            gameState = MainMenu;
                            networkManager.disconnect();
                        }
                    }
                    break;

                default:
                    if (gameState == MainMenu) {
                        menu.handleInput(event);
                        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
                            switch (menu.getSelectedItem()) {
                                case 0: // Play Offline
                                    gameState = OfflineGame;
                                    // Сброс состояния игры для новой офлайн-игры
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

                                    clock.restart();
                                    break;
                                case 1: // Launch Server
                                    if (networkManager.startServer(PORT)) {
                                        gameState = Server;
                                         // Сброс состояния игры для новой сетевой игры
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
                                            gameState = Client;
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

        if (gameState == MainMenu) {
            menu.draw(window);
        } else if (gameState == OfflineGame) {
            // На одном экране
            float deltaTime = clock.restart().asSeconds();

            ball.move(velocity * deltaTime);

            if (ball.getGlobalBounds().intersects(paddle1.getGlobalBounds()) || 
                ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) 
            {
                velocity.x = -velocity.x * 1.05f;
                ball.move(velocity * deltaTime * 2.0f);
            }


            if (ball.getGlobalBounds().intersects(topBorder.getGlobalBounds()) || 
                ball.getGlobalBounds().intersects(botBorder.getGlobalBounds())) 
            {
                velocity.y = -velocity.y;
            }

            
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(paddle1.getGlobalBounds().intersects(topBorder.getGlobalBounds()))) 
                paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(paddle1.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !(paddle2.getGlobalBounds().intersects(topBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && !(paddle2.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));

            
            if (ball.getPosition().x < 0) {
                intScore2++;
                strScore2 = std::to_string(intScore2);
                textScore2.setString(strScore2);
                ball.setPosition(WINDOW_X - BallRad, WINDOW_Y/2);
                velocity.x = -initialVelocity.x;
                velocity.y = initialVelocity.y;
            }
            else if (ball.getPosition().x > WINDOW_X) {
                intScore1++;
                strScore1 = std::to_string(intScore1);
                textScore1.setString(strScore1);
                ball.setPosition(BallRad, WINDOW_Y/2);
                velocity = initialVelocity;
            }

            // Отрисовка
            window.draw(paddle1);
            window.draw(paddle2);
            window.draw(ball);
            window.draw(topBorder);
            window.draw(botBorder);
            window.draw(line, 2, sf::Lines);
            window.draw(textScore1);
            window.draw(textScore2);

        } 
        else if (gameState == Server) {
            float deltaTime = clock.restart().asSeconds();

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(paddle1.getGlobalBounds().intersects(topBorder.getGlobalBounds())))
                paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(paddle1.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

            // Прием ввода от клиента
            PlayerInputPacket clientInput;
            networkManager.receivePlayerInput(clientInput);

            if (clientInput.moveUp && !(paddle2.getGlobalBounds().intersects(topBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if (clientInput.moveDown && !(paddle2.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));


            if (networkManager.hasClient()) {
                if (ball.getGlobalBounds().intersects(paddle1.getGlobalBounds()) || ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) {
                    velocity.x = -velocity.x * 1.05f;                  
                    
                    ball.move(velocity * deltaTime * 2.0f);
                }


                
                if (ball.getGlobalBounds().intersects(topBorder.getGlobalBounds()) || 
                    ball.getGlobalBounds().intersects(botBorder.getGlobalBounds())) {
                    velocity.y = -velocity.y;
                }


                // Проверка на гол
                if (ball.getPosition().x > WINDOW_X) {
                    intScore1++;
                    strScore1 = std::to_string(intScore1);
                    textScore1.setString(strScore1);
                    ball.setPosition(0, WINDOW_Y/2);
                    velocity = initialVelocity;
                }

                if (ball.getPosition().x < 0) {
                    intScore2++;
                    strScore2 = std::to_string(intScore2);
                    textScore2.setString(strScore2);
                    ball.setPosition(WINDOW_X, WINDOW_Y/2);
                    velocity.x = -initialVelocity.x;
                    velocity.y = initialVelocity.y;
                }


                // Отправка состояния игры клиенту
                if (networkManager.isConnected() && networkClock.getElapsedTime().asSeconds() >= 1.0f / TICK_RATE) {
                    GameStatePacket currentState;
                    currentState.ballPos = ball.getPosition();
                    currentState.paddle1Pos = paddle1.getPosition();
                    currentState.paddle2Pos = paddle2.getPosition();
                    currentState.score1 = intScore1;
                    currentState.score2 = intScore2;
                    currentState.velocity = velocity;
                    networkManager.sendGameState(currentState);
                    networkClock.restart();
                }

                ball.move(velocity * deltaTime);
            }



            // Отрисовка игры
            window.draw(paddle1);
            window.draw(paddle2);
            window.draw(ball);
            window.draw(topBorder);
            window.draw(botBorder);
            window.draw(line, 2, sf::Lines);
            window.draw(textScore1);
            window.draw(textScore2);

        } 
        else if (gameState == Client) {
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
                ball.setPosition(serverState.ballPos);
                paddle1.setPosition(serverState.paddle1Pos);
                paddle2.setPosition(serverState.paddle2Pos);

                intScore1 = serverState.score1;
                intScore2 = serverState.score2;
                strScore1 = std::to_string(intScore1);
                textScore1.setString(strScore1);
                strScore2 = std::to_string(intScore2);
                textScore2.setString(strScore2);

                velocity = serverState.velocity;
            }

            // Отрисовка игры
            window.draw(paddle1);
            window.draw(paddle2); 
            window.draw(ball);
            window.draw(topBorder);
            window.draw(botBorder);
            window.draw(line, 2, sf::Lines);
            window.draw(textScore1);
            window.draw(textScore2);
        }

        window.display();
    }

    return 0;
}
