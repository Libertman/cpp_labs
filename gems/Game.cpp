#include "Game.h"

Game::Game() : m_hasSelection(false), m_selectedPoint({ -1, -1 }) {
    m_window.create(sf::VideoMode(BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE), "GEMS Game");
    m_board = std::make_unique<Board>(BOARD_WIDTH, BOARD_HEIGHT);
    m_window.setFramerateLimit(60);
}

void Game::run() {
    while (m_window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            Point clickedPoint = mapPixelToGrid(sf::Mouse::getPosition(m_window));

            if (clickedPoint.x >= 0 && clickedPoint.x < BOARD_WIDTH && clickedPoint.y >= 0 && clickedPoint.y < BOARD_HEIGHT) {
                if (!m_hasSelection) {
                    m_selectedPoint = clickedPoint;
                    m_hasSelection = true;
                }
                else {
                    m_board->trySwap(m_selectedPoint, clickedPoint);
                        m_hasSelection = false;
                }
            }
        }
    }
}

void Game::update() {
    while (m_board->updateState()) {}
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));

    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            const Gem& gem = m_board->getGem(x, y);
            if (gem.isEmpty()) continue;

            sf::RectangleShape shape(sf::Vector2f(CELL_SIZE - 4, CELL_SIZE - 4));
            shape.setPosition(x * CELL_SIZE + 2, y * CELL_SIZE + 2);
            shape.setFillColor(convertColor(gem.getColor()));

            if (gem.getBonusType() == BonusType::Bomb) {
                shape.setOutlineThickness(2);
                shape.setOutlineColor(sf::Color::Red);
            }
            else if (gem.getBonusType() == BonusType::Recolor) {
                shape.setOutlineThickness(2);
                shape.setOutlineColor(sf::Color::Cyan);
            }

            if (m_hasSelection && m_selectedPoint.x == x && m_selectedPoint.y == y) {
                shape.setOutlineThickness(3);
                shape.setOutlineColor(sf::Color::White);
            }

            m_window.draw(shape);
        }
    }
    m_window.display();
}

Point Game::mapPixelToGrid(sf::Vector2i mousePos) {
    return { mousePos.x / CELL_SIZE, mousePos.y / CELL_SIZE };
}

sf::Color Game::convertColor(GemColor color) {
    switch (color) {
    case GemColor::Red:    return sf::Color(220, 50, 50);
    case GemColor::Green:  return sf::Color(50, 200, 50);
    case GemColor::Blue:   return sf::Color(50, 50, 250);
    case GemColor::Yellow: return sf::Color(240, 240, 50);
    case GemColor::Purple: return sf::Color(150, 50, 200);
    default:               return sf::Color::Black;
    }
}
