#include "PluginManager.h"
#include <iostream>
#include <algorithm>

namespace calc {

bool PluginManager::isSupportedSharedLibraryExtension(const std::filesystem::path& path) {
    const auto ext = path.extension().string();
    std::string lowerExt = ext;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return (lowerExt == ".dll" || lowerExt == ".dylib" || lowerExt == ".so");
}

size_t PluginManager::loadPluginsFromDirectory(const std::filesystem::path& directoryPath) {
    std::error_code ec;
    if (!std::filesystem::exists(directoryPath, ec) || !std::filesystem::is_directory(directoryPath, ec)) {
        std::cout << "[Plugins] Directory '" << directoryPath.string()
                  << "' does not exist. No plugins loaded." << std::endl;
        return 0;
    }

    size_t loadedCount = 0;
    size_t scannedCount = 0;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath, ec)) {
        if (ec) {
            std::cerr << "[Plugins] Warning: Error iterating directory: " << ec.message() << std::endl;
            break;
        }

        if (!entry.is_regular_file()) {
            continue;
        }

        const auto& path = entry.path();
        if (!isSupportedSharedLibraryExtension(path)) {
            continue;
        }

        ++scannedCount;
        if (loadPluginFile(path)) {
            ++loadedCount;
        }
    }

    if (scannedCount == 0) {
        std::cout << "[Plugins] No dynamic libraries (.dll/.dylib/.so) found in '"
                  << directoryPath.string() << "'." << std::endl;
    } else {
        std::cout << "[Plugins] Loaded " << loadedCount << " of "
                  << scannedCount << " plugin(s) from '" << directoryPath.string() << "'." << std::endl;
    }

    return loadedCount;
}

bool PluginManager::loadPluginFile(const std::filesystem::path& filePath) {
    try {
        auto lib = std::make_unique<DynamicLibrary>(filePath);
        auto plugin = std::make_shared<Plugin>(std::move(lib));

        const std::string& name = plugin->getName();

        if (plugins_.find(name) != plugins_.end()) {
            std::cerr << "[Plugins] Warning: Plugin with name '" << name
                      << "' is already registered. Skipping duplicate from "
                      << filePath.filename().string() << std::endl;
            return false;
        }

        if (plugin->getType() == PLUGIN_TYPE_BINARY_OP) {
            binaryOperators_[name] = plugin;
        }
        plugins_[name] = plugin;

        std::cout << "[Plugins] Successfully loaded plugin '" << name
                  << "' (type: " << (plugin->getType() == PLUGIN_TYPE_BINARY_OP ? "binary operator" : "function")
                  << ", arity: " << plugin->getArity() << ") from "
                  << filePath.filename().string() << std::endl;
        return true;
    } catch (const CalculatorException& e) {
        std::cerr << "[Plugins] Warning: Failed to load plugin from '"
                  << filePath.filename().string() << "': " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[Plugins] Warning: Unexpected standard error loading plugin from '"
                  << filePath.filename().string() << "': " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[Plugins] Warning: Unknown error loading plugin from '"
                  << filePath.filename().string() << "'" << std::endl;
        return false;
    }
}

bool PluginManager::hasPlugin(const std::string& name) const {
    return plugins_.find(name) != plugins_.end();
}

std::shared_ptr<Plugin> PluginManager::getPlugin(const std::string& name) const {
    auto it = plugins_.find(name);
    if (it != plugins_.end()) {
        return it->second;
    }
    return nullptr;
}

bool PluginManager::isBinaryOperator(const std::string& symbol) const {
    return binaryOperators_.find(symbol) != binaryOperators_.end();
}

std::shared_ptr<Plugin> PluginManager::getBinaryOperator(const std::string& symbol) const {
    auto it = binaryOperators_.find(symbol);
    if (it != binaryOperators_.end()) {
        return it->second;
    }
    return nullptr;
}

void PluginManager::clear() noexcept {
    binaryOperators_.clear();
    plugins_.clear();
}

} // namespace calc
