#include "controller.h"

// Constructor / Destructor

Controller::~Controller() { sqlite3_close_v2(db); }

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
        consoleView.showListItems(databaseModel.getItemList(this->db));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

bool Controller::isFileOpen() const {
  if (this->db == nullptr) {
    MessageBox(
        nullptr,
        L"The database file is not open. Open or create a file and try again.",
        L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }
  return true;
}

void Controller::processInput() {
  consoleView.showMenu();

  while (true) {
    switch (_getch()) {
      case SHORTCUTS::CTRL_O: {  // OPEN FILE
        if (databaseModel.openFile(this->db)) {
          consoleView.showMenu();
          std::vector<Item> itemList = databaseModel.getItemList(this->db);
          consoleView.showListItems(itemList);
        }
        break;
      }
      case SHORTCUTS::CTRL_N: {  // NEW FILE
        if (databaseModel.createFile(this->db)) {
          consoleView.showMenu();
          std::vector<Item> itemList = databaseModel.getItemList(this->db);
          consoleView.showListItems(itemList);
        }
        break;
      }
      case SHORTCUTS::CTRL_M: {  // NEW ITEM
        if (!isFileOpen()) break;
        while (true) {
          if (std::string insertQuery = winapiView.newItem();
              !insertQuery.empty()) {
            if (databaseModel.newItem(db, insertQuery)) {
              consoleView.showMenu();
              std::vector<Item> itemList = databaseModel.getItemList(this->db);
              consoleView.showListItems(itemList);
              break;
            }
          } else {
            break;
          }
        }
        break;
      }
      case SHORTCUTS::CTRL_I: {  // IMPORT CSV
        if (!isFileOpen()) break;
        if (databaseModel.importCsv(db)) {
          consoleView.showMenu();
          std::vector<Item> itemList = databaseModel.getItemList(this->db);
          consoleView.showListItems(itemList);
        }
        break;
      }
      case SHORTCUTS::CTRL_U: {  // EXPORT CSV
        if (!isFileOpen()) break;
        databaseModel.exportCsv(db);
        break;
      }
      case SHORTCUTS::CTRL_R: {  // AVAILABILITY
        break;
      }
      case SHORTCUTS::CTRL_E: {  // EDIT ITEM
        break;
      }
      case SHORTCUTS::CTRL_D: {  // DELETE ITEM
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
  std::jthread threadResizeEvent(&Controller::resizeEvent, this);
  threadResizeEvent.detach();

  processInput();
}