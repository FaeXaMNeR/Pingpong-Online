#pragma once
#include <SFML/Graphics.hpp>

#define MAX_NUMBER_ITEMS 4

class Menu {
    public:
        Menu(float width, float height);
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
        sf::Text menu[MAX_NUMBER_ITEMS];
};