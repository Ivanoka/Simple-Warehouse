#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
 public:
  size_t barcode;
  std::string name;
  size_t stock;
  size_t changeTime;

  Item();
};

#endif  // ITEM_H