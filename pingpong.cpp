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


sf::Vector2f PaddleSize(WINDOW_X / 160, WINDOW_Y * 2 / 30); // TODO Почему не даёт слинковать, если он в constants.hpp?

enum Paddle {
    Left,
    Right
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
    sf::Font font;   
    sf::CircleShape ball;
    sf::Vector2f velocity;
    sf::Clock clock;

    void setTheGame() {
        paddle1.setSize(PaddleSize);
        paddle2.setSize(PaddleSize);

        paddle1.setFillColor(sf::Color::White);
        paddle2.setFillColor(sf::Color::White);

        paddle1.setPosition(               PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y / 2);
        paddle2.setPosition(WINDOW_X - 2 * PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y / 2);


        topBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        botBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        
        topBorder.setPosition(0, 0);
        botBorder.setPosition(0, WINDOW_Y * 89 / 90);

        
        textScore1.setCharacterSize(WINDOW_Y / 10);
        textScore2.setCharacterSize(WINDOW_Y / 10);

        textScore1.setPosition(sf::Vector2f(WINDOW_X * 1 / 5, 0));
        textScore2.setPosition(sf::Vector2f(WINDOW_X * 4 / 5, 0));
        
        font.loadFromFile("pong.ttf");
        textScore1.setFont(font);
        textScore2.setFont(font);

        convertScoreToText();

        line[0] = sf::Vertex(sf::Vector2f(WINDOW_X / 2, 0));
        line[1] = sf::Vertex(sf::Vector2f(WINDOW_X / 2, WINDOW_Y));

        ball.setRadius(BallRad);
        ball.setPointCount(10);
        ball.setFillColor(sf::Color::White);
        ball.setPosition(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));  
    }

    void convertScoreToText() {
        strScore1 = std::to_string(intScore1);
        textScore1.setString(strScore1);

        strScore2 = std::to_string(intScore2);
        textScore2.setString(strScore2);
    }
    
    void reset() {
        ball.setPosition(sf::Vector2f(BallRad + 2 * PaddleSize.x, WINDOW_Y / 2));
        paddle1.setPosition(               PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y / 2);
        paddle2.setPosition(WINDOW_X - 2 * PaddleSize.x, WINDOW_Y / 2 - PaddleSize.y / 2);
        intScore1 = 0;
        intScore2 = 0;
        strScore1 = std::to_string(intScore1);
        textScore1.setString(strScore1);
        strScore2 = std::to_string(intScore2);
        textScore2.setString(strScore2);
        resetVelocity(Left);
        clock.restart();
    }

    void resetVelocity(Paddle servingPaddle) {
        velocity.y = rand() % 600 - 300;
        
        if (servingPaddle == Left) {
            velocity.x = rand() % 150 + 450;
        } else {
            velocity.x = - (rand() % 150 + 450);
        }
    }

    void draw(sf::RenderWindow &window) {
        window.clear(sf::Color::Black);
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(ball);
        window.draw(topBorder);
        window.draw(botBorder);
        window.draw(line, 2, sf::Lines);
        window.draw(textScore1);
        window.draw(textScore2);
    }

    float getDeltaTime() {
        return clock.restart().asSeconds();
    }

    bool ballPaddleIntersection() {
        return  ball.getGlobalBounds().intersects(paddle1.getGlobalBounds()) || 
                ball.getGlobalBounds().intersects(paddle2.getGlobalBounds());
    }

    bool ballBorderIntersection() {
        return  ball.getGlobalBounds().intersects(topBorder.getGlobalBounds()) || 
                ball.getGlobalBounds().intersects(botBorder.getGlobalBounds());
    }

    void gooool(Paddle winnerPaddle) {
        if (winnerPaddle == Right) {
            intScore2++;
            strScore2 = std::to_string(intScore2);
            textScore2.setString(strScore2);
            ball.setPosition(WINDOW_X - BallRad - 3 * PaddleSize.x, WINDOW_Y/2);
            resetVelocity(Right);
        } else {
            intScore1++;
            strScore1 = std::to_string(intScore1);
            textScore1.setString(strScore1);
            ball.setPosition(BallRad + 2 * PaddleSize.x, WINDOW_Y/2);
            resetVelocity(Left);
        }
    }

    void handleBallCollisions() {
        velocity.x -= 2 * velocity.x * 1.05 * ballPaddleIntersection();
        velocity.y -= 2 * velocity.y * ballBorderIntersection();
    }
};

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
    bool eventOccured;

    while (window.isOpen()) {
        
        switch (gameMode) {
            case MainMenu: {
                while (gameMode == MainMenu) {
                    eventOccured = window.pollEvent(event);
                    menu.draw(window);
                    if (eventOccured) {
                        gameMode = menu.handleInput(event, window);
                          
                        if (event.type == sf::Event::Closed) {
                            window.close();
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
                        pongState.paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));

                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && 
                            !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                        pongState.paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && 
                            !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
                        pongState.paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
                    
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && 
                                !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
                        pongState.paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));  // TODO что-то сделать с этим безобразием

                    if (pongState.ball.getPosition().x < 0) {
                        pongState.gooool(Right);
                    } else if (pongState.ball.getPosition().x > WINDOW_X) {
                        pongState.gooool(Left);
                    }

                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        window.close();
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
                while (gameMode == Server) {
                    serverManager.handleNetworkInput(input);
                    

                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    } 
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    } 
                }
                break;
            }

            case Client: {
                ClientManager clientManager;
                clientManager.sendConnectionReq();
                while (gameMode == Client) {
                    clientManager.handleNetworkInput();
                    
                    window.pollEvent(event);
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameMode = MainMenu;
                    }
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


        // if (gameMode == Server) {42263
        //     float deltaTime = clock.restart().asSeconds();

        //     if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
        //         pongState.paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
        //     if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(pongState.paddle1.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
        //         pongState.paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

        //     // Прием ввода от клиента
        //     PlayerInputPacket clientInput;
        //     networkManager.receivePlayerInput(clientInput);

        //     if (clientInput.moveUp && !(pongState.paddle2.getGlobalBounds().intersects(pongState.topBorder.getGlobalBounds())))
        //         pongState.paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
        //     if (clientInput.moveDown && !(pongState.paddle2.getGlobalBounds().intersects(pongState.botBorder.getGlobalBounds())))
        //         pongState.paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));


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
