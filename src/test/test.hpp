#pragma once

#include <reloader.hpp>

extern "C" {
void foo();
extern int bar;
void run();
}

std::array<const char*, 3> g_exports = {"foo", "bar", "run"};

class TestModule : public ReloaderModule<TestModule, g_exports.size()> {
   public:
    // Add more functions accordingly, also make sure to put them
    // in g_exports
    static void Foo() { getInstance().execute<void>("foo"); }
    static int GetBar() { return *getInstance().getVar<decltype(bar)>("bar"); }
    static void Run() { getInstance().execute<void>("run"); }

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