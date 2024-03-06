#include "databaseModel.h"

// Private
bool DatabaseModel::isLatinChars(TCHAR* fileName, const DWORD& fileNameLength) {
  // Linear search checks for file extension
  for (DWORD i = 0; i < fileNameLength / 2; i++) {
    if ((fileName[i] < 32 || fileName[i] > 127) && fileName[i] != 0) {
      return false;
    }
  }

  return true;
}

int DatabaseModel::callbackProductCount(void* data, int argc, char** argv,
                                        char** colNames) {
  // Write the result of the SQL query into a variable
  int* count = static_cast<int*>(data);
  *count = std::stoi(argv[0]);

  return 0;
}

int DatabaseModel::callbackProductInfo(void* data, int argc, char** argv,
                                       char** colNames) {
  // Writing the result of SQL query into the variable quantity of products in
  // the database
  auto* productList = static_cast<std::vector<Item>*>(data);
  Item itemInfo;

  std::stringstream sstream(argv[0]);
  sstream >> itemInfo.barcode;
  itemInfo.name = argv[1];
  itemInfo.stock = std::stoi(argv[2]);
  itemInfo.changeTime = std::stoi(argv[3]);
  productList->push_back(itemInfo);

  return 0;
}

std::vector<std::string> DatabaseModel::split(const std::string& str,
                                              char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream tokenStream(str);
  std::string token;
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// Public
bool DatabaseModel::openFile(sqlite3*& db) {
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
      char* charFileName = new char[size];
      WideCharToMultiByte(CP_ACP, 0, fileName, -1, charFileName, size, nullptr,
                          nullptr);

      // Open a connection to a sdatabase file
      sqlite3* testDb;
      if (sqlite3_open(charFileName, &testDb) == SQLITE_OK) {
        // If there is an open file, it must be safely closed
        if (db != nullptr) {
          sqlite3_close(db);
        }

        sqlite3_close(testDb);
        sqlite3_open(charFileName, &(db));
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

bool DatabaseModel::createFile(sqlite3*& db) {
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
          "CREATE TABLE IF NOT EXISTS 'items' ('barcode' INTEGER NOT "
          "NULL DEFAULT 0 UNIQUE, 'name' TEXT NOT NULL DEFAULT 'New "
          "item', 'stock' INTEGER NOT NULL DEFAULT 0, "
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

std::vector<Item>& DatabaseModel::getItemList(sqlite3*& db) {
  // Array of items information
  static std::vector<Item> itemList;
  itemList.clear();

  // SQL query
  std::string query = "SELECT COUNT(*) FROM items";

  // Number of products in the database
  int itemCount = 0;

  // Sending a request
  if (sqlite3_exec(db, query.c_str(), callbackProductCount, &itemCount,
                   nullptr) != SQLITE_OK) {
    MessageBox(nullptr,
               L"Database reading error. You may be using a database that is "
               L"not compatible with this program!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    sqlite3_close(db);
    db = nullptr;
    return itemList;
  }

  // SQL query
  query = "SELECT barcode, name, stock, change_time FROM items ORDER BY name";

  // Sending a request
  if (sqlite3_exec(db, query.c_str(), callbackProductInfo, &itemList,
                   nullptr) != SQLITE_OK) {
    MessageBox(nullptr,
               L"Database reading error. You may be using a database that is "
               L"not compatible with this program!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    sqlite3_close(db);
    db = nullptr;
    return itemList;
  }

  return itemList;
}

bool DatabaseModel::newItem(sqlite3*& db, std::string insertQuery) {
  int execResult =
      sqlite3_exec(db, insertQuery.c_str(), nullptr, nullptr, nullptr);

  if (execResult == SQLITE_CONSTRAINT) {
    MessageBox(nullptr, L"A item with the same barcode already exists!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  } else if (execResult != SQLITE_OK) {
    MessageBox(nullptr, L"Unknown error while working with the database!",
               L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
    return false;
  }

  return true;
}

bool DatabaseModel::importCsv(sqlite3*& db) {
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
            "INSERT INTO items (barcode, name, stock, change_time) VALUES "
            "(?, ?, ?, ?);";
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
          std::string sqlErrorMsg(sqlite3_errmsg(db));
          std::wstring wSqlErrorMsg(sqlErrorMsg.begin(), sqlErrorMsg.end());
          std::wstring errorMsg = L"SQL error \"" + wSqlErrorMsg + L"\"";

          MessageBox(nullptr, errorMsg.c_str(), L"Error",
                     MB_TOPMOST | MB_ICONERROR | MB_OK);
          // std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
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

bool DatabaseModel::exportCsv(sqlite3*& db) {
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
             fileName[i - 2] != 99 && fileName[i - 3] != 46)) {
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

      const char* sql = "SELECT * FROM items;";
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
        MessageBox(nullptr, L"Error writing data to CSV file!", L"Error",
                   MB_ICONERROR);
      }

      delete[] charFileName;
      return true;
    } else {
      return false;
    }
  }

  return false;
}
