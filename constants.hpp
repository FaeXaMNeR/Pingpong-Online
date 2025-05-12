#pragma once

#include <SFML/Graphics.hpp>

const int WINDOW_X = 1600;
const int WINDOW_Y = 900;

const int BallRad = 6;
sf::Vector2f PaddleSize(10, 60);

float initialBallSpeed = 550.f;
float initialBallAngle = 75.f;

sf::Vector2f velocity;



const unsigned short PORT = 50001;
const float TICK_RATE = 120.f;
const int BUFFER = 512;