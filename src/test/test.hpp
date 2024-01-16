#pragma once

#include <reloader.hpp>

extern "C" {
void foo();
extern int bar;
}

std::array<const char*, 2> g_exports = {"foo", "bar"};

class TestModule : public ReloaderModule<TestModule, g_exports.size()> {
 public:
  static void Foo() { getInstance().execute<void>("foo"); }
  static int getBar() { return *getInstance().getVar<decltype(bar)>("bar"); }

 protected:
  virtual const char* getPath() const override {
#ifdef DEBUG
    return "bin/Debug/libtest.so";
#else
    return "bin/Release/libtest.so";
#endif
  }

  virtual std::array<const char*, g_exports.size()>& getSymbolNames()
      const override {
    return g_exports;
  }
};