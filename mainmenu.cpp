#include <SFML/Network.hpp>

#include "mainmenu.hpp"

Menu::Menu(float width, float height) {
    font.loadFromFile("pong.ttf");

    for (int i = 0; i < MAX_NUMBER_ITEMS; ++i) {
        menu[i].setFont(font);
        menu[i].setFillColor(sf::Color(128, 128, 128));
        menu[i].setCharacterSize(30);
    }

    menu[0].setString("Play Offline");
    menu[0].setPosition(sf::Vector2f(width / 10, height/(MAX_NUMBER_ITEMS+1)*1));

    menu[1].setString("Launch Server");
    menu[1].setPosition(sf::Vector2f(width / 10, height/(MAX_NUMBER_ITEMS+1)*2));

    menu[2].setString("Join Server");
    menu[2].setPosition(sf::Vector2f(width / 10, height/(MAX_NUMBER_ITEMS+1)*3));

    menu[3].setString("Exit");
    menu[3].setPosition(sf::Vector2f(width / 10, height/(MAX_NUMBER_ITEMS+1)*4));

    menu[0].setFillColor(sf::Color::White); // Выбранный пункт по умолчанию
    selectedItemIndex = 0;
}

Menu::~Menu() {}

void Menu::draw(sf::RenderWindow &window) {
    for (int i = 0; i < MAX_NUMBER_ITEMS; ++i) {
        window.draw(menu[i]);
    }
}

void Menu::handleInput(sf::Event &event) {
    if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                moveUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                moveDown();
                break;
                
            default:
                break;
        }
    }
}

void Menu::moveUp() {
    if (selectedItemIndex - 1 >= 0) {
        menu[selectedItemIndex].setFillColor(sf::Color(128, 128, 128));
        selectedItemIndex--;
        menu[selectedItemIndex].setFillColor(sf::Color::White);
    }
}

void Menu::moveDown() {
    if (selectedItemIndex + 1 < MAX_NUMBER_ITEMS) {
        menu[selectedItemIndex].setFillColor(sf::Color(128, 128, 128));
        selectedItemIndex++;
        menu[selectedItemIndex].setFillColor(sf::Color::White);
    }
}
