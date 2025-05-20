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

    window.setVerticalSyncEnabled(true);

    PongState pongState;

    GameMode gameMode = MainMenu;

    sf::Event event;

    while (window.isOpen()) {
        switch (gameMode) {
            case MainMenu: {
                Menu menu;
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
                    pongState.moveBall();

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
                        pongState.paddle2.move(sf::Vector2f(0, PADDLE_X / 2));

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
                while (gameMode == Server) {
                    //std::cout << "Before" << std::endl;
                    serverManager.handleNetworkInput();
                    
                    serverManager.runRooms();

                    serverManager.sendGameState();

                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        gameMode = None;
                        window.close();
                    } 
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    }

                    
                    serverManager.drawGameState(window);
                    //std::cout << "After" << std::endl;
                    window.display();
                }
                break;
            }

            case Client: {
                ClientManager clientManager;
                clientManager.sendConnectionReq(window);
                sf::Clock clientClock;
                while (gameMode == Client) {
                    clientManager.handleNetworkInput();

                    clientManager.sendPlayerInput();
                    
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

    return 0;
}
