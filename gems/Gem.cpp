#include "Gem.h"
#include "Board.h"
#include <memory>

BaseGem::BaseGem(GemColor color) : m_color(color) {}

void NormalGem::activate(Board& board, Point myPos) {}

void RecolorGem::activate(Board& board, Point myPos) {
    GemColor myColor = getColor();
    auto neighborhood = board.getNeighborhood(myPos, 3);

    std::vector<Point> nonNeighbors;
    for (const auto& p : neighborhood) {
        int manhattanDistance = std::abs(p.x - myPos.x) + std::abs(p.y - myPos.y);
        if (manhattanDistance > 1) {
            nonNeighbors.push_back(p);
        }
    }

    std::shuffle(nonNeighbors.begin(), nonNeighbors.end(), board.getRng());
    int toRecolor = std::min(2, static_cast<int>(nonNeighbors.size()));
    for (int i = 0; i < toRecolor; ++i) {
        board.setCellColor(nonNeighbors[i], myColor);
    }
}

void BombGem::activate(Board& board, Point myPos) {
    std::vector<Point> allPoints;
    for (int y = 0; y < board.getHeight(); ++y) {
        for (int x = 0; x < board.getWidth(); ++x) {
            if (!(x == myPos.x && y == myPos.y)) {
                allPoints.push_back({ x, y });
            }
        }
    }

    std::shuffle(allPoints.begin(), allPoints.end(), board.getRng());
    int extraDestroy = std::min(4, static_cast<int>(allPoints.size()));
    for (int i = 0; i < extraDestroy; ++i) {
        board.clearCell(allPoints[i]);
    }
}

std::unique_ptr<Gem> GemFactory::createGem(GemColor color, BonusType bonus) {
    switch (bonus) {
    case BonusType::Recolor:
        return std::make_unique<RecolorGem>(color);
    case BonusType::Bomb:
        return std::make_unique<BombGem>(color);
    default:
        return std::make_unique<NormalGem>(color);
    }
}
