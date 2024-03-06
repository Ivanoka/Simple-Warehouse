#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
 public:
  size_t barcode = 0;
  std::string name;
  size_t stock = 0;
  size_t changeTime = 0;
};

#endif  // ITEM_H