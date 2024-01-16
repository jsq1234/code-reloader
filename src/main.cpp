#include <exception>
#include <iostream>
#include <reloader.hpp>
#include <test.hpp>

int main() {
  TestModule::LoadLibrary();
  TestModule::Foo();
  std::cout << "bar == " << TestModule::getBar() << std::endl;

  while (std::cin.get() != '\n') {
  }

  TestModule::ReloadLibrary();
  TestModule::Foo();
  std::cout << "bar == " << TestModule::getBar() << std::endl;

  return 0;
}