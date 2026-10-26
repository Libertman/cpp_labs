#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include "PluginManager.h"
#include "Exceptions.h"

namespace calc {

class Calculator {
public:
    Calculator() = default;
    ~Calculator() = default;

    Calculator(const Calculator&) = delete;
    Calculator& operator=(const Calculator&) = delete;
    Calculator(Calculator&&) noexcept = default;
    Calculator& operator=(Calculator&&) noexcept = default;

    size_t loadPlugins(const std::filesystem::path& pluginsDir = "./plugins");
    double evaluate(const std::string& expression);

    const PluginManager& getPluginManager() const noexcept { return pluginManager_; }
    PluginManager& getPluginManager() noexcept { return pluginManager_; }

private:
    PluginManager pluginManager_;
};

} // namespace calc
