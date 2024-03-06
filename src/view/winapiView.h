#ifndef WINAPIVIEW_H
#define WINAPIVIEW_H


#include <Windows.h>
#include <CommCtrl.h>
#include <time.h>

#include <string>

#include "sqlite3.h"

class WinAPIView {
 private:
  static LRESULT windowProcNewItem(HWND, UINT, WPARAM, LPARAM);

 public:
  std::string newItem();
};

#endif  // WINAPIVIEW_H