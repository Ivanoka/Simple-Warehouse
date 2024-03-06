#include "consoleView.h"
#include <iostream>

// Private
size_t ConsoleView::getLeftMargin() const {
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

// Public
void ConsoleView::showMenu() const {
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

void ConsoleView::showListItems(const std::vector<Item>& itemList) const {
  size_t leftMargin = getLeftMargin();
  std::string leftMarginStr(getLeftMargin(), ' ');

  std::array<std::string, 3> columnNames{
      std::string(1, static_cast<const char>(201)) +
          std::string(15, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(203)) +
          std::string(96, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(203)) +
          std::string(7, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(203)) +
          std::string(13, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(187)),

      std::string(1, static_cast<const char>(186)) + " BARCODE       " +
          std::string(1, static_cast<const char>(186)) +
          " NAME                                                               "
          "                            " +
          std::string(1, static_cast<const char>(186)) + " STOCK " +
          std::string(1, static_cast<const char>(186)) + " CHANGE TIME " +
          std::string(1, static_cast<const char>(186)),

      std::string(1, static_cast<const char>(200)) +
          std::string(15, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(202)) +
          std::string(96, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(202)) +
          std::string(7, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(202)) +
          std::string(13, static_cast<const char>(205)) +
          std::string(1, static_cast<const char>(188)),
  };

  for (const auto& str : columnNames) {
    std::cout << leftMarginStr << str << std::endl;
  }

  // Variables to display the time correctly
  time_t unixChangeTime;
  char buffer[11];
  struct tm curChangeTime;

  // Output the list of products as a loop
  for (const auto& item : itemList) {
    unixChangeTime = item.changeTime;
    localtime_s(&curChangeTime, &unixChangeTime);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &curChangeTime);

    for (size_t i = 0; i < leftMargin; i++) {
      std::cout << ' ';
    }
    std::cout << "  " << item.barcode << "   " << std::left << std::setw(94)
              << item.name << "   " << std::right << std::setw(5) << item.stock
              << "   " << std::setw(11) << buffer << std::endl
              << "  ";
    for (size_t i = 0; i < leftMargin - 1; i++) {
      std::cout << ' ';
    }
    for (size_t i = 0; i < 134; i++) {
      std::cout << (const char)(196);
    }
    std::cout << std::endl;
  }
}
