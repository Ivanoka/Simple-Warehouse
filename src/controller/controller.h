#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Windows.h>
#include <conio.h>

#include <chrono>
#include <thread>

#include "../model/databaseModel.h"
#include "../view/consoleView.h"
#include "../view/winapiView.h"

class Controller {
private:
  DatabaseModel databaseModel;
  ConsoleView consoleView;
  WinAPIView winapiView;

  enum SHORTCUTS {
    CTRL_D = 4,
    CTRL_E = 5,
    CTRL_I = 9,
    CTRL_M = 13,
    CTRL_N = 14,
    CTRL_O = 15,
    CTRL_R = 18,
    CTRL_U = 21,
    ESC = 27,
  };

  void resizeEvent();
  void processInput();

 public:
  void init();
};

#endif  // CONTROLLER_H