#include "Parser.h"
#include <cmath>

namespace calc {

// --- AST Nodes Evaluation ---

double UnaryOpNode::evaluate() const {
    double val = operand_->evaluate();
    if (op_ == '-') {
        return -val;
    }
    return val;
}

double BinaryOpNode::evaluate() const {
    double l = left_->evaluate();
    double r = right_->evaluate();

    if (op_ == "+") {
        return l + r;
    }
    if (op_ == "-") {
        return l - r;
    }
    if (op_ == "*") {
        return l * r;
    }
    if (op_ == "/") {
        if (std::abs(r) == 0.0) {
            throw EvaluationException("Division by zero");
        }
        return l / r;
    }

    if (plugin_) {
        return plugin_->execute({l, r});
    }

    if (op_ == "^") {
        throw EvaluationException("Operator '^' is not available. Please load the 'funcdeg' plugin.");
    }

    throw EvaluationException("Unsupported binary operator: '" + op_ + "'");
}

double FunctionCallNode::evaluate() const {
    if (!plugin_) {
        throw EvaluationException("Unknown function '" + name_ +
                                  "'. Function is not loaded from any plugin.");
    }

    std::vector<double> evaluatedArgs;
    evaluatedArgs.reserve(args_.size());
    for (const auto& arg : args_) {
        evaluatedArgs.push_back(arg->evaluate());
    }

    return plugin_->execute(evaluatedArgs);
}

Parser::Parser(std::vector<Token> tokens, const PluginManager* pluginManager)
    : tokens_(std::move(tokens)), pluginManager_(pluginManager) {}

const Token& Parser::current() const {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_];
    }
    return tokens_.back();
}

const Token& Parser::peek() const {
    if (pos_ + 1 < tokens_.size()) {
        return tokens_[pos_ + 1];
    }
    return tokens_.back();
}

Token Parser::advance() {
    Token tok = current();
    if (pos_ < tokens_.size()) {
        ++pos_;
    }
    return tok;
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& errorMessage) {
    if (current().type == type) {
        return advance();
    }
    throw ParserException(errorMessage + " (got '" + current().text + "')", current().position);
}

int Parser::getOperatorPrecedence(const Token& token) const {
    if (token.type == TokenType::PLUS || token.type == TokenType::MINUS) {
        return 1;
    }
    if (token.type == TokenType::MULTIPLY || token.type == TokenType::DIVIDE) {
        return 2;
    }
    if (token.type == TokenType::CUSTOM_OP) {
        if (pluginManager_) {
            auto plugin = pluginManager_->getBinaryOperator(token.text);
            if (plugin) {
                return plugin->getPrecedence();
            }
        }
        if (token.text == "^") {
            return 3;
        }
        return 1;
    }
    return 0;
}

bool Parser::isOperatorRightAssociative(const Token& token) const {
    if (token.type == TokenType::CUSTOM_OP) {
        if (pluginManager_) {
            auto plugin = pluginManager_->getBinaryOperator(token.text);
            if (plugin) {
                return plugin->isRightAssociative();
            }
        }
        if (token.text == "^") {
            return true;
        }
    }
    return false;
}

bool Parser::isBinaryOperatorToken(const Token& token) const {
    if (token.type == TokenType::PLUS || token.type == TokenType::MINUS ||
        token.type == TokenType::MULTIPLY || token.type == TokenType::DIVIDE) {
        return true;
    }
    if (token.type == TokenType::CUSTOM_OP) {
        if (token.text == "^") {
            return true;
        }
        if (pluginManager_ && pluginManager_->isBinaryOperator(token.text)) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parse() {
    if (current().type == TokenType::END_OF_INPUT) {
        throw ParserException("Expression is empty", current().position);
    }

    auto expr = parseExpression(1);

    if (current().type != TokenType::END_OF_INPUT) {
        throw ParserException("Unexpected token after expression: '" + current().text + "'",
                              current().position);
    }

    return expr;
}

std::unique_ptr<ASTNode> Parser::parseExpression(int minPrecedence) {
    auto left = parseUnary();

    while (isBinaryOperatorToken(current())) {
        int prec = getOperatorPrecedence(current());
        if (prec < minPrecedence) {
            break;
        }

        Token opToken = advance();
        bool rightAssoc = isOperatorRightAssociative(opToken);
        int nextMinPrec = rightAssoc ? prec : (prec + 1);

        auto right = parseExpression(nextMinPrec);

        std::shared_ptr<Plugin> plugin = nullptr;
        if (pluginManager_) {
            plugin = pluginManager_->getBinaryOperator(opToken.text);
        }

        left = std::make_unique<BinaryOpNode>(opToken.text, std::move(left),
                                             std::move(right), std::move(plugin));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (current().type == TokenType::PLUS) {
        advance();
        return std::make_unique<UnaryOpNode>('+', parseUnary());
    }
    if (current().type == TokenType::MINUS) {
        advance();
        return std::make_unique<UnaryOpNode>('-', parseUnary());
    }

    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (current().type == TokenType::NUMBER) {
        Token numTok = advance();
        return std::make_unique<NumberNode>(numTok.numberValue);
    }

    if (current().type == TokenType::LPAREN) {
        advance();
        auto expr = parseExpression(1);
        expect(TokenType::RPAREN, "Expected ')' to close parenthesis");
        return expr;
    }

    if (current().type == TokenType::IDENTIFIER) {
        Token idTok = advance();
        expect(TokenType::LPAREN, "Expected '(' after function name '" + idTok.text + "'");

        std::vector<std::unique_ptr<ASTNode>> args;
        if (current().type != TokenType::RPAREN) {
            args.push_back(parseExpression(1));
            while (match(TokenType::COMMA)) {
                args.push_back(parseExpression(1));
            }
        }
        expect(TokenType::RPAREN, "Expected ')' after function arguments");

        std::shared_ptr<Plugin> plugin = nullptr;
        if (pluginManager_) {
            plugin = pluginManager_->getPlugin(idTok.text);
        }

        return std::make_unique<FunctionCallNode>(idTok.text, std::move(args), std::move(plugin));
    }

    throw ParserException("Expected number, function or '('", current().position);
}

} // namespace calc
