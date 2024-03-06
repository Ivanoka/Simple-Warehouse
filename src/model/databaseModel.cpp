#include "databaseModel.h"

#include <iostream>

// Private
bool DatabaseModel::isLatinChars(const TCHAR* fileName,
                                 const size_t fileNameLength) const {
  // Linear search checks for file extension
  for (size_t i = 0; i < fileNameLength / 2; i++) {
    if ((fileName[i] < 32 || fileName[i] > 127) && fileName[i] != 0) {
      return false;
    }
  }

  return true;
}

int DatabaseModel::callbackProductCount(void* data, [[maybe_unused]] int argc,
                                        char** argv,
                                        [[maybe_unused]] char** colNames) {
  // Write the result of the SQL query into a variable
  auto count = static_cast<int*>(data);
  *count = std::stoi(argv[0]);

  return 0;
}

int DatabaseModel::callbackProductInfo(void* data, [[maybe_unused]] int argc,
                                       char** argv,
                                       [[maybe_unused]] char** colNames) {
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
                                              char delimiter) const {
  std::vector<std::string> tokens;
  std::istringstream tokenStream(str);
  std::string token;
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// Public
bool DatabaseModel::openFile(sqlite3*& db) const {
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

  for (std::wstring wFileName;;) {
    // Opening the file selection window
    isNonErrorDialog = GetOpenFileName(&ofn);
    wFileName = fileName;

    if (isNonErrorDialog) {
      // Check for the correct path to the file
      if (!(isLatinChars(wFileName.c_str(), wFileName.size()))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Convert wchar array to char array
      std::string charFileName(wFileName.begin(), wFileName.end());

      // Open a connection to a sdatabase file
      if (sqlite3 * testDb;
          sqlite3_open(charFileName.c_str(), &testDb) == SQLITE_OK) {
        // If there is an open file, it must be safely closed
        sqlite3_close(db);

        sqlite3_close(testDb);
        sqlite3_open(charFileName.c_str(), &db);
      } else {
        // Handling database opening error
        MessageBox(nullptr,
                   L"A file opening error. The file may be corrupted or opened "
                   L"in another application!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }
      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool DatabaseModel::createFile(sqlite3*& db) const {
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

  for (std::wstring wFileName;;) {
    // Opening the file selection window
    isNonErrorDialog = GetSaveFileName(&ofn);

    if (isNonErrorDialog) {
      wFileName = fileName;
      // Check on an existing file
      if (PathFileExists(wFileName.c_str())) {
        MessageBox(nullptr,
                   L"A file with this name already exists. Change the name of "
                   L"the file to be created and try again!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the correct path to the file
      if (!(isLatinChars(wFileName.c_str(), wFileName.size()))) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }
      // Check for the .db file extension in the path
      if (wcscmp(PathFindExtension(wFileName.c_str()), L".db") != 0) {
        wFileName.append(L".db");
      }

      // Convert wchar string to char string
      std::string charFileName(wFileName.begin(), wFileName.end());

      // Open a connection to a database file
      if (sqlite3 * testDb;
          sqlite3_open(charFileName.c_str(), &testDb) == SQLITE_OK) {
        // If there is an open file, it must be safely closed
        sqlite3_close(db);

        sqlite3_close(testDb);
        sqlite3_open(charFileName.c_str(), &db);
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

        charFileName.clear();
        continue;
      }
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
  if (int itemCount = 0;
      sqlite3_exec(db, query.c_str(), callbackProductCount, &itemCount,
                   nullptr) != SQLITE_OK) {  // Sending a request
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

bool DatabaseModel::newItem(sqlite3*& db,
                            const std::string& insertQuery) const {
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

bool DatabaseModel::importCsv(sqlite3*& db) const {
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

  for (std::wstring wFileName;;) {
    // Opening the file selection window
    isNonErrorDialog = GetOpenFileName(&ofn);
    wFileName = fileName;

    if (isNonErrorDialog) {
      // Check for the correct path to the file
      if (!isLatinChars(wFileName.c_str(), wFileName.size())) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Convert wchar string to char string
      std::string charFileName(wFileName.begin(), wFileName.end());

      if (std::ifstream csvFile(charFileName);
          csvFile.is_open()) {  // Open CSV file for reading
        std::string line;
        getline(csvFile, line);  // Skip column headers

        // Preparing SQL query for data insertion
        const char* insertSQL =
            "INSERT INTO items (barcode, name, stock, change_time) VALUES "
            "(?, ?, ?, ?);";

        if (sqlite3_stmt * stmt; sqlite3_prepare_v2(db, insertSQL, -1, &stmt,
                                                    nullptr) == SQLITE_OK) {
          // Read and insert data from a CSV file
          while (getline(csvFile, line)) {
            std::vector<std::string> fields = split(line, ';');
            int paramIndex = 0;
            for (const std::string& field : fields) {
              ++paramIndex;
              sqlite3_bind_text(stmt, paramIndex, field.c_str(), -1,
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

      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool DatabaseModel::exportCsv(sqlite3*& db) const {
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

  for (std::wstring wFileName;;) {
    // Opening the file selection window
    isNonErrorDialog = GetSaveFileName(&ofn);
    wFileName = fileName;

    if (isNonErrorDialog) {
      // Check on an existing file
      if (PathFileExists(wFileName.c_str())) {
        MessageBox(nullptr,
                   L"A file with this name already exists. Change the name of "
                   L"the file to be created and try again!",
                   L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the correct path to the file
      if (!isLatinChars(wFileName.c_str(), wFileName.size())) {
        MessageBox(
            nullptr,
            L"Incorrect file path. You may be using non-Latin characters!",
            L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);

        continue;
      }

      // Check for the .csv file extension in the path
      if (wcscmp(PathFindExtension(wFileName.c_str()), L".csv") != 0) {
        wFileName.append(L".csv");
      }

      // Convert wchar string to char string
      std::string charFileName(wFileName.begin(), wFileName.end());

      const char* sql = "SELECT * FROM items;";

      if (sqlite3_stmt * stmt;
          sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::ofstream csvFile(charFileName);

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
            auto columnData = (const char*)sqlite3_column_text(stmt, i);
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

      return true;
    } else {
      return false;
    }
  }

  return false;
}
