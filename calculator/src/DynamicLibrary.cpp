#include "DynamicLibrary.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace calc {

namespace {

std::string getLastErrorMessage() {
#if defined(_WIN32) || defined(__CYGWIN__)
  DWORD errorCode = GetLastError();
  if (errorCode == 0) {
    return "Unknown error";
  }
  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer, 0, nullptr);
  std::string msg(messageBuffer, size);
  LocalFree(messageBuffer);
  return msg;
#else
  const char *err = dlerror();
  return err ? std::string(err) : "Unknown error";
#endif
}

} // namespace

DynamicLibrary::DynamicLibrary(const std::filesystem::path &path)
    : path_(path) {
#if defined(_WIN32) || defined(__CYGWIN__)
  handle_ = LoadLibraryA(path.string().c_str());
  if (!handle_) {
    throw DynamicLibraryException("Failed to load library '" + path.string() +
                                  "': " + getLastErrorMessage());
  }
#else
  dlerror();
  handle_ = dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!handle_) {
    throw DynamicLibraryException("Failed to load library '" + path.string() +
                                  "': " + getLastErrorMessage());
  }
#endif
}

DynamicLibrary::~DynamicLibrary() { unload(); }

DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
    : path_(std::move(other.path_)), handle_(other.handle_) {
  other.handle_ = nullptr;
}

DynamicLibrary &DynamicLibrary::operator=(DynamicLibrary &&other) noexcept {
  if (this != &other) {
    unload();
    path_ = std::move(other.path_);
    handle_ = other.handle_;
    other.handle_ = nullptr;
  }
  return *this;
}

void DynamicLibrary::unload() noexcept {
  if (handle_) {
#if defined(_WIN32) || defined(__CYGWIN__)
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
    handle_ = nullptr;
  }
}

void *DynamicLibrary::getSymbol(const std::string &symbolName) const {
  if (!handle_) {
    return nullptr;
  }
#if defined(_WIN32) || defined(__CYGWIN__)
  return reinterpret_cast<void *>(
      GetProcAddress(static_cast<HMODULE>(handle_), symbolName.c_str()));
#else
  return dlsym(handle_, symbolName.c_str());
#endif
}

} // namespace calc
