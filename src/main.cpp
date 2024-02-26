#include <Windows.h>

#include "controller/controller.h"

int main() {
  // Disabling synchronization of std::cout.
  std::ios_base::sync_with_stdio(false);

  // Changing the program title
  SetConsoleTitle(L"Simple Warehouse");

  // Maximize the window
  HWND hwnd = GetConsoleWindow();
  ShowWindow(hwnd, SW_MAXIMIZE);

  // Set background and text color
  system("color 17");

  // Hiding the cursor
  CONSOLE_CURSOR_INFO cci;
  HANDLE hStdOut;
  hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleCursorInfo(hStdOut, &cci);
  cci.bVisible = FALSE;
  SetConsoleCursorInfo(hStdOut, &cci);

  // Start program
  Controller controller;
  controller.init();

  return 0;
}