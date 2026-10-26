#include "Board.h"
#include <queue>
#include <algorithm>

Board::Board(int width, int height) : m_width(width), m_height(height), m_rng(std::random_device{}()) {
    m_grid.resize(m_height);
    for (int y = 0; y < m_height; ++y) {
        m_grid[y].resize(m_width);
    }
    initBoard();
}

void Board::initBoard() {
    int attempts = 0;
    constexpr int MAX_ATTEMPTS = 100;

    do {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                m_grid[y][x] = GemFactory::createGem(getRandomColor());
            }
        }
        attempts++;
    } while (updateState() && attempts < MAX_ATTEMPTS);
}

GemColor Board::getRandomColor() {
    std::uniform_int_distribution<int> dist(1, 5);
    return static_cast<GemColor>(dist(m_rng));
}

bool Board::isAdjacent(Point p1, Point p2) const {
    return (std::abs(p1.x - p2.x) == 1 && p1.y == p2.y) || (std::abs(p1.y - p2.y) == 1 && p1.x == p2.x);
}

bool Board::trySwap(Point p1, Point p2) {
    if (!isAdjacent(p1, p2)) return false;

    std::swap(m_grid[p1.y][p1.x], m_grid[p2.y][p2.x]);

    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(m_width, false));
    bool hasMatches = false;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (!visited[y][x] && !m_grid[y][x]->isEmpty()) {
                auto comp = findConnectedComponent({ x, y }, visited);
                if (comp.size() >= 3) hasMatches = true;
            }
        }
    }

    if (!hasMatches) {
        std::swap(m_grid[p1.y][p1.x], m_grid[p2.y][p2.x]);
        return false;
    }
    return true;
}

std::vector<Point> Board::findConnectedComponent(Point start, std::vector<std::vector<bool>>& visited) {
    std::vector<Point> component;
    std::queue<Point> q;
    GemColor targetColor = m_grid[start.y][start.x]->getColor();

    q.push(start);
    visited[start.y][start.x] = true;

    int dx[] = { 0, 0, -1, 1 };
    int dy[] = { -1, 1, 0, 0 };

    while (!q.empty()) {
        Point curr = q.front();
        q.pop();
        component.push_back(curr);

        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            if (nx >= 0 && nx < m_width && ny >= 0 && ny < m_height) {
                if (!visited[ny][nx] && m_grid[ny][nx]->getColor() == targetColor) {
                    visited[ny][nx] = true;
                    q.push({ nx, ny });
                }
            }
        }
    }
    return component;
}

std::vector<Point> Board::getNeighborhood(Point center, int radius) const {
    std::vector<Point> neighbors;
    for (int y = std::max(0, center.y - radius); y <= std::min(m_height - 1, center.y + radius); ++y) {
        for (int x = std::max(0, center.x - radius); x <= std::min(m_width - 1, center.x + radius); ++x) {
            neighbors.push_back({ x, y });
        }
    }
    return neighbors;
}

void Board::triggerBonusEffect(Point origin, GemColor originColor) {
    std::uniform_int_distribution<int> chance(1, 100);
    if (chance(m_rng) > 10) return;

    auto neighborhood = getNeighborhood(origin, 3);
    if (neighborhood.empty()) return;

    std::uniform_int_distribution<int> indexDist(0, neighborhood.size() - 1);
    Point target = neighborhood[indexDist(m_rng)];

    std::uniform_int_distribution<int> bonusDist(0, 1);
    BonusType bonus = static_cast<BonusType>(bonusDist(m_rng) + 1);

    GemColor targetColor = m_grid[target.y][target.x]->getColor();
    if (targetColor != GemColor::Empty) {
        m_grid[target.y][target.x] = GemFactory::createGem(targetColor, bonus);
    }
}

bool Board::updateState() {
    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(m_width, false));
    std::vector<std::vector<bool>> markedForDeletion(m_height, std::vector<bool>(m_width, false));
    bool hasChanges = false;

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (!visited[y][x] && !m_grid[y][x]->isEmpty()) {
                auto component = findConnectedComponent({ x, y }, visited);
                if (component.size() >= 3) {
                    hasChanges = true;
                    for (const auto& p : component) {
                        markedForDeletion[p.y][p.x] = true;
                    }
                }
            }
        }
    }

    if (!hasChanges) return false;

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (markedForDeletion[y][x]) {
                GemColor originalColor = m_grid[y][x]->getColor();

                m_grid[y][x]->activate(*this, { x, y });

                triggerBonusEffect({ x, y }, originalColor);
            }
        }
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (markedForDeletion[y][x] || m_grid[y][x]->getColor() == GemColor::Empty) {
                m_grid[y][x] = GemFactory::createGem(GemColor::Empty);
            }
        }
    }

    for (int x = 0; x < m_width; ++x) {
        int emptyRow = m_height - 1;
        for (int y = m_height - 1; y >= 0; --y) {
            if (!m_grid[y][x]->isEmpty()) {
                if (y != emptyRow) {
                    m_grid[emptyRow][x] = std::move(m_grid[y][x]);
                    m_grid[y][x] = GemFactory::createGem(GemColor::Empty);
                }
                emptyRow--;
            }
        }
        for (int y = emptyRow; y >= 0; --y) {
            m_grid[y][x] = GemFactory::createGem(getRandomColor());
        }
    }

    return true;
}