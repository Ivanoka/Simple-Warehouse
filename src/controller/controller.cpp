#include "controller.h"
void Controller::processInput() {
  consoleView.showMenu();

  while (true) {
    switch (_getch()) {
      case SHORTCUTS::CTRL_O: {  // OPEN FILE
        consoleView.showMenu();
        break;
      }
      case SHORTCUTS::CTRL_N: {  // NEW FILE
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

void Controller::init() {
  processInput();
}