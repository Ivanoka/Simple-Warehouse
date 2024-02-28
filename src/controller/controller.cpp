#include "controller.h"

// Constructor / Destructor
Controller::Controller() : db(nullptr) {}

Controller::~Controller() { db = nullptr; }

// Private
void Controller::resizeEvent() {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // Get window width
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  int consoleWidth = csbi.dwSize.X;

  while (true) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    if (csbi.dwSize.X != consoleWidth) {
      consoleWidth = csbi.dwSize.X;

      consoleView.showMenu();
      if (this->db != nullptr) {
        std::vector<Item>& itemList = databaseModel.getItemList(this->db);
        consoleView.showListItems(itemList);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Controller::processInput() {
  consoleView.showMenu();

  while (true) {
    switch (_getch()) {
      case SHORTCUTS::CTRL_O: {  // OPEN FILE
        if (databaseModel.openFile(this->db)) {
          consoleView.showMenu();
          std::vector<Item>& itemList = databaseModel.getItemList(this->db);
          consoleView.showListItems(itemList);
        }
        break;
      }
      case SHORTCUTS::CTRL_N: {  // NEW FILE
        if (databaseModel.createFile(this->db)) {
          consoleView.showMenu();
          std::vector<Item>& itemList = databaseModel.getItemList(this->db);
          consoleView.showListItems(itemList);
        }
        break;
      }
      case SHORTCUTS::CTRL_M: {  // NEW PRODUCT
        break;
      }
      case SHORTCUTS::CTRL_I: {  // IMPORT CSV
        break;
      }
      case SHORTCUTS::CTRL_U: {  // EXPORT CSV
        break;
      }
      case SHORTCUTS::CTRL_R: {  // AVAILABILITY
        break;
      }
      case SHORTCUTS::CTRL_E: {  // EDIT PRODUCT
        break;
      }
      case SHORTCUTS::CTRL_D: {  // DELETE PRODUCT
        break;
      }
      case SHORTCUTS::ESC: {  // EXIT
        if (MessageBox(nullptr, L"Do you want to exit the program?", L"Exit",
                       MB_TOPMOST | MB_ICONEXCLAMATION | MB_DEFBUTTON2 |
                           MB_YESNO) == 6) {
          return;
        }
        break;
      }
      default:
        break;
    }
  }
}

// Public
void Controller::init() {
  std::thread threadResizeEvent(&Controller::resizeEvent, this);
  threadResizeEvent.detach();

  processInput();
}