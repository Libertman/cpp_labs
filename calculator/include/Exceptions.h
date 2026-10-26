#pragma once

#include <stdexcept>
#include <string>

namespace calc {

class CalculatorException : public std::runtime_error {
public:
    explicit CalculatorException(const std::string& message)
        : std::runtime_error(message) {}
};

class LexerException : public CalculatorException {
public:
    LexerException(const std::string& message, size_t position)
        : CalculatorException("Lexer error at position " + std::to_string(position) + ": " + message),
          position_(position) {}

    size_t getPosition() const noexcept { return position_; }

private:
    size_t position_;
};

class ParserException : public CalculatorException {
public:
    ParserException(const std::string& message, size_t position)
        : CalculatorException("Syntax error at position " + std::to_string(position) + ": " + message),
          position_(position) {}

    size_t getPosition() const noexcept { return position_; }

private:
    size_t position_;
};

class EvaluationException : public CalculatorException {
public:
    explicit EvaluationException(const std::string& message)
        : CalculatorException("Calculation error: " + message) {}
};

class DynamicLibraryException : public CalculatorException {
public:
    explicit DynamicLibraryException(const std::string& message)
        : CalculatorException("Library error: " + message) {}
};

class InvalidPluginException : public CalculatorException {
public:
    explicit InvalidPluginException(const std::string& message)
        : CalculatorException("Invalid plugin: " + message) {}
};

class PluginExecutionException : public EvaluationException {
public:
    PluginExecutionException(const std::string& pluginName, const std::string& detail)
        : EvaluationException("plugin '" + pluginName + "' threw exception: " + detail),
          pluginName_(pluginName) {}

    const std::string& getPluginName() const noexcept { return pluginName_; }

private:
    std::string pluginName_;
};

} // namespace calc
