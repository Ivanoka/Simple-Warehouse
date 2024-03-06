#include "winapiView.h"

LRESULT WinAPIView::windowProcNewItem(HWND hwnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
  static HWND hEditName;
  static HWND hEditBarcode;
  static HWND hEditStock;

  auto buffer = (std::string*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      // Creating a gray brush
      HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));

      // Drawing a rectangle with a gray brush
      RECT rc = {0, 101, 341, 145};
      FillRect(hdc, &rc, hBrush);

      // Deleting a created brush
      DeleteObject(hBrush);

      EndPaint(hwnd, &ps);
      break;
    }
    case WM_CREATE: {
      // Remove the WS_MINIMIZEBOX, WS_SIZEBOX and WS_MAXIMIZEBOX styles from
      // the window style
      SetWindowLongPtr(hwnd, GWL_STYLE,
                       GetWindowLongPtr(hwnd, GWL_STYLE) &
                           ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm{};
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      // Field for entering the name
      hEditName = CreateWindowW(
          L"EDIT", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
          21, 23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditName, WM_SETFONT, (WPARAM)(hFont), TRUE);
      SendMessage(hEditName, EM_SETLIMITTEXT, 94, 0);
      Edit_SetCueBannerText(hEditName, L"Product name");

      // Field for entering a barcode
      hEditBarcode = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          55, 100, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditBarcode, WM_SETFONT, (WPARAM)(hFont), TRUE);
      SendMessage(hEditBarcode, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditBarcode, L"Barcode");

      // Field for entering the stock
      hEditStock = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 269,
          55, 52, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditStock, WM_SETFONT, (WPARAM)(hFont), TRUE);
      SendMessage(hEditStock, EM_SETLIMITTEXT, 5, 0);
      Edit_SetCueBannerText(hEditStock, L"Stock");

      // Add button
      HWND hButtonAdd = CreateWindow(
          L"BUTTON", L"Add", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167, 112,
          73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hButtonAdd, WM_SETFONT, (WPARAM)(hFont), TRUE);

      // Cancel button
      HWND hButtonCancel = CreateWindow(
          L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 112, 73, 23, hwnd,
          reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, (WPARAM)(hFont), TRUE);
      break;
    }
    case WM_COMMAND: {
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferName[256];
        GetWindowTextW(hEditName, bufferName, 256);
        wchar_t bufferBarcode[256];
        GetWindowTextW(hEditBarcode, bufferBarcode, 256);
        wchar_t bufferStock[256];
        GetWindowTextW(hEditStock, bufferStock, 256);

        // Check that the input field is not empty

        if (bufferName[0] == L'\0') {
          MessageBox(hwnd, L"The field for entering a name cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditName);
          return 0;
        }
        if (bufferBarcode[0] == L'\0') {
          MessageBox(hwnd, L"The field for entering a barcode cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }
        if (bufferStock[0] == L'\0') {
          MessageBox(hwnd, L"The field for entering a stock cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditStock);
          return 0;
        }

        // Checking for non-Latin characters.
        for (size_t i = 0; bufferName[i] != L'\0'; ++i) {
          if (bufferName[i] < L' ' || bufferName[i] > L'~') {
            MessageBox(hwnd,
                       L"The Name field cannot contain non-Latin characters!",
                       L"Error", MB_ICONERROR);
            SetFocus(hEditName);
            return 0;
          }
        }

        // Checking for correct barcode length
        if (wcslen(bufferBarcode) != 13) {
          MessageBox(hwnd, L"Enter a 13-digit barcode!", L"Error",
                     MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }

        std::wstring wstrName(bufferName);
        std::wstring wstrBarcode(bufferBarcode);
        std::wstring wstrStock(bufferStock);

        std::string strName(wstrName.begin(), wstrName.end());
        std::string strBarcode(wstrBarcode.begin(), wstrBarcode.end());
        std::string strStock(wstrStock.begin(), wstrStock.end());

        *buffer = std::format(R"(INSERT INTO items VALUES
   ('{}', '{}', '{}', '{}'))",
                              strBarcode, strName, strStock, time(nullptr));
        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        buffer->clear();
        DestroyWindow(hwnd);
      }
      break;
    }
    case WM_CLOSE: {
      buffer->clear();
      DestroyWindow(hwnd);
      break;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }
    default: {
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
  }
  return 0;
}

std::string WinAPIView::newItem() const {
  std::string buffer = "";

  // Register window class
  //HINSTANCE hInstance = GetModuleHandle(NULL);
  WNDCLASS wc = {0};
  wc.lpfnWndProc = windowProcNewItem;
  wc.hInstance = GetModuleHandle(nullptr);
  // wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY_ICON));
  wc.lpszClassName = L"Adding a new item";
  RegisterClass(&wc);

  int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  int y = GetSystemMetrics(SM_CYSCREEN) / 2 - 96;

  // Creating a window
  HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"Adding a new product",
                              WS_OVERLAPPEDWINDOW, x, y, 359, 184, nullptr,
                              nullptr, wc.hInstance, &buffer);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return buffer;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(&buffer));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return buffer;
}
