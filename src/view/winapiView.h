#ifndef WINAPIVIEW_H
#define WINAPIVIEW_H

#pragma warning(push)
#pragma warning(disable: 4537)
#include <Windows.h>
#include <CommCtrl.h>
#include <time.h>

#include <string>
#include <format>

#include "sqlite3.h"
#pragma warning(pop)

class WinAPIView {
 private:
  static LRESULT windowProcNewItem(HWND, UINT, WPARAM, LPARAM);

 public:
  std::string newItem() const;
};

#endif  // WINAPIVIEW_H