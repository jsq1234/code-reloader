#include "reloader.hpp"

#include <cstdio>
#include <stdexcept>

void* Load(const char* filepath) {
  void* handle = dlopen(filepath, RTLD_NOW);
  if (!handle) {
    throw std::runtime_error(dlerror());
  }
  return handle;
}

void* LoadSymbol(void* library, const char* symbol) {
  void* func = dlsym(library, symbol);
  if (!func) {
    throw std::runtime_error(dlerror());
  }
  return func;
}

void Reload(void*& library, const char* filepath) {
  dlclose(library);
  library = dlopen(filepath, RTLD_NOW);
  if (!library) {
    throw std::runtime_error(dlerror());
  }
}