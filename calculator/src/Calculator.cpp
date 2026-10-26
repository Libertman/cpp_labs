#include "Calculator.h"
#include "Lexer.h"
#include "Parser.h"

namespace calc {

size_t Calculator::loadPlugins(const std::filesystem::path& pluginsDir) {
    return pluginManager_.loadPluginsFromDirectory(pluginsDir);
}

double Calculator::evaluate(const std::string& expression) {
    Lexer lexer(&pluginManager_);
    auto tokens = lexer.tokenize(expression);

    Parser parser(std::move(tokens), &pluginManager_);
    auto ast = parser.parse();

    if (!ast) {
        throw EvaluationException("Failed to generate AST for expression: " + expression);
    }

    return ast->evaluate();
}

} // namespace calc
