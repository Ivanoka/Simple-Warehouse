#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>

#include <codecvt>
#include <fstream>
#include <iosfwd>
#include <locale>
#include <memory>
#include <sstream>
#include <vector>

#include "../item.h"
#include "sqlite3.h"

class DatabaseModel {
 private:
  bool isLatinChars(const TCHAR* fileName, const size_t fileNameLength) const;
  static int callbackProductCount(void* data, int argc, char** argv,
                                  char** colNames);
  static int callbackProductInfo(void* data, int argc, char** argv,
                                 char** colNames);
  std::vector<std::string> split(const std::string&, char) const;

 public:
  bool openFile(sqlite3*&) const;
  bool createFile(sqlite3*&) const;
  std::vector<Item>& getItemList(sqlite3*&);
  bool newItem(sqlite3*&, const std::string&) const;
  bool importCsv(sqlite3*&) const;
  bool exportCsv(sqlite3*&) const;
};

#endif  // DATABASEMODEL_H