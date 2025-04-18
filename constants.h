#pragma once

#include <SFML/Graphics.hpp>

const int WINDOW_X = 1600;
const int WINDOW_Y = 900;

const int BallRad = 6;
sf::Vector2f PaddleSize(10, 60);

float BallSpeed = 400.f;
float BallAngle = 75.f;

sf::Vector2f velocity;