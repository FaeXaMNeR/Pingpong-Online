#include <SFML/Network.hpp>

#include "mainmenu.hpp"
#include "constants.hpp"
#include "pingpong.hpp"

Menu::Menu() {
    font.loadFromFile("pong.ttf");

    for (int i = 0; i < MENU_ITEMS_NUM; ++i) {
        menu[i].setFont(font);
        menu[i].setFillColor(sf::Color(128, 128, 128));
        menu[i].setCharacterSize(WINDOW_Y / 30);
    }

    menu[PlayOffline    ].setString("Play Offline");
    menu[PlayOffline    ].setPosition(sf::Vector2f(WINDOW_X / 10, 1 * WINDOW_Y/(MENU_ITEMS_NUM+1)));
    
    menu[LaunchServer   ].setString("Launch Server");
    menu[LaunchServer   ].setPosition(sf::Vector2f(WINDOW_X / 10, 2 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[JoinServer     ].setString("Join Server");
    menu[JoinServer     ].setPosition(sf::Vector2f(WINDOW_X / 10, 3 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[Exit           ].setString("Exit");
    menu[Exit           ].setPosition(sf::Vector2f(WINDOW_X / 10, 4 * WINDOW_Y/(MENU_ITEMS_NUM+1)));

    menu[PlayOffline].setFillColor(sf::Color::White); // Выбранный пункт по умолчанию
    selectedItemIndex = 0;
}

Menu::~Menu() {}

void Menu::draw(sf::RenderWindow &window) {
    window.clear(sf::Color::Black);
    for (int i = 0; i < MENU_ITEMS_NUM; ++i) {
        window.draw(menu[i]);
    }
}

GameMode Menu::handleInput(sf::Event &event, sf::RenderWindow &window) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up: {
                moveUp();
                break;
            }

            case sf::Keyboard::Down: {
                moveDown();
                break;
            }

            case sf::Keyboard::Enter: {
                switch (selectedItemIndex) {
                    case PlayOffline: {
                        return OfflineGame;
                        break;
                    }

                    case LaunchServer: {
                        return Server;
                        break;
                    }

                    case JoinServer: {
                        return Client;
                        break;
                    }

                    case Exit: {
                        window.close();
                        return None;
                        break;
                    }

                    default: {
                        break;
                    }
                }
            }

            default: {
                break;
            }
        }
    }

    return MainMenu;
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
