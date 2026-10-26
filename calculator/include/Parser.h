#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Token.h"
#include "PluginManager.h"
#include "Exceptions.h"

namespace calc {

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual double evaluate() const = 0;
};

class NumberNode : public ASTNode {
public:
    explicit NumberNode(double value) : value_(value) {}
    double evaluate() const override { return value_; }

private:
    double value_;
};

class UnaryOpNode : public ASTNode {
public:
    UnaryOpNode(char op, std::unique_ptr<ASTNode> operand)
        : op_(op), operand_(std::move(operand)) {}

    double evaluate() const override;

private:
    char op_;
    std::unique_ptr<ASTNode> operand_;
};

class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(std::string op, std::unique_ptr<ASTNode> left,
                 std::unique_ptr<ASTNode> right, std::shared_ptr<Plugin> plugin = nullptr)
        : op_(std::move(op)), left_(std::move(left)), right_(std::move(right)),
          plugin_(std::move(plugin)) {}

    double evaluate() const override;

private:
    std::string op_;
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
    std::shared_ptr<Plugin> plugin_;
};

class FunctionCallNode : public ASTNode {
public:
    FunctionCallNode(std::string name, std::vector<std::unique_ptr<ASTNode>> args,
                     std::shared_ptr<Plugin> plugin)
        : name_(std::move(name)), args_(std::move(args)), plugin_(std::move(plugin)) {}

    double evaluate() const override;

private:
    std::string name_;
    std::vector<std::unique_ptr<ASTNode>> args_;
    std::shared_ptr<Plugin> plugin_;
};

class Parser {
public:
    Parser(std::vector<Token> tokens, const PluginManager* pluginManager = nullptr);

    std::unique_ptr<ASTNode> parse();

private:
    const Token& current() const;
    const Token& peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& errorMessage);

    std::unique_ptr<ASTNode> parseExpression(int minPrecedence = 1);
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePrimary();

    int getOperatorPrecedence(const Token& token) const;
    bool isOperatorRightAssociative(const Token& token) const;
    bool isBinaryOperatorToken(const Token& token) const;

    std::vector<Token> tokens_;
    size_t pos_{0};
    const PluginManager* pluginManager_{nullptr};
};

} // namespace calc
