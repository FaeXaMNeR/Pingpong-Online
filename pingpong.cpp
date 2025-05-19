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
#include "pingpong.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "PingPong", sf::Style::Titlebar | sf::Style::Close);
    Menu menu;

    window.setVerticalSyncEnabled(true);

    PongState pongState;

    pongState.setTheGame();

    GameMode gameMode = MainMenu;
    
    sf::Clock networkClock;
    sf::Clock clientInputClock;

    sf::Event event;

    while (window.isOpen()) {
        
        switch (gameMode) {
            case MainMenu: {
                while (gameMode == MainMenu) {
                    menu.draw(window);
                    if (window.pollEvent(event)) {
                        gameMode = menu.handleInput(event, window);
                          
                        if (event.type == sf::Event::Closed) {
                            window.close();
                            gameMode = None;
                        }                 
                    }  
                    window.display();
                }
                pongState.reset();
                break;
            }

            case OfflineGame: {
                while (gameMode == OfflineGame) {
                    pongState.ball.move(pongState.velocity * pongState.getDeltaTime());

                    pongState.handleBallCollisions();

                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && 
                            !(pongState.paddle1.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds()))) 
                        pongState.paddle1.move(sf::Vector2f(0, -(PADDLE_X / 2)));

                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && 
                            !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                        pongState.paddle1.move(sf::Vector2f(0, PADDLE_X / 2));

                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && 
                            !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
                        pongState.paddle2.move(sf::Vector2f(0, -(PADDLE_X / 2)));
                    
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && 
                                !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                        pongState.paddle2.move(sf::Vector2f(0, PADDLE_X / 2));  // TODO что-то сделать с этим безобразием

                    if (pongState.ball.getPosition().x < 0) {
                        pongState.gooool(Right);
                    } else if (pongState.ball.getPosition().x > WINDOW_X) {
                        pongState.gooool(Left);
                    }

                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        gameMode = None;
                    } 
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    } 
                    
                    pongState.draw(window);
                    window.display();
                } 
                break;
            }

            case Server: {
                ServerManager serverManager;
                PlayerInputPacket input;
                int counter = 0;
                sf::Clock clock;
                while (gameMode == Server) {
                    serverManager.handleNetworkInput(input);

                    pongState.ball.move(pongState.velocity * pongState.getDeltaTime());

                    pongState.handleBallCollisions();

                    if (pongState.ball.getPosition().x < 0) {
                        pongState.gooool(Right);
                    } else if (pongState.ball.getPosition().x > WINDOW_X) {
                        pongState.gooool(Left);
                    }

                    serverManager.sendGameState(pongState);

                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        gameMode = None;
                        window.close();
                    } 
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    } 
                    counter++;
                    if (clock.getElapsedTime().asSeconds() >= 1) {
                        //std::cout << counter << std::endl;
                        counter = 0;
                        clock.restart();
                    }

                    pongState.draw(window);
                    serverManager.drawServerInfo(window);
                    window.display();
                }
                break;
            }

            case Client: {
                ClientManager clientManager;
                clientManager.sendConnectionReq(window);
                sf::Clock clientClock;
                int counter = 0;
                sf::Clock clock;
                while (gameMode == Client) {
                    // std::cout << clientClock.restart().asSeconds() << std::endl;
                    clientManager.handleNetworkInput();

                    counter++;
                    if (clock.getElapsedTime().asSeconds() >= 1) {
                        std::cout << "FPS: " << counter << std::endl;
                        counter = 0;
                        clock.restart();
                    }
                    
                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        gameMode = None;
                        window.close();
                    }
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    }

                    clientManager.drawGameState(pongState, window);
                    window.display();
                }
            }

            default:
                break;
        }

        if (event.type == sf::Event::Closed) {
            window.close();
        } 
        if (event.key.code == sf::Keyboard::Escape) {
            gameMode = MainMenu;
        }                  
        
        window.display();
    }


        // if (gameMode == Server) {
        //     float deltaTime = clock.restart().asSeconds();

        //     if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
        //         pongState.paddle1.move(sf::Vector2f(0, -(PADDLE_X / 2)));
        //     if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
        //         pongState.paddle1.move(sf::Vector2f(0, PADDLE_X / 2));

        //     // Прием ввода от клиента
        //     PlayerInputPacket clientInput;
        //     networkManager.receivePlayerInput(clientInput);

        //     if (clientInput.moveUp && !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
        //         pongState.paddle2.move(sf::Vector2f(0, -(PADDLE_X / 2)));
        //     if (clientInput.moveDown && !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
        //         pongState.paddle2.move(sf::Vector2f(0, PADDLE_X / 2));


        //     if (networkManager.hasClient()) {
        //         pongState.handleBallCollisions();

        //         if (pongState.ball.getPosition().x < 0) {
        //             pongState.gooool(Right);
        //         } else if (pongState.ball.getPosition().x > WINDOW_X) {
        //             pongState.gooool(Left);
        //         }


        //         // Отправка состояния игры клиенту
        //         if (networkManager.isConnected() && networkClock.getElapsedTime().asSeconds() >= 1.0f / TICK_RATE) {
        //             GameStatePacket currentState;
        //             currentState.ballPos = pongState.ball.getPosition();
        //             currentState.paddle1Pos = pongState.paddle1.getPosition();
        //             currentState.paddle2Pos = pongState.paddle2.getPosition();
        //             currentState.score1 = pongState.intScore1;
        //             currentState.score2 = pongState.intScore2;
        //             currentState.velocity = pongState.velocity;
        //             networkManager.sendGameState(currentState);
        //             networkClock.restart();
        //         }

        //         pongState.ball.move(pongState.velocity * deltaTime);
        //     }

        //     // Отрисовка игры
        //     pongState.draw(window);
        // } 
        // else if (gameMode == Client) {
        //     if (networkManager.isConnected() && clientInputClock.getElapsedTime().asSeconds() >= 1.0f / TICK_RATE) {
        //         PlayerInputPacket clientInput;
        //         clientInput.moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        //         clientInput.moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        //         networkManager.sendPlayerInput(clientInput);
        //         clientInputClock.restart();
        //     }

        //     // Прием состояния игры от сервера
        //     GameStatePacket serverState;
        //     if (networkManager.receiveGameState(serverState)) {
        //         pongState.ball.setPosition(serverState.ballPos);
        //         pongState.paddle1.setPosition(serverState.paddle1Pos);
        //         pongState.paddle2.setPosition(serverState.paddle2Pos);

        //         pongState.intScore1 = serverState.score1;
        //         pongState.intScore2 = serverState.score2;
        //         pongState.strScore1 = std::to_string(pongState.intScore1);
        //         pongState.textScore1.setString(pongState.strScore1);
        //         pongState.strScore2 = std::to_string(pongState.intScore2);
        //         pongState.textScore2.setString(pongState.strScore2);

        //         pongState.velocity = serverState.velocity;
        //     }

        //     // Отрисовка игры
        //     pongState.draw(window);
        // // }

    return 0;
}
