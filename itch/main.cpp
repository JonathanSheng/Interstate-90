#include <array>
#include <cstdint>
#include <filesystem> // C++17
#include <fstream>
#include <iostream>
#include <vector>
#include "book/book.hpp"
#include "decode.hpp"

int main() {

  // Open itch data file
  std::string filepath = "data/01302019.NASDAQ_ITCH50";
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    return 1;
  }
  Book book;
  /*
  Binary File Spec:
  https://www.nasdaqtrader.com/content/technicalSupport/specifications/dataproducts/binaryfile.pdf

  ITCH spec:
  https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf
  */
  std::vector<std::byte> payload;
  std::vector<int> totals(9);
  while (read_record(file, payload)) {

    switch (std::to_integer<char>(payload[0])) {
    case 'S':
      SystemEvent event;
      cast_system_event(event, payload);
      totals[0]++;
      break;
    case 'R':
      StockDirectory stock_directory;
      cast_stock_directory(stock_directory, payload);
      totals[1]++;
      break;
    case 'A': {
      AddOrder order;
      cast_add_order(order, payload);
      book.add_order(order); //Add order to our book
      totals[2]++;
      break;
    }
    case 'F': {
      AddOrderMpid order;
      cast_add_order_mpid(order, payload);
      book.add_order(order);
      totals[3]++;
      break;
    }
    case 'E': {
      OrderExecuted executed;
      cast_order_executed(executed, payload);
      book.execute_order(executed);
      totals[4]++;
      break;
    }
    case 'C': {
      OrderExecutedWithPrice executed;
      cast_order_executed_with_price(executed, payload);
      book.execute_order(executed);
      totals[5]++;
      break;
    }
    case 'X': {
      OrderCancel cancel;
      cast_order_cancel(cancel, payload);
      book.cancel_order(cancel);
      totals[6]++;
      break;
    }
    case 'D': {
      OrderDelete deleted;
      cast_order_delete(deleted, payload);
      book.delete_order(deleted);
      totals[7]++;
      break;
    }
    case 'U': {
      OrderReplace replaced;
      cast_order_replace(replaced, payload);
      book.replace_order(replaced);
      totals[8]++;
      break;
    }
    }
  }
  for (auto const& t: totals){
    std::cout << t << '\n';
  }

  
  file.close();
  return 0;
}