#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Board.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    Point mapPixelToGrid(sf::Vector2i mousePos);
    sf::Color convertColor(GemColor color);

private:
    const int CELL_SIZE = 64;
    const int BOARD_WIDTH = 8;
    const int BOARD_HEIGHT = 8;

    sf::RenderWindow m_window;
    std::unique_ptr<Board> m_board;

    bool m_hasSelection;
    Point m_selectedPoint;
};
