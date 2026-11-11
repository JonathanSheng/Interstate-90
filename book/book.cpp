#include "book/book.hpp"

template <AddMessage Added> void Book::add_order(const Added &order) {
  Order bookOrder;
  bookOrder.stock_locate = order.stock_locate;
  bookOrder.side = order.side;
  bookOrder.price = order.price;
  bookOrder.shares = order.shares;

  orders[order.order_reference] = bookOrder;
}

template void Book::add_order<AddOrder>(const AddOrder &);

template void Book::add_order<AddOrderMpid>(const AddOrderMpid &);

template <ExecutedMessage Executed>
void Book::execute_order(const Executed &executed) {
  subtract_shares(executed.order_reference, executed.executed_shares);
}

template void Book::execute_order<OrderExecuted>(const OrderExecuted &);
template void
Book::execute_order<OrderExecutedWithPrice>(const OrderExecutedWithPrice &);

/*
Cancel order
Cancelled shares is number of shares being removed from display
*/
void Book::cancel_order(const OrderCancel &cancel) {
  subtract_shares(cancel.order_reference, cancel.cancelled_shares);
}

/*
Delete the order given order reference
*/
void Book::delete_order(const OrderDelete &deleted) {
  orders.erase(deleted.order_reference);
}

/*
Replace Order with new info
*/
void Book::replace_order(const OrderReplace &replaced) {
  auto it = orders.find(
      replaced.original_order_reference); // original order reference to replace
  if (it == orders.end()) {
    return;
  }

  Order bookOrder = it->second;
  orders.erase(it);
  bookOrder.shares = replaced.shares;
  bookOrder.price = replaced.price;
  if (bookOrder.shares == 0) {
    return;
  }
  orders[replaced.new_order_reference] = bookOrder;
}

/*
symbols[stock_locate_number] = "Stock Ticker (AAPL)"
*/
void Book::add_symbol(const StockDirectory &stock) {
  if (stock.stock_locate >= symbols.size()) {
    symbols.resize(static_cast<size_t>(stock.stock_locate) + 1);
  }
  symbols[stock.stock_locate] = stock.stock;
}

/*
Helper function to subtract shares from an order reference
if it dips to 0, we'll remove from our book
*/
void Book::subtract_shares(uint64_t order_reference, uint32_t shares) {
  auto it = orders.find(order_reference);
  if (it == orders.end()) {
    return;
  }
  if (shares >= it->second.shares) {
    orders.erase(it);
    return;
  }
  it->second.shares -= shares;
}
