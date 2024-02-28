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

