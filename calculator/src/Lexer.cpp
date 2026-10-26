#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace calc {

Lexer::Lexer(const PluginManager* pluginManager)
    : pluginManager_(pluginManager) {}

std::string Lexer::normalizeUnicode(const std::string& input) {
    std::string result;
    result.reserve(input.size());

    for (size_t i = 0; i < input.size(); ) {
        unsigned char c = static_cast<unsigned char>(input[i]);

        // Нормализация UTF-8 тире (–, —, −) в обычный минус
        if (c == 0xE2 && i + 2 < input.size()) {
            unsigned char c1 = static_cast<unsigned char>(input[i + 1]);
            unsigned char c2 = static_cast<unsigned char>(input[i + 2]);
            if ((c1 == 0x80 && (c2 == 0x93 || c2 == 0x94)) || (c1 == 0x88 && c2 == 0x92)) {
                result += '-';
                i += 3;
                continue;
            }
        }

        result += input[i];
        ++i;
    }

    return result;
}

std::vector<Token> Lexer::tokenize(const std::string& rawInput) {
    std::string input = normalizeUnicode(rawInput);
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t n = input.size();

    while (i < n) {
        char c = input[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        const size_t tokenStart = i;

        if (std::isdigit(static_cast<unsigned char>(c)) || (c == '.' && i + 1 < n && std::isdigit(static_cast<unsigned char>(input[i + 1])))) {
            size_t start = i;
            bool hasDot = false;
            while (i < n && (std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '.')) {
                if (input[i] == '.') {
                    if (hasDot) break;
                    hasDot = true;
                }
                ++i;
            }
            if (i < n && (input[i] == 'e' || input[i] == 'E')) {
                size_t expStart = i;
                ++i;
                if (i < n && (input[i] == '+' || input[i] == '-')) {
                    ++i;
                }
                if (i < n && std::isdigit(static_cast<unsigned char>(input[i]))) {
                    while (i < n && std::isdigit(static_cast<unsigned char>(input[i]))) {
                        ++i;
                    }
                } else {
                    i = expStart;
                }
            }

            std::string numStr = input.substr(start, i - start);
            try {
                size_t idx = 0;
                double val = std::stod(numStr, &idx);
                tokens.emplace_back(TokenType::NUMBER, numStr, val, tokenStart);
            } catch (...) {
                throw LexerException("Invalid number format: '" + numStr + "'", tokenStart);
            }
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(input[i])) || input[i] == '_')) {
                ++i;
            }
            std::string ident = input.substr(start, i - start);
            tokens.emplace_back(TokenType::IDENTIFIER, ident, tokenStart);
            continue;
        }

        switch (c) {
            case '+':
                tokens.emplace_back(TokenType::PLUS, "+", tokenStart);
                ++i;
                break;
            case '-':
                tokens.emplace_back(TokenType::MINUS, "-", tokenStart);
                ++i;
                break;
            case '*':
                tokens.emplace_back(TokenType::MULTIPLY, "*", tokenStart);
                ++i;
                break;
            case '/':
                tokens.emplace_back(TokenType::DIVIDE, "/", tokenStart);
                ++i;
                break;
            case '(':
                tokens.emplace_back(TokenType::LPAREN, "(", tokenStart);
                ++i;
                break;
            case ')':
                tokens.emplace_back(TokenType::RPAREN, ")", tokenStart);
                ++i;
                break;
            case ',':
                tokens.emplace_back(TokenType::COMMA, ",", tokenStart);
                ++i;
                break;
            case '^':
                tokens.emplace_back(TokenType::CUSTOM_OP, "^", tokenStart);
                ++i;
                break;
            default: {
                std::string singleChar(1, c);
                if (pluginManager_ && pluginManager_->isBinaryOperator(singleChar)) {
                    tokens.emplace_back(TokenType::CUSTOM_OP, singleChar, tokenStart);
                    ++i;
                    break;
                }
                throw LexerException("Unexpected character: '" + singleChar + "'", tokenStart);
            }
        }
    }

    tokens.emplace_back(TokenType::END_OF_INPUT, "", n);
    return tokens;
}

} // namespace calc
