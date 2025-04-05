#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <math.h>
#include "Menu.h"


int main() {
    const int screenX = 600;
    const int screenY = 400;
    sf::Vector2f paddleSize(10, 60);
    const int ballRad = 5;
    float ballSpeed = 400.f;
    float ballAngle = 75.f;
    sf::Vector2f velocity;

    sf::RenderWindow window(sf::VideoMode(screenX,screenY), "PingPong", sf::Style::Titlebar | sf::Style::Close);
    Menu menu(window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);

    sf::RectangleShape paddle1;
    paddle1.setSize(paddleSize);
    paddle1.setFillColor(sf::Color::White);
    paddle1.setPosition(paddleSize.x,(screenY/2) - paddleSize.y);
    paddle1.setOrigin(sf::Vector2f(paddle1.getSize().x / 2, paddle1.getSize().y / 2));

    sf::RectangleShape paddle2;
    paddle2.setSize(paddleSize);
    paddle2.setFillColor(sf::Color::White);
    paddle2.setPosition(screenX-(paddleSize.x*2), (screenY/2)-paddleSize.y);

    sf::RectangleShape topBorder;
    topBorder.setSize(sf::Vector2f(screenX,paddleSize.x));
    topBorder.setPosition(0, 0);

    sf::RectangleShape botBorder;
    botBorder.setSize(sf::Vector2f(screenX, paddleSize.x));
    botBorder.setPosition(0, screenY-paddleSize.x);

    sf::Font font;

    font.loadFromFile("digit.ttf");

    //bool Sound;
    sf::SoundBuffer bufferPaddle;
    if (!bufferPaddle.loadFromFile("res/SoundFX/paddle.wav")) {}
    sf::Sound soundPaddle;
    soundPaddle.setBuffer(bufferPaddle);

    sf::SoundBuffer bufferScore;
    if (!bufferScore.loadFromFile("res/SoundFX/score.wav")) {}
    sf::Sound soundScore;
    soundScore.setBuffer(bufferScore);

    sf::SoundBuffer bufferWall;
    if (!bufferWall.loadFromFile("res/SoundFX/wall.wav")) {}
    sf::Sound soundWall;
    soundWall.setBuffer(bufferWall);

    int intScore1 = 0;
    sf::Text textScore1;
    std::string strScore1 = std::to_string(intScore1);
    textScore1.setString(strScore1);
    textScore1.setFont(font);
    textScore1.setCharacterSize(paddleSize.y*1.5f);
    textScore1.setPosition(sf::Vector2f(screenX/5.f, 0.f));

    int intScore2 = 0;
    sf::Text textScore2;
    std::string strScore2 = std::to_string(intScore2);
    textScore2.setString(strScore2);
    textScore2.setFont(font);
    textScore2.setCharacterSize(paddleSize.y*1.5f);
    textScore2.setPosition(sf::Vector2f(365.f, 0.f));

    //add sf::VertexArray, dashed line splitting map into 2

    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(screenX/2+1,0)),
        sf::Vertex(sf::Vector2f(screenX/2+1,screenY))
    };

    sf::CircleShape ball(ballRad);
    ball.setPointCount(10);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(sf::Vector2f(screenX/2.f, screenY/2.f));

    //run prog as long as the window is open
    sf::Clock clock;
    bool Playing = false;
    while (window.isOpen())
    {
        if (Playing) 
        {   
            float deltaTime = clock.restart().asSeconds();
            float factor = deltaTime * ballSpeed;
            velocity.x = std::cos(ballAngle)*factor;
            velocity.y = std::sin(ballAngle)*factor;

            ball.move(velocity.x, velocity.y);

            if ((ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) || ball.getGlobalBounds().intersects(paddle1.getGlobalBounds()))
            {
                ball.move(-velocity.x, -velocity.y);
                velocity.x = -(velocity.x);
                ballSpeed = -ballSpeed;
                ballAngle = -ballAngle;
                soundPaddle.play();
            }


            if (ball.getGlobalBounds().intersects(topBorder.getGlobalBounds()) || ball.getGlobalBounds().intersects(botBorder.getGlobalBounds()))
            {
                velocity.x = -velocity.x;
                ballAngle = -ballAngle;
                soundWall.play();
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && !(paddle1.getGlobalBounds().intersects(topBorder.getGlobalBounds()))) //only move up if not touching top border
                paddle1.move(sf::Vector2f(0, -(paddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && !(paddle1.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle1.move(sf::Vector2f(0, paddleSize.x / 2));

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !(paddle2.getGlobalBounds().intersects(topBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, -(paddleSize.x / 2)));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && !(paddle2.getGlobalBounds().intersects(botBorder.getGlobalBounds())))
                paddle2.move(sf::Vector2f(0, paddleSize.x / 2));

            if (ball.getPosition().x > paddle2.getPosition().x + 2*ballRad)
            {
                intScore1++;
                strScore1 = std::to_string(intScore1);
                textScore1.setString(strScore1);
                ball.setPosition(sf::Vector2f(screenX / 2, screenY / 2));
                soundScore.play();
            }

            if (ball.getPosition().x < paddle1.getPosition().x - 2*ballRad)
            {
                intScore2++;
                strScore2 = std::to_string(intScore2);
                textScore2.setString(strScore2);
                ball.setPosition(sf::Vector2f(screenX/2, screenY/2));
                soundScore.play();
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

        if (!Playing)
        {
            window.clear(sf::Color::Black);
            menu.draw(window);
            window.display();
        }



        //check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {

            //"close requested" event: we close the window

            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();

            case sf::Event::KeyPressed:
                switch (event.key.code)
            case sf::Keyboard::Escape:
                Playing = false;
                break;

            case sf::Event::KeyReleased:  //menu controls
                switch (event.key.code)
                {
                case sf::Keyboard::Up:  
                case sf::Keyboard::W:
                    menu.MoveUp();
                    break;
                case sf::Keyboard::Down:
                case sf::Keyboard::S:
                    menu.MoveDown();
                    break;

                case sf::Keyboard::Return:
                    switch (menu.GetPressedItem())
                    {
                    case 0:
                        Playing = true;
                        break;
                    case 1:
                        //open sound menu
                        //Sound = !Sound; add to sound menu
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
    return EXIT_SUCCESS;
}