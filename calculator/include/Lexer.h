#pragma once

#include <string>
#include <vector>
#include "Token.h"
#include "PluginManager.h"
#include "Exceptions.h"

namespace calc {

class Lexer {
public:
    explicit Lexer(const PluginManager* pluginManager = nullptr);

    std::vector<Token> tokenize(const std::string& input);

private:
    static std::string normalizeUnicode(const std::string& input);

    const PluginManager* pluginManager_{nullptr};
};

} // namespace calc
