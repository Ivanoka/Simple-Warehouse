#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#pragma warning(push)
#pragma warning(disable : 4244)

#include <Windows.h>

#include <array>
#include <iomanip>
#include <iosfwd>
#include <vector>

#include "../item.h"
#include "sqlite3.h"

#pragma warning(pop)

class ConsoleView {
 private:
  std::array<std::array<std::string, 4>, 2> menuItems{
      std::array<std::string, 4>{"[CTRL + O] OPEN FILE", "[CTRL + N] NEW FILE",
                                 "[CTRL + I] IMPORT CSV",
                                 "[CTRL + U] EXPORT CSV"},
      std::array<std::string, 4>{"[CTRL + M] NEW PRODUCT",
                                 "[CTRL + R] AVAILABILITY", "[CTRL + E] EDIT",
                                 "[CTRL + D] DELETE"}};

  size_t getLeftMargin() const;

 public:
  void showMenu() const;
  void showListItems(const std::vector<Item>&) const;
};

#endif  // CONSOLEVIEW_H