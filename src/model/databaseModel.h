#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../item.h"
#include "sqlite3.h"

class DatabaseModel {
 private:
  bool isLatinChars(TCHAR* fileName, const DWORD& fileNameLength);
  static int callbackProductCount(void* data, int argc, char** argv,
                                  char** colNames);
  static int callbackProductInfo(void* data, int argc, char** argv,
                                 char** colNames);

 public:
  bool openFile(sqlite3*&);
  bool createFile(sqlite3*&);
  std::vector<Item>& getItemList(sqlite3*&);
};

#endif  // DATABASEMODEL_H