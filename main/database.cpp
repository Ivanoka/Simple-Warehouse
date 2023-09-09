#include "database.h"

// Icon resource identifier
#define IDI_MY_ICON 101

Database::Database(void) = default;

extern int leftMarginText;
extern sqlite3* db;

// Structure for storing information about the product
struct Product {
  size_t barcode;
  std::string name;
  int stock;
  int changeTime;

  Product() {
    barcode = 0;
    stock = 0;
    changeTime = 0;
  };
};

bool Database::open() {
  // Structure for storing file selection window parameters
  OPENFILENAME ofn;
  // Buffer to store the selected file path
  TCHAR fileName[MAX_PATH] = {0};

  // Initializing the OPENFILENAME structure
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = sizeof(fileName);
  ofn.lpstrFilter = L"Database (*.db)\0*.db\0";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  bool isNonErrorDialog = false;

  while (true) {
    // Opening the file selection window
    isNonErrorDialog = GetOpenFileName(&ofn);

    if (isNonErrorDialog) {
      // Check for the correct path to the file
      if (!(isLatinChars(fileName, sizeof(fileName)))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Convert wchar array to char array
      int size = WideCharToMultiByte(CP_ACP, 0, fileName, -1, nullptr, 0,
                                     nullptr, nullptr);
      auto* charFileName = new char[size];
      WideCharToMultiByte(CP_ACP, 0, fileName, -1, charFileName, size, nullptr,
                          nullptr);

      // Open a connection to a database file
      sqlite3* testDb;
      if (sqlite3_open(charFileName, &testDb) == SQLITE_OK) {
        // If there is an open file, it must be safely closed
        if (db != nullptr) {
          sqlite3_close(db);
        }

        sqlite3_close(testDb);
        sqlite3_open(charFileName, &db);
      } else {
        // Handling database opening error
        MessageBox(nullptr,
                   L"A file opening error. The file may be corrupted or opened "
                   L"in another application!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      delete[] charFileName;
      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool Database::create() {
  // Structure for storing file selection window parameters
  OPENFILENAME ofn;
  // Buffer to store the selected file path
  TCHAR fileName[MAX_PATH] = L"New Database.db";

  // Initializing the OPENFILENAME structure
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = sizeof(fileName);
  ofn.lpstrFilter = L"All Files (*.*)\0*.*\0Database (*.db)\0*.db\0";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  bool isNonErrorDialog = false;

  while (true) {
    // Opening the file selection window
    isNonErrorDialog = GetSaveFileName(&ofn);

    if (isNonErrorDialog) {
      // Check on an existing file
      if (PathFileExists(fileName)) {
        MessageBox(nullptr,
                   L"A file with this name already exists. Change the name of "
                   L"the file to be created and try again!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the correct path to the file
      if (!(isLatinChars(fileName, sizeof(fileName)))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the .db file extension in the path
      for (int i = 3; i < MAX_PATH - 2; i++) {
        if (fileName[i] == 0 &&
            (fileName[i - 1] != 98 && fileName[i - 2] != 100 &&
             fileName[i - 3] != 46)) {
          fileName[i] = 46;
          fileName[i + 1] = 100;
          fileName[i + 2] = 98;
        }
      }

      // Convert wchar array to char array
      int size = WideCharToMultiByte(CP_ACP, 0, fileName, -1, nullptr, 0,
                                     nullptr, nullptr);
      auto* charFileName = new char[size];
      WideCharToMultiByte(CP_ACP, 0, fileName, -1, charFileName, size, nullptr,
                          nullptr);

      // Open a connection to a database file
      sqlite3* testDb;
      if (sqlite3_open(charFileName, &testDb) == SQLITE_OK) {
        // If there is an open file, it must be safely closed
        if (db != nullptr) {
          sqlite3_close(db);
        }

        sqlite3_close(testDb);
        sqlite3_open(charFileName, &db);
      } else {
        // Handling database opening error
        MessageBox(nullptr,
                   L"A file opening error. The file may be corrupted or opened "
                   L"in another application!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Create a base structure in the database
      std::string initQuery =
          "CREATE TABLE IF NOT EXISTS 'products' ('barcode'	INTEGER NOT "
          "NULL DEFAULT 0 UNIQUE, 'name' TEXT NOT NULL DEFAULT 'New "
          "product', 'stock' INTEGER NOT NULL DEFAULT 0, "
          "'change_time' INTEGER NOT NULL DEFAULT 0)";

      // Checking database structure error
      if (sqlite3_exec(db, initQuery.c_str(), nullptr, nullptr, nullptr) !=
          SQLITE_OK) {
        MessageBox(nullptr,
                   L"An unexpected error occurred while creating the file!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        std::remove(charFileName);
        continue;
      }

      delete[] charFileName;
      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool Database::newProduct() {
  // If the note has been successfully added
  bool isAdd = false;

  // Register window class
  WNDCLASS wc = {0};
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = windowProcNewProduct;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY_ICON));
  wc.lpszClassName = L"Adding a new product";
  RegisterClass(&wc);

  int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  int y = GetSystemMetrics(SM_CYSCREEN) / 2 - 96;

  // Creating a window
  HWND hwnd = CreateWindow(wc.lpszClassName, L"Adding a new product",
                           WS_OVERLAPPEDWINDOW, x, y, 359, 184, nullptr,
                           nullptr, wc.hInstance, &isAdd);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&isAdd));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (isAdd) {
    return true;
  }

  return false;
}

bool Database::deleteProduct() {
  // If the note has been successfully deleted
  bool isDelete = false;

  // Register window class
  WNDCLASS wc = {0};
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = windowProcDeleteProduct;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY_ICON));
  wc.lpszClassName = L"Deleting a product";
  RegisterClass(&wc);

  int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  int y = GetSystemMetrics(SM_CYSCREEN) / 2 - 76;

  // Creating a window
  HWND hwnd =
      CreateWindow(wc.lpszClassName, L"Deleting a product", WS_OVERLAPPEDWINDOW,
                   x, y, 359, 152, nullptr, nullptr, wc.hInstance, &isDelete);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&isDelete));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (isDelete) {
    return true;
  }

  return false;
}

bool Database::availabilityProduct() {
  std::vector<Product> productInfo;

  // Register window class
  WNDCLASS wc = {0};
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = windowProcAvailabilityProductBarcode;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY_ICON));
  wc.lpszClassName = L"Enter barcode";
  RegisterClass(&wc);

  int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  int y = GetSystemMetrics(SM_CYSCREEN) / 2 - 76;

  // Creating a window
  HWND hwnd =
      CreateWindow(wc.lpszClassName, L"Enter barcode", WS_OVERLAPPEDWINDOW, x,
                   y, 359, 152, nullptr, nullptr, wc.hInstance, &productInfo);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA,
                   reinterpret_cast<LONG_PTR>(&productInfo));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (productInfo.size() == 0) {
    return false;
  }

  // Register window class
  wc.lpfnWndProc = windowProcAvailabilityProductEdit;
  wc.lpszClassName = L"Availability";
  RegisterClass(&wc);

  x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  y = GetSystemMetrics(SM_CYSCREEN) / 2 - 96;

  // Creating a window
  hwnd =
      CreateWindow(wc.lpszClassName, L"Availability", WS_OVERLAPPEDWINDOW, x, y,
                   359, 184, nullptr, nullptr, wc.hInstance, &productInfo);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA,
                   reinterpret_cast<LONG_PTR>(&productInfo));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (productInfo.size() == 0) {
    return true;
  }

  return false;
}

bool Database::editProduct() {
  std::vector<Product> productInfo;

  // Register window class
  WNDCLASS wc = {0};
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = windowProcAvailabilityProductBarcode;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY_ICON));
  wc.lpszClassName = L"Enter barcode";
  RegisterClass(&wc);

  int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  int y = GetSystemMetrics(SM_CYSCREEN) / 2 - 76;

  // Creating a window
  HWND hwnd =
      CreateWindow(wc.lpszClassName, L"Enter barcode", WS_OVERLAPPEDWINDOW, x,
                   y, 359, 152, nullptr, nullptr, wc.hInstance, &productInfo);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA,
                   reinterpret_cast<LONG_PTR>(&productInfo));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (productInfo.size() == 0) {
    return false;
  }

  // Register window class
  wc.lpfnWndProc = windowProcEditProductEdit;
  wc.lpszClassName = L"Edit product";
  RegisterClass(&wc);

  x = GetSystemMetrics(SM_CXSCREEN) / 2 - 179;
  y = GetSystemMetrics(SM_CYSCREEN) / 2 - 96;

  // Creating a window
  hwnd =
      CreateWindow(wc.lpszClassName, L"Edit product", WS_OVERLAPPEDWINDOW, x, y,
                   359, 184, nullptr, nullptr, wc.hInstance, &productInfo);
  if (!hwnd) {
    MessageBox(nullptr, L"Window creation error!", L"Error",
               MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  // Set db pointer to custom window data
  SetWindowLongPtr(hwnd, GWLP_USERDATA,
                   reinterpret_cast<LONG_PTR>(&productInfo));

  // Display window
  ShowWindow(hwnd, SW_SHOWNORMAL);

  // Message processing cycle
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (productInfo.size() == 0) {
    return true;
  }

  return false;
}

bool Database::exportCsv() { 
// Structure for storing file selection window parameters
  OPENFILENAME ofn;
  // Buffer to store the selected file path
  TCHAR fileName[MAX_PATH] = L"Export DB.csv";

  // Initializing the OPENFILENAME structure
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = sizeof(fileName);
  ofn.lpstrFilter = L"All Files (*.*)\0*.*\0CSV Files (*.csv)\0*.csv\0";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  bool isNonErrorDialog = false;

  while (true) {
    // Opening the file selection window
    isNonErrorDialog = GetSaveFileName(&ofn);

    if (isNonErrorDialog) {
      // Check on an existing file
      if (PathFileExists(fileName)) {
        MessageBox(nullptr,
                   L"A file with this name already exists. Change the name of "
                   L"the file to be created and try again!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the correct path to the file
      if (!(isLatinChars(fileName, sizeof(fileName)))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the .csv file extension in the path
      for (int i = 4; i < MAX_PATH - 3; i++) {
        if (fileName[i] == 0 &&
            (fileName[i - 1] != 118 && fileName[i - 1] != 115 &&
             fileName[i - 2] != 99 &&
             fileName[i - 3] != 46)) {
          fileName[i] = 46;
          fileName[i + 1] = 99;
          fileName[i + 2] = 115;
          fileName[i + 3] = 118;
        }
      }

      // Convert wchar array to char array
      int size = WideCharToMultiByte(CP_ACP, 0, fileName, -1, nullptr, 0,
                                     nullptr, nullptr);
      auto* charFileName = new char[size];
      WideCharToMultiByte(CP_ACP, 0, fileName, -1, charFileName, size, nullptr,
                          nullptr);

      std::ofstream csvFile(charFileName);

      const char* sql = "SELECT * FROM products;";
      sqlite3_stmt* stmt;
      if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        // Writing column headers to a CSV file
        int columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; ++i) {
          csvFile << sqlite3_column_name(stmt, i);
          if (i < columnCount - 1) {
            csvFile << ";";
          }
        }
        csvFile << "\n";

        // Writing data to a CSV file
        while (sqlite3_step(stmt) == SQLITE_ROW) {
          for (int i = 0; i < columnCount; ++i) {
            const char* columnData =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            csvFile << columnData;
            if (i < columnCount - 1) {
              csvFile << ";";
            }
          }
          csvFile << "\n";
        }

        // Close the CSV file and free the memory after the query execution
        csvFile.close();
        sqlite3_finalize(stmt);
      } else {
        MessageBox(nullptr, L"Error writing data to CSV file!",
                   L"Error", MB_ICONERROR);
      }

      delete[] charFileName;
      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool Database::importCsv() { 
// Structure for storing file selection window parameters
  OPENFILENAME ofn;
  // Buffer to store the selected file path
  TCHAR fileName[MAX_PATH] = {0};

  // Initializing the OPENFILENAME structure
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = sizeof(fileName);
  ofn.lpstrFilter = L"All Files (*.*)\0*.*\0CSV Files (*.csv)\0*.csv\0";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  bool isNonErrorDialog = false;

  while (true) {
    // Opening the file selection window
    isNonErrorDialog = GetOpenFileName(&ofn);

    if (isNonErrorDialog) {
      // Check for the correct path to the file
      if (!(isLatinChars(fileName, sizeof(fileName)))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Convert wchar array to char array
      int size = WideCharToMultiByte(CP_ACP, 0, fileName, -1, nullptr, 0,
                                     nullptr, nullptr);
      auto* charFileName = new char[size];
      WideCharToMultiByte(CP_ACP, 0, fileName, -1, charFileName, size, nullptr,
                          nullptr);

      // Open CSV file for reading
      std::ifstream csvFile(charFileName);

      if (csvFile.is_open()) {
        std::string line;
        getline(csvFile, line);  // Skip column headers

        // Preparing SQL query for data insertion
        const char* insertSQL =
            "INSERT INTO products (barcode, name, stock, change_time) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, 0) == SQLITE_OK) {
          // Read and insert data from a CSV file
          while (getline(csvFile, line)) {
            std::vector<std::string> fields = split(line, ';');
            int paramIndex = 1;
            for (const std::string& field : fields) {
              sqlite3_bind_text(stmt, paramIndex++, field.c_str(), -1,
                                SQLITE_STATIC);
            }
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
          }

          // Free resources after data insertion
          sqlite3_finalize(stmt);
        } else {
          std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        }

        // Closing the CSV file
        csvFile.close();
      } else {
        std::cerr << "Can't open CSV file" << std::endl;
      }

      delete[] charFileName;
      return true;
    } else {
      return false;
    }
  }

  return false;
}

void Database::printListProducts() {
  // SQL query
  std::string query = "SELECT COUNT(*) FROM products";

  // Number of products in the database
  int productCount = 0;

  // Sending a request
  if (sqlite3_exec(db, query.c_str(), callbackProductCount, &productCount,
                   nullptr) != SQLITE_OK) {
    MessageBox(nullptr,
               L"Database reading error. You may be using a database that is "
               L"not compatible with this program!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    sqlite3_close(db);
    db = nullptr;
    return;
  }

  // SQL query
  query =
      "SELECT barcode, name, stock, change_time FROM products ORDER BY name";

  // Array of products information
  std::vector<Product> productList;

  // Sending a request
  if (sqlite3_exec(db, query.c_str(), callbackProductInfo, &productList,
                   nullptr) != SQLITE_OK) {
    MessageBox(nullptr,
               L"Database reading error. You may be using a database that is "
               L"not compatible with this program!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    sqlite3_close(db);
    db = nullptr;
    return;
  }

  // Output the list of products from the database
  //
  // First row of table
  std::cout << std::endl;
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(201);
  for (int i = 0; i < 15; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(203);
  for (int i = 0; i < 96; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(203);
  for (int i = 0; i < 7; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(203);
  for (int i = 0; i < 13; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(187) << std::endl;

  // Category line
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(186) << " BARCODE       " << (const char)(186)
            << " NAME                                                    "
               "                                       "
            << (const char)(186) << " STOCK " << (const char)(186)
            << " CHANGE TIME " << (const char)(186);
  std::cout << std::endl;

  // Separator line
  for (int i = 0; i < leftMarginText; i++) {
    std::cout << ' ';
  }
  std::cout << (const char)(200);
  for (int i = 0; i < 15; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(202);
  for (int i = 0; i < 96; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(202);
  for (int i = 0; i < 7; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(202);
  for (int i = 0; i < 13; i++) {
    std::cout << (const char)(205);
  }
  std::cout << (const char)(188) << std::endl;

  // Variables to display the time correctly
  time_t unixChangeTime;
  char buffer[11];
  struct tm curChangeTime;

  // Output the list of products as a loop
  for (auto product : productList) {
    unixChangeTime = product.changeTime;
    localtime_s(&curChangeTime, &unixChangeTime);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &curChangeTime);

    for (int i = 0; i < leftMarginText; i++) {
      std::cout << ' ';
    }
    std::cout << "  " << product.barcode << "   " << std::left << std::setw(94)
              << product.name << "   " << std::right << std::setw(5)
              << product.stock << "   " << std::setw(11) << buffer << std::endl
              << "  ";
    for (int i = 0; i < leftMarginText - 1; i++) {
      std::cout << ' ';
    }
    for (int i = 0; i < 134; i++) {
      std::cout << (const char)(196);
    }
    std::cout << std::endl;
  }
}

bool Database::isLatinChars(TCHAR* fileName, const DWORD& fileNameLength) {
  // Linear search checks for file extension
  for (DWORD i = 0; i < fileNameLength / 2; i++) {
    if ((fileName[i] < 32 || fileName[i] > 127) && fileName[i] != 0) {
      return false;
    }
  }

  return true;
}

int Database::callbackProductCount(void* data, int argc, char** argv,
                                   char** colNames) {
  // Write the result of the SQL query into a variable
  int* count = static_cast<int*>(data);
  *count = std::stoi(argv[0]);

  return 0;
}

int Database::callbackProductInfo(void* data, int argc, char** argv,
                                  char** colNames) {
  // Writing the result of SQL query into the variable quantity of products in
  // the database
  auto* productList = static_cast<std::vector<Product>*>(data);
  Product productInfo;
  std::stringstream sstream(argv[0]);
  sstream >> productInfo.barcode;
  productInfo.name = argv[1];
  productInfo.stock = std::stoi(argv[2]);
  productInfo.changeTime = std::stoi(argv[3]);
  productList->push_back(productInfo);

  return 0;
}

std::vector<std::string> Database::split(const std::string& s, char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream tokenStream(s);
  std::string token;
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

LRESULT Database::windowProcNewProduct(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam) {
  static HWND hEditName;
  static HWND hEditBarcode;
  static HWND hEditStock;
  // Get a pointer to the bool* object from the window's user data
  bool* pIsAdd = reinterpret_cast<bool*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

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
      SetWindowLong(hwnd, GWL_STYLE,
                    GetWindowLong(hwnd, GWL_STYLE) &
                        ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm;
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      // Field for entering the name
      hEditName = CreateWindowW(
          L"EDIT", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
          21, 23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditName, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
      SendMessage(hEditName, EM_SETLIMITTEXT, 94, 0);
      Edit_SetCueBannerText(hEditName, L"Product name");

      // Field for entering a barcode
      hEditBarcode = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          55, 100, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditBarcode, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditBarcode, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditBarcode, L"Barcode");

      // Field for entering the stock
      hEditStock = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 269,
          55, 52, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditStock, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditStock, EM_SETLIMITTEXT, 5, 0);
      Edit_SetCueBannerText(hEditStock, L"Stock");

      // Add button
      HWND hButtonAdd = CreateWindow(
          L"BUTTON", L"Add", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167, 112,
          73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hButtonAdd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);

      // Cancel button
      HWND hButtonCancel = CreateWindow(
          L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 112, 73, 23, hwnd,
          reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      break;
    }
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferName[256];
        GetWindowTextW(hEditName, bufferName, 256);
        wchar_t bufferBarcode[256];
        GetWindowTextW(hEditBarcode, bufferBarcode, 256);
        wchar_t bufferStock[256];
        GetWindowTextW(hEditStock, bufferStock, 256);

        // Check that the input field is not empty
        if (lstrlenW(bufferName) == 0) {
          MessageBox(hwnd, L"The field for entering a name cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditName);
          return 0;
        }
        if (lstrlenW(bufferBarcode) == 0) {
          MessageBox(hwnd, L"The field for entering a barcode cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }
        if (lstrlenW(bufferStock) == 0) {
          MessageBox(hwnd, L"The field for entering a stock cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditStock);
          return 0;
        }

        // Checking for non-Latin characters.
        for (wchar_t i = 0; bufferName[i] != L'\0'; i++) {
          wchar_t ch = bufferName[i];
          if (bufferName[i] < L'!' || bufferName[i] > L'~') {
            MessageBox(hwnd,
                       L"The Name field cannot contain non-Latin characters!",
                       L"Error", MB_ICONERROR);
            SetFocus(hEditName);
            return 0;
          }
        }

        // Checking for correct barcode length
        if (lstrlenW(bufferBarcode) != 13) {
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

        std::string insertQuery =
            "INSERT INTO products VALUES "
            "('" +
            strBarcode + "', '" + strName + "', '" + strStock + "', '" +
            std::to_string(time(NULL)) + "')";

        int execResult =
            sqlite3_exec(db, insertQuery.c_str(), nullptr, nullptr, nullptr);

        if (execResult == SQLITE_CONSTRAINT) {
          MessageBox(nullptr,
                     L"A product with the same barcode already exists!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        } else if (execResult != SQLITE_OK) {
          MessageBox(nullptr, L"Unknown error while working with the database!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        }

        *pIsAdd = true;

        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        DestroyWindow(hwnd);
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT Database::windowProcDeleteProduct(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam) {
  static HWND hEditBarcode;
  // Get a pointer to the bool* object from the window's user data
  bool* pIsDelete =
      reinterpret_cast<bool*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      // Creating a gray brush
      HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));

      // Drawing a rectangle with a gray brush
      RECT rc = {0, 69, 341, 113};
      FillRect(hdc, &rc, hBrush);

      // Deleting a created brush
      DeleteObject(hBrush);

      EndPaint(hwnd, &ps);
      break;
    }
    case WM_CREATE: {
      // Remove the WS_MINIMIZEBOX, WS_SIZEBOX and WS_MAXIMIZEBOX styles from
      // the window style
      SetWindowLong(hwnd, GWL_STYLE,
                    GetWindowLong(hwnd, GWL_STYLE) &
                        ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm;
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      // Field for entering a barcode
      hEditBarcode = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditBarcode, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditBarcode, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditBarcode, L"Barcode");

      // Add button
      HWND hButtonAdd = CreateWindow(
          L"BUTTON", L"Delete", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167,
          80, 73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hButtonAdd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);

      // Cancel button
      HWND hButtonCancel =
          CreateWindow(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 80, 73,
                       23, hwnd, reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      break;
    }
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferBarcode[256];
        GetWindowTextW(hEditBarcode, bufferBarcode, 256);

        // Check that the input field is not empty
        if (lstrlenW(bufferBarcode) == 0) {
          MessageBox(hwnd, L"The field for entering a barcode cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }

        // Checking for correct barcode length
        if (lstrlenW(bufferBarcode) != 13) {
          MessageBox(hwnd, L"Enter a 13-digit barcode!", L"Error",
                     MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }

        std::wstring wstrBarcode(bufferBarcode);

        std::string strBarcode(wstrBarcode.begin(), wstrBarcode.end());

        std::string insertQuerySelect =
            "SELECT barcode, name, stock, change_time FROM products WHERE "
            "barcode = '" +
            strBarcode + "'";
        std::string insertQueryDelete =
            "DELETE FROM products WHERE barcode = '" + strBarcode + "'";

        std::vector<Product> product;

        // Sending a request
        if (sqlite3_exec(db, insertQuerySelect.c_str(), callbackProductInfo,
                         &product, nullptr) != SQLITE_OK) {
          MessageBox(nullptr, L"Unknown error while working with the database!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

          return 0;
        }

        if (product.size() == 0) {
          MessageBox(nullptr,
                     L"A product with this barcode was not found. Check that "
                     L"the data entered is correct!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        }

        std::wstring wstrName(product[0].name.begin(), product[0].name.end());
        std::wstring wstrStock = std::to_wstring(product[0].stock);

        std::wstring productInfo = L"Barcode: " + wstrBarcode + L"\nName: " +
                                   wstrName + L"\nStock: " + wstrStock;

        if (MessageBox(nullptr, productInfo.c_str(), L"Delete this product?",
                       MB_TOPMOST | MB_ICONEXCLAMATION | MB_YESNO) == 6) {
          if (sqlite3_exec(db, insertQueryDelete.c_str(), nullptr, nullptr,
                           nullptr) != SQLITE_OK) {
            MessageBox(nullptr,
                       L"Unknown error while working with the database!",
                       L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return 0;
          }
        }

        *pIsDelete = true;

        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        DestroyWindow(hwnd);
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT Database::windowProcAvailabilityProductBarcode(HWND hwnd, UINT msg,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
  static HWND hEditBarcode;
  std::vector<Product>* productInfo = reinterpret_cast<std::vector<Product>*>(
      GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      // Creating a gray brush
      HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));

      // Drawing a rectangle with a gray brush
      RECT rc = {0, 69, 341, 113};
      FillRect(hdc, &rc, hBrush);

      // Deleting a created brush
      DeleteObject(hBrush);

      EndPaint(hwnd, &ps);
      break;
    }
    case WM_CREATE: {
      // Remove the WS_MINIMIZEBOX, WS_SIZEBOX and WS_MAXIMIZEBOX styles from
      // the window style
      SetWindowLong(hwnd, GWL_STYLE,
                    GetWindowLong(hwnd, GWL_STYLE) &
                        ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm;
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      // Field for entering a barcode
      hEditBarcode = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditBarcode, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditBarcode, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditBarcode, L"Barcode");

      // Add button
      HWND hFindAdd = CreateWindow(
          L"BUTTON", L"Find", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167, 80,
          73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hFindAdd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

      // Cancel button
      HWND hButtonCancel =
          CreateWindow(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 80, 73,
                       23, hwnd, reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      break;
    }
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferBarcode[256];
        GetWindowTextW(hEditBarcode, bufferBarcode, 256);

        // Check that the input field is not empty
        if (lstrlenW(bufferBarcode) == 0) {
          MessageBox(hwnd, L"The field for entering a barcode cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }

        // Checking for correct barcode length
        if (lstrlenW(bufferBarcode) != 13) {
          MessageBox(hwnd, L"Enter a 13-digit barcode!", L"Error",
                     MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }

        std::wstring wstrBarcode(bufferBarcode);

        std::string strBarcode(wstrBarcode.begin(), wstrBarcode.end());

        std::string insertQuerySelect =
            "SELECT barcode, name, stock, change_time FROM products WHERE "
            "barcode = '" +
            strBarcode + "'";

        // Sending a request
        if (sqlite3_exec(db, insertQuerySelect.c_str(), callbackProductInfo,
                         &*productInfo, nullptr) != SQLITE_OK) {
          MessageBox(nullptr, L"Unknown error while working with the database!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

          return 0;
        }

        if (productInfo->size() == 0) {
          MessageBox(nullptr,
                     L"A product with this barcode was not found. Check that "
                     L"the data entered is correct!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        }
        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        DestroyWindow(hwnd);
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT Database::windowProcAvailabilityProductEdit(HWND hwnd, UINT msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam) {
  static HWND hEditSold;
  static HWND hEditAdded;
  std::vector<Product>* productInfo = reinterpret_cast<std::vector<Product>*>(
      GetWindowLongPtr(hwnd, GWLP_USERDATA));

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
      SetWindowLong(hwnd, GWL_STYLE,
                    GetWindowLong(hwnd, GWL_STYLE) &
                        ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm;
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      hEditSold = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditSold, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
      SendMessage(hEditSold, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditSold, L"Sold");

      hEditAdded = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          55, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditAdded, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditAdded, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditAdded, L"Added");

      // Add button
      HWND hFindAdd = CreateWindow(
          L"BUTTON", L"Edit", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167,
          112, 73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hFindAdd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

      // Cancel button
      HWND hButtonCancel = CreateWindow(
          L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 112, 73, 23, hwnd,
          reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      break;
    }
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferSold[256];
        wchar_t bufferAdded[256];
        GetWindowTextW(hEditSold, bufferSold, 256);
        GetWindowTextW(hEditAdded, bufferAdded, 256);

        std::wstring wstrSold(bufferSold);
        std::wstring wstrAdded(bufferAdded);

        std::string strSold(wstrSold.begin(), wstrSold.end());
        std::string strAdded(wstrAdded.begin(), wstrAdded.end());

        if (strSold.size() == 0) {
          strSold = "0";
        }
        if (strAdded.size() == 0) {
          strAdded = "0";
        }

        std::string insertQueryUpdate =
            "UPDATE products SET stock = stock - " + strSold + " + " +
            strAdded +
            " WHERE barcode = " + std::to_string((*productInfo)[0].barcode);

        // Sending a request
        if (sqlite3_exec(db, insertQueryUpdate.c_str(), nullptr, nullptr,
                         nullptr) != SQLITE_OK) {
          MessageBox(nullptr, L"Unknown error while working with the database!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

          return 0;
        }

        productInfo->clear();

        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        productInfo->clear();
        DestroyWindow(hwnd);
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT Database::windowProcEditProductEdit(HWND hwnd, UINT msg, WPARAM wParam,
                                            LPARAM lParam) {
  static HWND hEditName;
  static HWND hEditBarcode;
  static HWND hEditStock;
  std::vector<Product>* productInfo = reinterpret_cast<std::vector<Product>*>(
      GetWindowLongPtr(hwnd, GWLP_USERDATA));

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
      SetWindowLong(hwnd, GWL_STYLE,
                    GetWindowLong(hwnd, GWL_STYLE) &
                        ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX));

      // Setting the WS_EX_TOPMOST style for the window
      SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

      // Get the current settings of the system font metrics
      NONCLIENTMETRICSW ncm;
      ncm.cbSize = sizeof(NONCLIENTMETRICSW);
      SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW),
                            &ncm, 0);

      // Getting the font height for system messages
      HDC hdc = GetDC(hwnd);
      HFONT hFont = CreateFontIndirectW(&ncm.lfMessageFont);
      HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

      // Release of resources
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);

      std::vector<Product>* pProductInfo =
          reinterpret_cast<std::vector<Product>*>(
              reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

      std::string name = (*pProductInfo)[0].name;
      int bufferSize =
          MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, NULL, 0);
      LPWSTR wideName = new WCHAR[bufferSize];
      MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideName, bufferSize);

      // Field for entering the name
      hEditName = CreateWindowW(
          L"EDIT", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
          21, 23, 300, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditName, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
      SendMessage(hEditName, EM_SETLIMITTEXT, 94, 0);
      Edit_SetCueBannerText(hEditName, L"Product name");
      SetWindowTextW(hEditName, wideName);
      delete[] wideName;

      // Field for entering a barcode
      hEditBarcode = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 21,
          55, 100, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditBarcode, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditBarcode, EM_SETLIMITTEXT, 13, 0);
      Edit_SetCueBannerText(hEditBarcode, L"Barcode");
      SetWindowTextW(
          hEditBarcode,
          const_cast<LPWSTR>(
              (std::to_wstring((*pProductInfo)[0].barcode)).c_str()));

      // Field for entering the stock
      hEditStock = CreateWindowW(
          L"EDIT", nullptr,
          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 269,
          55, 52, 23, hwnd, nullptr, nullptr, nullptr);
      SendMessage(hEditStock, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      SendMessage(hEditStock, EM_SETLIMITTEXT, 5, 0);
      Edit_SetCueBannerText(hEditStock, L"Stock");
      SetWindowTextW(hEditStock,
                     const_cast<LPWSTR>(
                         (std::to_wstring((*pProductInfo)[0].stock)).c_str()));

      // Add button
      HWND hButtonAdd = CreateWindow(
          L"BUTTON", L"Save", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 167, 112,
          73, 23, hwnd, reinterpret_cast<HMENU>(1), nullptr, nullptr);
      SendMessage(hButtonAdd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);

      // Cancel button
      HWND hButtonCancel = CreateWindow(
          L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 249, 112, 73, 23, hwnd,
          reinterpret_cast<HMENU>(2), nullptr, nullptr);
      SendMessage(hButtonCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont),
                  TRUE);
      break;
    }
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        // Getting text from the input field
        wchar_t bufferName[256];
        GetWindowTextW(hEditName, bufferName, 256);
        wchar_t bufferBarcode[256];
        GetWindowTextW(hEditBarcode, bufferBarcode, 256);
        wchar_t bufferStock[256];
        GetWindowTextW(hEditStock, bufferStock, 256);

        // Check that the input field is not empty
        if (lstrlenW(bufferName) == 0) {
          MessageBox(hwnd, L"The field for entering a name cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditName);
          return 0;
        }
        if (lstrlenW(bufferBarcode) == 0) {
          MessageBox(hwnd, L"The field for entering a barcode cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditBarcode);
          return 0;
        }
        if (lstrlenW(bufferStock) == 0) {
          MessageBox(hwnd, L"The field for entering a stock cannot be empty!",
                     L"Error", MB_ICONERROR);
          SetFocus(hEditStock);
          return 0;
        }

        // Checking for non-Latin characters.
        for (wchar_t i = 0; bufferName[i] != L'\0'; i++) {
          wchar_t ch = bufferName[i];
          if (bufferName[i] < L'!' || bufferName[i] > L'~') {
            MessageBox(hwnd,
                       L"The Name field cannot contain non-Latin characters!",
                       L"Error", MB_ICONERROR);
            SetFocus(hEditName);
            return 0;
          }
        }

        // Checking for correct barcode length
        if (lstrlenW(bufferBarcode) != 13) {
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

        std::string insertQuery =
            "UPDATE products SET barcode = '" + strBarcode + "', name = '" +
            strName + "', stock = '" + strStock + "', change_time = '" +
            std::to_string(time(NULL)) + "' WHERE barcode = " + std::to_string((*productInfo)[0].barcode);

        int execResult =
            sqlite3_exec(db, insertQuery.c_str(), nullptr, nullptr, nullptr);

        if (execResult == SQLITE_CONSTRAINT) {
          MessageBox(nullptr,
                     L"A product with the same barcode already exists!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        } else if (execResult != SQLITE_OK) {
          MessageBox(nullptr, L"Unknown error while working with the database!",
                     L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
          return 0;
        }

        productInfo->clear();
        DestroyWindow(hwnd);
      } else if (LOWORD(wParam) == 2) {
        productInfo->clear();
        DestroyWindow(hwnd);
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}
