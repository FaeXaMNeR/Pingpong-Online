#pragma once

#include "constants.hpp"
#include <iostream>

enum GameMode {
    MainMenu,
    OfflineGame,
    Server,
    Client,
    None
};

enum Paddle {
    Left,
    Right
};

class PongState {
public:
    sf::RectangleShape paddle1;
    sf::RectangleShape paddle2;
    sf::CircleShape ball;
    sf::RectangleShape topBorder;
    sf::RectangleShape botBorder;
    int intScore1;
    int intScore2;

    PongState() {
        paddle1.setSize(sf::Vector2f(PADDLE_X, PADDLE_Y));
        paddle2.setSize(sf::Vector2f(PADDLE_X, PADDLE_Y));

        paddle1.setFillColor(sf::Color::White);
        paddle2.setFillColor(sf::Color::White);

        paddle1.setPosition(               PADDLE_X, WINDOW_Y / 2 - PADDLE_Y / 2);
        paddle2.setPosition(WINDOW_X - 2 * PADDLE_X, WINDOW_Y / 2 - PADDLE_Y / 2);


        topBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        botBorder.setSize(sf::Vector2f(WINDOW_X, WINDOW_Y / 90));
        
        topBorder.setPosition(0, 0);
        botBorder.setPosition(0, WINDOW_Y * 89 / 90);


        intScore1 = 0;
        intScore2 = 0;

        textScore1.setCharacterSize(WINDOW_Y / 10);
        textScore2.setCharacterSize(WINDOW_Y / 10);

        textScore1.setPosition(sf::Vector2f(WINDOW_X * 1 / 5, 0));
        textScore2.setPosition(sf::Vector2f(WINDOW_X * 4 / 5, 0));
        
        font.loadFromFile("pong.ttf");
        textScore1.setFont(font);
        textScore2.setFont(font);
        textScore1.setFillColor(sf::Color::White);
        textScore2.setFillColor(sf::Color::White);

        convertScoreToText();

        line[0] = sf::Vertex(sf::Vector2f(WINDOW_X / 2, 0));
        line[1] = sf::Vertex(sf::Vector2f(WINDOW_X / 2, WINDOW_Y));

        ball.setRadius(BallRad);
        ball.setPointCount(10);
        ball.setFillColor(sf::Color::White);
        ball.setPosition(sf::Vector2f(WINDOW_X / 2, WINDOW_Y / 2));  

        resetVelocity(Right);
    }

    PongState(const PongState& other):
        paddle1(other.paddle1),
        paddle2(other.paddle2),
        ball(other.ball),
        topBorder(other.topBorder),
        botBorder(other.botBorder),
        intScore1(other.intScore1),
        intScore2(other.intScore2),
        velocity(other.velocity),
        strScore1(other.strScore1),
        strScore2(other.strScore2),
        textScore1(other.textScore1),
        textScore2(other.textScore2),
        font(other.font)
    {
        textScore1.setFont(font);
        textScore2.setFont(font);
        line[0] = other.line[0];
        line[1] = other.line[1];
    }
    PongState& operator=(const PongState&) = delete;

    void convertScoreToText() {
        strScore1 = std::to_string(intScore1);
        textScore1.setString(strScore1);

        strScore2 = std::to_string(intScore2);
        textScore2.setString(strScore2);
    }
    
    void reset() {
        ball.setPosition(sf::Vector2f(BallRad + 2 * PADDLE_X, WINDOW_Y / 2));
        paddle1.setPosition(               PADDLE_X, WINDOW_Y / 2 - PADDLE_Y / 2);
        paddle2.setPosition(WINDOW_X - 2 * PADDLE_X, WINDOW_Y / 2 - PADDLE_Y / 2);
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
            ball.setPosition(WINDOW_X - BallRad - 3 * PADDLE_X, WINDOW_Y/2);
            resetVelocity(Right);
        } else {
            intScore1++;
            strScore1 = std::to_string(intScore1);
            textScore1.setString(strScore1);
            ball.setPosition(BallRad + 2 * PADDLE_X, WINDOW_Y/2);
            resetVelocity(Left);
        }
    }

    void handleBallCollisions() {
        velocity.x -= 2 * velocity.x * 1.05 * ballPaddleIntersection();
        velocity.y -= 2 * velocity.y * ballBorderIntersection();
    }

    void moveBall() {
        ball.move(velocity * getDeltaTime());
    }

    void handleGoals() {
        if (ball.getPosition().x < 0) {
            gooool(Right);
        } else if (ball.getPosition().x > WINDOW_X) {
            gooool(Left);
        }
    }
private: 
    sf::Vector2f velocity;
    sf::Vertex line[2];
    std::string strScore1;
    std::string strScore2;
    sf::Text textScore1;
    sf::Text textScore2;
    sf::Font font;   
    sf::Clock clock;
};
