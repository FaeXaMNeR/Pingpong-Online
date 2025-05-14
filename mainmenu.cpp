#include <SFML/Network.hpp>
#include "mainmenu.hpp"
#include "constants.hpp"

Menu::Menu() {
    font.loadFromFile("pong.ttf");

    for (int i = 0; i < MENU_ITEMS_NUM; ++i) {
        menu[i].setFont(font);
        menu[i].setFillColor(sf::Color(128, 128, 128));
        menu[i].setCharacterSize(WINDOW_Y / 30);
    }

    menu[PlayOffline].setString("Play Offline");
    menu[PlayOffline].setPosition(sf::Vector2f(WINDOW_X / 10, 1 * WINDOW_Y/(MENU_ITEMS_NUM+1)));
    

    menu[LaunchServer].setString("Launch Server");
    menu[LaunchServer].setPosition(sf::Vector2f(WINDOW_X / 10, 2 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[JoinServer].setString("Join Server");
    menu[JoinServer].setPosition(sf::Vector2f(WINDOW_X / 10, 3 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[Exit].setString("Exit");
    menu[Exit].setPosition(sf::Vector2f(WINDOW_X / 10, 4 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[PlayOffline].setFillColor(sf::Color::White); // Выбранный пункт по умолчанию
    selectedItemIndex = 0;
}

Menu::~Menu() {}

void Menu::draw(sf::RenderWindow &window) {
    for (int i = 0; i < MENU_ITEMS_NUM; ++i) {
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
    if (selectedItemIndex + 1 < MENU_ITEMS_NUM) {
        menu[selectedItemIndex].setFillColor(sf::Color(128, 128, 128));
        selectedItemIndex++;
        menu[selectedItemIndex].setFillColor(sf::Color::White);
    }
}
