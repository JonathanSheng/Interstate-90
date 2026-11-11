#pragma once

#include <concepts>
#include <cstdint>
#include <unordered_map>

#include "itch/decode.hpp"

// The two ITCH add messages. Both carry the fields add_order reads.
template <typename Added>
concept AddMessage =
    std::same_as<Added, AddOrder> || std::same_as<Added, AddOrderMpid>;

template <typename Executed>
concept ExecutedMessage = std::same_as<Executed, OrderExecuted> ||
                          std::same_as<Executed, OrderExecutedWithPrice>;

// Bare minimum of tracking an order
struct Order {
  uint16_t stock_locate; // Stock locate number
  Side side;             // Sell or Buy
  uint32_t price;        // price
  uint32_t shares;       // shares
};

// Book holds a mapping of every order_reference: order info
struct Book {

  std::unordered_map<uint64_t, Order> orders;
  std::vector<std::array<char, 8>> symbols;

public:
  template <AddMessage Added> void add_order(const Added &order);

  template <ExecutedMessage Executed>
  void execute_order(const Executed &executed);

  void cancel_order(const OrderCancel &cancel);
  void delete_order(const OrderDelete &deleted);
  void replace_order(const OrderReplace &replaced);

  void add_symbol(const StockDirectory &stock);

private:
  void subtract_shares(uint64_t order_reference, uint32_t shares);
};
