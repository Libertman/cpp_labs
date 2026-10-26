#pragma once

#include <memory>
#include <string>
#include <vector>
#include "PluginInterface.h"
#include "DynamicLibrary.h"
#include "Exceptions.h"

namespace calc {

class Plugin {
public:
    using NameFunc = const char* (*)();
    using TypeFunc = int (*)();
    using ArityFunc = int (*)();
    using PrecedenceFunc = int (*)();
    using AssociativityFunc = int (*)();
    using DescriptionFunc = const char* (*)();
    using CalculateFunc = int (*)(const double*, int, double*, char*, int);

    explicit Plugin(std::unique_ptr<DynamicLibrary> lib);
    ~Plugin() = default;

    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin(Plugin&&) noexcept = default;
    Plugin& operator=(Plugin&&) noexcept = default;

    const std::string& getName() const noexcept { return name_; }
    PluginType getType() const noexcept { return type_; }
    int getArity() const noexcept { return arity_; }
    int getPrecedence() const noexcept { return precedence_; }
    bool isRightAssociative() const noexcept { return rightAssociative_; }
    const std::string& getDescription() const noexcept { return description_; }
    const std::filesystem::path& getLibraryPath() const noexcept { return lib_->getPath(); }

    double execute(const std::vector<double>& args) const;

private:
    std::unique_ptr<DynamicLibrary> lib_;
    std::string name_;
    PluginType type_{PLUGIN_TYPE_UNARY_FUNC};
    int arity_{1};
    int precedence_{0};
    bool rightAssociative_{false};
    std::string description_;

    CalculateFunc calculateFunc_{nullptr};
};

} // namespace calc
