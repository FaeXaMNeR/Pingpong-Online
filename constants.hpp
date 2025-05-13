#pragma once

#include <SFML/Graphics.hpp>

const int WINDOW_X = 1600;
const int WINDOW_Y = 900;

const int BallRad = WINDOW_Y * 2 / 300;
sf::Vector2f PaddleSize(WINDOW_X / 160, WINDOW_Y * 2 / 30);

sf::Vector2f initialVelocity(507.f, -213.3f);

const unsigned short PORT = 50001;
const float TICK_RATE = 120.f;
const int BUFFER = 512;
