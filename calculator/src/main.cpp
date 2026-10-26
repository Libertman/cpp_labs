#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>
#include "Calculator.h"

namespace {

std::filesystem::path getExecutableDir(const char* argv0) {
    std::error_code ec;
    auto p = std::filesystem::absolute(argv0, ec);
    if (!ec && p.has_parent_path()) {
        return p.parent_path();
    }
    return std::filesystem::current_path();
}

void printBanner(const calc::Calculator& calculator) {
    std::cout << "========================================" << std::endl;
    std::cout << "       Dynamic Plugin Calculator        " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Standard operations: + - * / ( )" << std::endl;

    const auto& plugins = calculator.getPluginManager().getPlugins();
    if (plugins.empty()) {
        std::cout << "Loaded plugins: none (directory ./plugins is empty or missing)" << std::endl;
    } else {
        std::cout << "Loaded dynamic plugins (" << plugins.size() << "):" << std::endl;
        for (const auto& [name, plugin] : plugins) {
            std::cout << "  - " << name;
            if (plugin->getType() == PLUGIN_TYPE_BINARY_OP) {
                std::cout << " (binary operator, precedence: " << plugin->getPrecedence() << ")";
            } else {
                std::cout << " (function, arity: " << plugin->getArity() << ")";
            }
            if (!plugin->getDescription().empty()) {
                std::cout << " : " << plugin->getDescription();
            }
            std::cout << std::endl;
        }
    }
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Type an expression to calculate (e.g. 16 + 4 * (3 - 1) or 2^4 + sin(90))." << std::endl;
    std::cout << "Type 'exit' or 'quit' to quit." << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace

int main(int argc, char* argv[]) {
    calc::Calculator calculator;

    std::filesystem::path pluginsDir = "./plugins";
    std::string expr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--plugins" || arg == "-p") && i + 1 < argc) {
            pluginsDir = argv[++i];
        } else {
            if (!expr.empty()) expr += " ";
            expr += arg;
        }
    }

    if (!std::filesystem::exists(pluginsDir) && pluginsDir == "./plugins") {
        auto exeDir = getExecutableDir(argv[0]);
        if (std::filesystem::exists(exeDir / "plugins")) {
            pluginsDir = exeDir / "plugins";
        }
    }

    calculator.loadPlugins(pluginsDir);

    if (!expr.empty()) {
        try {
            double result = calculator.evaluate(expr);
            std::cout << result << std::endl;
            return 0;
        } catch (const calc::CalculatorException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            return 1;
        }
    }

    printBanner(calculator);

    std::string line;
    while (true) {
        std::cout << "\nВвод: ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) {
            continue;
        }
        if (line == "exit" || line == "quit" || line == "q") {
            std::cout << "Exiting calculator. Goodbye!" << std::endl;
            break;
        }

        try {
            double result = calculator.evaluate(line);
            std::cout << "Вывод: " << result << std::endl;
        } catch (const calc::CalculatorException& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Непредвиденная ошибка: " << e.what() << std::endl;
        }
    }

    return 0;
}
