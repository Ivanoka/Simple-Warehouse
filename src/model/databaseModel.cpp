#include "databaseModel.h"


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

