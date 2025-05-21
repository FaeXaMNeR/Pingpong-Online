#pragma once

#include <SFML/Graphics.hpp>
#include "pingpong.hpp"

enum MenuItem {
    PlayOffline,
    LaunchServer,
    JoinServer,
    Exit
};

const int MENU_ITEMS_NUM = 4;

class Menu {
    public:
        Menu();
        ~Menu();

        void draw(sf::RenderWindow &window);
        GameMode handleInput(sf::Event &event, sf::RenderWindow &window);
        int getSelectedItem() const { 
            return selectedItemIndex; 
        }

    private:
        void moveUp();
        void moveDown();

        int selectedItemIndex;
        sf::Font font;
        sf::Text menu[MENU_ITEMS_NUM];
        sf::Text logo;
        sf::Text rights;
};
