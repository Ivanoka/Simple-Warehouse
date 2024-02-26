#include "consoleView.h"

size_t ConsoleView::getLeftMargin()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // Get window width
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  SHORT consoleWidth = csbi.dwSize.X;

  if (((consoleWidth - 136) / 2) <= 0) {
    return 0;
  } else {
    return ((static_cast<size_t>(consoleWidth) - 136) / 2);
  }
}

void ConsoleView::showMenu() {
  size_t leftMargin = getLeftMargin();
  std::string separator(134, static_cast<const char>(205));
  std::string margin(leftMargin, ' ');

  // Console cleaning
  system("cls");

  // Opening line
  std::cout << margin << static_cast<const char>(201) << separator
            << static_cast<const char>(187) << std::endl;

  // Item lines
  for (const auto& line : menuItems) {
    std::cout << margin << static_cast<const char>(186);

    size_t itemsLength = 0;
    for (const auto& item : line) {
      itemsLength += item.length();

      if (item != line.back()) itemsLength += 4;
    }

    std::string leftItemsMargin((134 - itemsLength) / 2, ' ');
    std::string rightItemsMargin(134 - itemsLength - leftItemsMargin.length(),
                                 ' ');

    std::cout << leftItemsMargin;

    std::string itemSeparator(4, ' ');
    for (const auto& item : line) {
      std::cout << item;

      if (item != line.back()) std::cout << itemSeparator;
    }

    std::cout << rightItemsMargin << static_cast<const char>(186) << std::endl;

    // Separator line
    if (line != menuItems.back()) {
      std::cout << margin << static_cast<const char>(204) << separator
                << static_cast<const char>(185) << std::endl;
    }
  }

  // Closing line
  std::cout << margin << static_cast<const char>(200) << separator
            << static_cast<const char>(188) << std::endl;
}
