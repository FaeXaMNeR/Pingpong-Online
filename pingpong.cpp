#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <math.h>

#include "mainmenu.h"
#include "constants.h"


int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "PingPong", sf::Style::Titlebar | sf::Style::Close);
    Menu menu(window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);

    sf::RectangleShape paddle1;
    paddle1.setSize(PaddleSize);
    paddle1.setFillColor(sf::Color::White);
    paddle1.setPosition(PaddleSize.x,(WINDOW_Y/2) - PaddleSize.y);
    paddle1.setOrigin(sf::Vector2f(paddle1.getSize().x / 2, paddle1.getSize().y / 2));

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

    font.loadFromFile("digit.ttf");

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
    bool Playing = false;
    while (window.isOpen()) {
        if (Playing) {   
            float deltaTime = clock.restart().asSeconds();
            float factor = deltaTime * BallSpeed;
            velocity.x = std::cos(BallAngle)*factor;
            velocity.y = std::sin(BallAngle)*factor;

            ball.move(velocity.x, velocity.y);

            if ((ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) || ball.getGlobalBounds().intersects(paddle1.getGlobalBounds())) {
                ball.move(-velocity.x, -velocity.y);
                velocity.x = -velocity.x * 1.05f;
                velocity.y = -velocity.y * 1.05f;
                BallSpeed = -BallSpeed * 1.05f;
                BallAngle = -BallAngle;
            }


            if (ball.getGlobalBounds().intersects(topBorder.getGlobalBounds()) || ball.getGlobalBounds().intersects(botBorder.getGlobalBounds())) {
                velocity.x = -velocity.x;
                BallAngle = -BallAngle;
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(paddle1.getGlobalBounds().intersects(topBorder.getGlobalBounds()))) 
                paddle1.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(paddle1.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle1.move(sf::Vector2f(0, PaddleSize.x / 2));

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !(paddle2.getGlobalBounds().intersects(topBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, -(PaddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && !(paddle2.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, PaddleSize.x / 2));

            if (ball.getPosition().x > paddle2.getPosition().x + 2*BallRad) {
                intScore1++;
                strScore1 = std::to_string(intScore1);
                textScore1.setString(strScore1);
                ball.setPosition(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));
                BallSpeed = 400.f;
            }

            if (ball.getPosition().x < paddle1.getPosition().x - 2*BallRad) {
                intScore2++;
                strScore2 = std::to_string(intScore2);
                textScore2.setString(strScore2);
                ball.setPosition(sf::Vector2f(WINDOW_X/2, WINDOW_Y/2));
                BallSpeed = 400.f;
            }


            window.clear(sf::Color::Black);
            window.draw(paddle1);
            window.draw(paddle2);
            window.draw(ball);
            window.draw(topBorder);
            window.draw(botBorder);
            window.draw(line, 2, sf::Lines);
            window.draw(textScore1);
            window.draw(textScore2);
            window.display();
        }

        if (!Playing) {
            window.clear(sf::Color::Black);
            menu.draw(window);
            window.display();
        }


        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();

                case sf::Event::KeyPressed:
                    switch (event.key.code)
                        case sf::Keyboard::Escape:
                            Playing = false;
                            break;

                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::Up:  
                        case sf::Keyboard::W:
                            menu.MoveUp();
                            break;
                        case sf::Keyboard::Down:
                        case sf::Keyboard::S:
                            menu.MoveDown();
                            break;

                    case sf::Keyboard::Return:
                        switch (menu.GetPressedItem()) {
                            case 0:
                                Playing = true;
                                break;
                            case 1:
                                //open sound menu
                                break;
                            case 2:
                                window.close();
                                break;
                            }
                        break;
                    }
                break;
            }

        }

    }

    return 0;
}