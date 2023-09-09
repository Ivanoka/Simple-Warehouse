#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>

#include <time.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../sqlite3/sqlite3.h"

#pragma comment(lib, "Shlwapi.lib")

class Database {
 public:
  Database(void);
  bool open();
  bool create();
  bool newProduct();
  bool deleteProduct();
  bool availabilityProduct();
  bool editProduct();
  bool exportCsv();
  bool importCsv();
  void printListProducts();

 private:
  bool isLatinChars(TCHAR* filePath, const DWORD& filePathLength);
  static int callbackProductCount(void* data, int argc, char** argv,
                                  char** colNames);
  static int callbackProductInfo(void* data, int argc, char** argv,
                                 char** colNames);
  std::vector<std::string> split(const std::string& s, char delimiter);
  static LRESULT CALLBACK windowProcNewProduct(HWND hwnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK windowProcDeleteProduct(HWND hwnd, UINT msg,
                                                WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK windowProcAvailabilityProductBarcode(HWND hwnd,
                                                               UINT msg,
                                                WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK windowProcAvailabilityProductEdit(HWND hwnd,
                                                               UINT msg,
                                                            WPARAM wParam,
                                                            LPARAM lParam);
  static LRESULT CALLBACK windowProcEditProductEdit(HWND hwnd, UINT msg,
                                                            WPARAM wParam,
                                                            LPARAM lParam);
};
