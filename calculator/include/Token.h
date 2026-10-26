#pragma once

#include <string>

namespace calc {

enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    COMMA,
    IDENTIFIER,
    CUSTOM_OP,
    END_OF_INPUT
};

struct Token {
    TokenType type{TokenType::END_OF_INPUT};
    std::string text;
    double numberValue{0.0};
    size_t position{0};

    Token() = default;
    Token(TokenType t, std::string txt, size_t pos)
        : type(t), text(std::move(txt)), numberValue(0.0), position(pos) {}
    Token(TokenType t, std::string txt, double val, size_t pos)
        : type(t), text(std::move(txt)), numberValue(val), position(pos) {}
};

} // namespace calc
