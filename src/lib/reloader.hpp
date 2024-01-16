#pragma once
#include <dlfcn.h>

#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>

// N denotes the number of exported symbols
template <typename E, size_t N>
class ReloaderModule {
 public:
  static void LoadLibrary() { getInstance().load(); }
  static void ReloadLibrary() { getInstance().reload(); }

 protected:
  static E& getInstance() {
    static E instance;
    return instance;
  }

  virtual const char* getPath() const = 0;
  virtual std::array<const char*, N>& getSymbolNames() const = 0;

  template <typename Rtn, typename... Args>
  Rtn execute(const char* name, Args... args) {
    // Look the function address
    auto symbol = _symbols.find(name);
    if (symbol != _symbols.end()) {
      // Cast the address to the appropriate function type and call it,
      // forwarding all arguments
      return reinterpret_cast<Rtn (*)(Args...)>(symbol->second)(args...);
    } else {
      throw std::runtime_error(std::string("Function not found: ") + name);
    }
  }

  template <typename T>
  T* getVar(const char* name) {
    auto symbol = _symbols.find(name);
    if (symbol != _symbols.end()) {
      return static_cast<T*>(symbol->second);
    } else {
      // the variable wasn't found, return null. (Could throw error too)
      return nullptr;
    }
  }

 private:
  void load() {
    _libHandle = dlopen(getPath(), RTLD_NOW);
    loadSymbols();
  }
  void reload() {
    dlclose(_libHandle);
    _symbols.clear();
    load();
  }

  void loadSymbols() {
    for (const char* symbol : getSymbolNames()) {
      void* temp = dlsym(_libHandle, symbol);
      if (!temp) {
        throw std::runtime_error(std::string("Error in loading symbols : ") +
                                 dlerror());
      }
      _symbols[symbol] = dlsym(_libHandle, symbol);
    }
  }

  void* _libHandle;
  std::unordered_map<std::string, void*> _symbols;
};