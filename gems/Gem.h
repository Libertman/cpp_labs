#pragma once
#include "Enums.h"
#include <memory>


class Board;
struct Point;

class Gem {
public:
    virtual ~Gem() = default;

    virtual GemColor getColor() const = 0;
    virtual void setColor(GemColor color) = 0;
    virtual BonusType getBonusType() const = 0;

    virtual void activate(Board& board, Point myPos) = 0;

    virtual bool isEmpty() const { return getColor() == GemColor::Empty; }
};

class BaseGem : public Gem {
protected:
    GemColor m_color;
public:
    BaseGem(GemColor color);
    GemColor getColor() const override { return m_color; }
    void setColor(GemColor color) override { m_color = color; }
};

class NormalGem : public BaseGem {
public:
    using BaseGem::BaseGem;
    BonusType getBonusType() const override { return BonusType::None; }
    void activate(Board& board, Point myPos) override;
};

class RecolorGem : public BaseGem {
public:
    using BaseGem::BaseGem;
    BonusType getBonusType() const override { return BonusType::Recolor; }
    void activate(Board& board, Point myPos) override;
};

class BombGem : public BaseGem {
public:
    using BaseGem::BaseGem;
    BonusType getBonusType() const override { return BonusType::Bomb; }
    void activate(Board& board, Point myPos) override;
};

class GemFactory {
public:
    static std::unique_ptr<Gem> createGem(GemColor color, BonusType bonus = BonusType::None);
};