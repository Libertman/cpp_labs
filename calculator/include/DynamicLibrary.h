#pragma once

#include <string>
#include <filesystem>
#include "Exceptions.h"

namespace calc {

class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::filesystem::path& path);
    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;
    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    void* getSymbol(const std::string& symbolName) const;
    bool isLoaded() const noexcept { return handle_ != nullptr; }
    const std::filesystem::path& getPath() const noexcept { return path_; }

private:
    void unload() noexcept;

    std::filesystem::path path_;
    void* handle_{nullptr};
};

} // namespace calc
