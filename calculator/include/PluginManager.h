#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include "Plugin.h"

namespace calc {

class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager() = default;

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept = default;
    PluginManager& operator=(PluginManager&&) noexcept = default;

    size_t loadPluginsFromDirectory(const std::filesystem::path& directoryPath);
    bool loadPluginFile(const std::filesystem::path& filePath);

    bool hasPlugin(const std::string& name) const;
    std::shared_ptr<Plugin> getPlugin(const std::string& name) const;

    bool isBinaryOperator(const std::string& symbol) const;
    std::shared_ptr<Plugin> getBinaryOperator(const std::string& symbol) const;

    const std::map<std::string, std::shared_ptr<Plugin>>& getPlugins() const noexcept {
        return plugins_;
    }

    void clear() noexcept;

private:
    static bool isSupportedSharedLibraryExtension(const std::filesystem::path& path);

    std::map<std::string, std::shared_ptr<Plugin>> plugins_;
    std::map<std::string, std::shared_ptr<Plugin>> binaryOperators_;
};

} // namespace calc
