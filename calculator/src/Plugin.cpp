#include "Plugin.h"
#include <array>
#include <iostream>

namespace calc {

Plugin::Plugin(std::unique_ptr<DynamicLibrary> lib)
    : lib_(std::move(lib)) {
    if (!lib_ || !lib_->isLoaded()) {
        throw InvalidPluginException("Dynamic library handle is null or unloaded.");
    }

    const auto libPathStr = lib_->getPath().string();

    auto nameFn = reinterpret_cast<NameFunc>(lib_->getSymbol("get_plugin_name"));
    if (!nameFn) {
        throw InvalidPluginException("Missing required symbol 'get_plugin_name' in " + libPathStr);
    }
    const char* rawName = nameFn();
    if (!rawName || rawName[0] == '\0') {
        throw InvalidPluginException("Plugin in " + libPathStr + " returned null or empty name.");
    }
    name_ = rawName;

    calculateFunc_ = reinterpret_cast<CalculateFunc>(lib_->getSymbol("calculate"));
    if (!calculateFunc_) {
        throw InvalidPluginException("Missing required symbol 'calculate' in " + libPathStr);
    }

    auto typeFn = reinterpret_cast<TypeFunc>(lib_->getSymbol("get_plugin_type"));
    if (typeFn) {
        int t = typeFn();
        if (t < 0 || t > 2) {
            throw InvalidPluginException("Plugin '" + name_ + "' returned unknown plugin type: " + std::to_string(t));
        }
        type_ = static_cast<PluginType>(t);
    } else {
        type_ = PLUGIN_TYPE_UNARY_FUNC;
    }

    auto arityFn = reinterpret_cast<ArityFunc>(lib_->getSymbol("get_plugin_arity"));
    if (arityFn) {
        arity_ = arityFn();
        if (arity_ <= 0) {
            throw InvalidPluginException("Plugin '" + name_ + "' returned invalid arity: " + std::to_string(arity_));
        }
    } else {
        arity_ = (type_ == PLUGIN_TYPE_UNARY_FUNC) ? 1 : 2;
    }

    auto precFn = reinterpret_cast<PrecedenceFunc>(lib_->getSymbol("get_plugin_precedence"));
    if (precFn) {
        precedence_ = precFn();
    } else {
        precedence_ = (type_ == PLUGIN_TYPE_BINARY_OP) ? 3 : 0;
    }

    auto assocFn = reinterpret_cast<AssociativityFunc>(lib_->getSymbol("get_plugin_associativity"));
    if (assocFn) {
        rightAssociative_ = (assocFn() != 0);
    } else {
        rightAssociative_ = (name_ == "^");
    }

    auto descFn = reinterpret_cast<DescriptionFunc>(lib_->getSymbol("get_plugin_description"));
    if (descFn) {
        const char* desc = descFn();
        if (desc) {
            description_ = desc;
        }
    }
}

double Plugin::execute(const std::vector<double>& args) const {
    if (static_cast<int>(args.size()) != arity_) {
        throw EvaluationException("Function '" + name_ + "' expected " +
                                  std::to_string(arity_) + " argument(s), got " +
                                  std::to_string(args.size()));
    }

    double result = 0.0;
    std::array<char, 512> errorBuffer{};

    try {
        int status = calculateFunc_(args.data(), static_cast<int>(args.size()),
                                    &result, errorBuffer.data(), static_cast<int>(errorBuffer.size()));
        if (status != PLUGIN_SUCCESS) {
            std::string err(errorBuffer.data());
            if (err.empty()) {
                err = "General plugin execution error";
            }
            throw PluginExecutionException(name_, err);
        }
    } catch (const PluginExecutionException&) {
        throw;
    } catch (const std::exception& e) {
        throw PluginExecutionException(name_, e.what());
    } catch (...) {
        throw PluginExecutionException(name_, "Unknown internal error in plugin");
    }

    return result;
}

} // namespace calc
