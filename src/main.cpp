#include <Windows.h>
#include <conio.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "model/databaseModel.h"
#include "view/consoleView.h"
#include "view/winapiView.h"
#include "controller/controller.h"


#include "sqlite3.h"
#include "database.h"

// Prototype functions
void setConsoleSettings(void);
void shortcutAltEnter(void);
void printMenuHeader(void);
void unopenFileError(void);
BOOL CtrlHandler(DWORD dwCtrlType);

#pragma comment( \
    linker,      \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Global variables for the database
sqlite3* db = nullptr;

// Left margin length for centered text output
int leftMarginText;

int main() {

  // Initial configuration of the console window
  setConsoleSettings();

  // DataBase class object
  Database database;

  // Initializing variables to start new processes
  STARTUPINFOW si = {sizeof(STARTUPINFOW)};

  // .exe names of new processes
  std::wstring bufferString = L"newProduct.exe";
  LPWSTR processNewProduct = const_cast<LPWSTR>(bufferString.c_str());

  // Display menu header in the console
  printMenuHeader();

  // Menu hotkey processing cycle
  while (true) {
    switch (_getch()) {
      case 15: {  // [CTRL + O] OPEN FILE
        if (database.open()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 14: {  // [CTRL + N] NEW FILE
        if (database.create()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 13: {  // [CTRL + M] NEW PRODUCT
        if (db == nullptr) {
          unopenFileError();
        } else if (database.newProduct()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 9: {  // [CTRL + I] IMPORT CSV
        if (db == nullptr) {
          unopenFileError();
        } else if (database.importCsv()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 21: {  // [CTRL + U] EXPORT CSV
        if (db == nullptr) {
          unopenFileError();
        } else {
          database.exportCsv();
        }
        break;
      }
      case 18: {  // [CTRL + R] AVAILABILITY
        if (db == nullptr) {
          unopenFileError();
        } else if (database.availabilityProduct()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 5: {  // [CTRL + E] EDIT PRODUCT
        if (db == nullptr) {
          unopenFileError();
        } else if (database.editProduct()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 4: {  // [CTRL + D] DELETE PRODUCT
        if (db == nullptr) {
          unopenFileError();
        } else if (database.deleteProduct()) {
          system("cls");
          printMenuHeader();
          database.printListProducts();
        }
        break;
      }
      case 27: {  // [ESC] EXIT
        if (MessageBox(nullptr, L"Do you want to exit the program?", L"Exit",
                       MB_TOPMOST | MB_ICONEXCLAMATION | MB_DEFBUTTON2 |
                           MB_YESNO) == 6) {
          return 0;
        }
        break;
      }
      default:
        break;
    }
  }

  return 0;
}

void setConsoleSettings(void) {
  // Fullscreen mod
  ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

  // Alt + Enter key override
  std::thread threadAltEnter(shortcutAltEnter);
  threadAltEnter.detach();

  // Set background and text color
  system("color 17");

  // Changing the program title
  SetConsoleTitle(L"Warehouse accounting");

  // Console close event handling to securely close the connection to the
  // database
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

  // Hiding the cursor
  CONSOLE_CURSOR_INFO cci;
  HANDLE hStdOut;
  hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleCursorInfo(hStdOut, &cci);
  cci.bVisible = FALSE;
  SetConsoleCursorInfo(hStdOut, &cci);

  // Get the descriptor of the standard console output
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_FONT_INFOEX fontInfo;
  fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
  GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);

  // Getting screen resolution
  int screenW = GetSystemMetrics(SM_CXSCREEN);
  int screenH = GetSystemMetrics(SM_CYSCREEN);

  if (screenH >= 1050 || screenW >= 1680) {
    fontInfo.dwFontSize.Y = 24;
    fontInfo.dwFontSize.X = 11;
  }

  // Applying font resizing
  SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);

  int fontWidth = fontInfo.dwFontSize.X;

  // Set the left margin value
  leftMarginText = (screenW / fontWidth - 136) / 2 - 1;
}

void shortcutAltEnter(void) {
  if (RegisterHotKey(nullptr, 1, MOD_ALT, VK_RETURN)) {
    // Waiting for pressing the Alt+Enter key combination
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
    }
  } else {
    MessageBox(nullptr, L"Failed to register Alt+Enter hotkey.", L"Error",
               MB_ICONERROR | MB_OK);
  }
}

void printMenuHeader(void) {
  // First row of table
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(201);
  for (int i = 0; i < 134; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(187) << std::endl;

  // File management line
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(186);
  std::cout << "                     [CTRL + O] OPEN FILE    [CTRL "
               "+ N] NEW FILE    [CTRL + I] IMPORT CSV    [CTRL + U] EXPORT CSV                    ";
  std::cout << (const char)(186) << std::endl;

  // Separator line
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(204);
  for (int i = 0; i < 134; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(185) << std::endl;

  // Product management line
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(186);
  std::cout << "                       [CTRL + M] NEW PRODUCT    [CTRL + R] AVAILABILITY    [CTRL + "
               "E] EDIT    [CTRL + D] DELETE                      ";
  std::cout << (const char)(186) << std::endl;

  // Last row of table
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(200);
  for (int i = 0; i < 134; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(188) << std::endl;
}

void unopenFileError(void) {
  MessageBox(nullptr,
             L"Please open an existing file or create a new one before "
             L"delete a product.",
             L"Error in adding a new product",
             MB_TOPMOST | MB_ICONEXCLAMATION | MB_OK);
}

BOOL CtrlHandler(DWORD dwCtrlType) {
  if (dwCtrlType == CTRL_CLOSE_EVENT && db == nullptr) {
    sqlite3_close(db);
  }
  return FALSE;
}
