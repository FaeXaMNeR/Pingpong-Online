#pragma once

#include <SFML/Graphics.hpp>

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
        void handleInput(sf::Event &event);
        int getSelectedItem() const { 
            return selectedItemIndex; 
        }

    private:
        void moveUp();
        void moveDown();

        int selectedItemIndex;
        sf::Font font;
        sf::Text menu[MENU_ITEMS_NUM];
};
