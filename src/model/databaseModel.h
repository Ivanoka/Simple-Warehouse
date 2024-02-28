#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>

#include "sqlite3.h"

class DatabaseModel {
private:

public:
  bool openFile(sqlite3*&);
  bool createFile(sqlite3*&);
};

#endif  // DATABASEMODEL_H