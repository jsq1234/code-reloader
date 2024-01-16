#include <dlfcn.h>

#include <cstdio>
#include <iostream>

int main() {
  const char* file_name = "./bin/Debug/libreloader.so";
  void* handle = dlopen(file_name, RTLD_NOW);
  if (!handle) {
    fprintf(stderr, "Error in linking %s : %s", file_name, dlerror());
    return 1;
  }

  void (*hello_world)(void) =
      reinterpret_cast<void (*)()>(dlsym(handle, "hello_world"));

  if (!hello_world) {
    fprintf(stderr, "Error in finding function : %s", dlerror());
    return 1;
  }

  hello_world();

  while (std::cin.get() != '\n') {
  }

  dlclose(handle);

  handle = dlopen(file_name, RTLD_NOW);
  if (!handle) {
    fprintf(stderr, "Error in linking %s : %s\n", dlerror());
    return 1;
  }

  hello_world = reinterpret_cast<void (*)()>(dlsym(handle, "hello_world"));

  if (!hello_world) {
    fprintf(stderr, "Error in loading function : %s\n", dlerror());
    return 1;
  }

  hello_world();

  dlclose(handle);

  return 0;
}
