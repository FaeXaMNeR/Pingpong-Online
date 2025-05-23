#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cassert>
#include <csignal>
#include <atomic>

#include "mainmenu.hpp"
#include "constants.hpp"
#include "networkmanager.hpp"
#include "pingpong.hpp"

bool quit = false;

void signal_handler(int) {
    quit = true;
}

void handleExit(GameMode &gameMode, sf::RenderWindow &window) {
    sf::Event event;
    window.pollEvent(event);
    if (event.type == sf::Event::Closed) {
        gameMode = None;
        window.close();
    }
    if (event.key.code == sf::Keyboard::Escape) {
        gameMode = MainMenu;
    }
    if (quit) {
        gameMode = None;
        window.close();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "PingPong", sf::Style::Titlebar | sf::Style::Close);

    window.setVerticalSyncEnabled(true);
    std::signal(SIGINT, signal_handler);

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
                            gameMode = None;
                            window.close();
                        }          
                    }  
                    handleExit(gameMode, window); 
                    window.display();
                }
                pongState.reset();
                break;
            }

            case OfflineGame: {
                bool Paddle1Up   = false;
                bool Paddle1Down = false;
                bool Paddle2Up   = false;
                bool Paddle2Down = false;
                while (gameMode == OfflineGame) {
                    pongState.moveBall();

                    pongState.handleBallCollisions();

                    Paddle1Up   = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
                    Paddle1Down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
                    Paddle2Up   = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
                    Paddle2Down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

                    pongState.handlePaddleMovement(Paddle1Up, Paddle1Down, Paddle2Up, Paddle2Down);

                    pongState.handleGoals();

                    handleExit(gameMode, window);
                    
                    pongState.draw(window);
                    window.display();
                } 
                break;
            }

            case Server: {
                ServerManager serverManager;
                while (gameMode == Server) {
                    
                    serverManager.handleNetworkInput();
                    
                    serverManager.runRooms();

                    serverManager.sendGameState();

                    handleExit(gameMode, window);

                    serverManager.drawGameState(window);
                    window.display(); 
                }
                break;
            }

            case Client: {
                ClientManager clientManager;
                clientManager.sendConnectionReq(window);
                while (gameMode == Client) {
                    clientManager.handleNetworkInput();

                    clientManager.sendPlayerInput();
                    
                    handleExit(gameMode, window);

                    clientManager.drawGameState(pongState, window);
                    window.display();
                }
                break;
            }

            default:
                break;
        }                 
    }

    return 0;
}
