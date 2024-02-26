#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <iostream>
#include <Windows.h>
#include <vector>

class ConsoleView {
 private:
  size_t getLeftMargin();

  std::vector<std::vector<std::string>> menuItems{
      {"[CTRL + O] OPEN FILE", "[CTRL + N] NEW FILE", "[CTRL + I] IMPORT CSV",
       "[CTRL + U] EXPORT CSV"},
      {"[CTRL + M] NEW PRODUCT", "[CTRL + R] AVAILABILITY", "[CTRL + E] EDIT",
       "[CTRL + D] DELETE"}};

 public:
  void showMenu();
};

#endif  // CONSOLEVIEW_H