#pragma once
#include <vector>
#include <random>
#include "Gem.h"

struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

class Board {
public:
    Board(int width, int height);

    void initBoard();
    bool trySwap(Point p1, Point p2);
    bool updateState();

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    const Gem& getGem(int x, int y) const { return *m_grid[y][x]; }

    std::mt19937& getRng() { return m_rng; }
    void clearCell(Point p) { m_grid[p.y][p.x] = GemFactory::createGem(GemColor::Empty); }
    void setCellColor(Point p, GemColor color) { m_grid[p.y][p.x]->setColor(color); }
    std::vector<Point> getNeighborhood(Point center, int radius) const;

private:
    int m_width;
    int m_height;
    std::vector<std::vector<std::unique_ptr<Gem>>> m_grid;
    std::mt19937 m_rng;

    GemColor getRandomColor();
    bool isAdjacent(Point p1, Point p2) const;

    std::vector<Point> findConnectedComponent(Point start, std::vector<std::vector<bool>>& visited);
    void triggerBonusEffect(Point origin, GemColor originColor);
};
