#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <Windows.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../item.h"
#include "sqlite3.h"

class ConsoleView {
 private:
  std::vector<std::vector<std::string>> menuItems{
      {"[CTRL + O] OPEN FILE", "[CTRL + N] NEW FILE", "[CTRL + I] IMPORT CSV",
       "[CTRL + U] EXPORT CSV"},
      {"[CTRL + M] NEW PRODUCT", "[CTRL + R] AVAILABILITY", "[CTRL + E] EDIT",
       "[CTRL + D] DELETE"}};

  size_t getLeftMargin();

 public:
  void showMenu();
  void showListItems(std::vector<Item>&);
};

#endif  // CONSOLEVIEW_H