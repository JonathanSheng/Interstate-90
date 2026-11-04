#include "decode.hpp"

/*
Reads a line from the ITCH file and returns the payload in the line
*/
bool read_record(std::ifstream &file, std::vector<std::byte> &payload) {
  std::byte len_bytes[2];

  file.read(reinterpret_cast<char *>(len_bytes), 2);
  if (!file) {
    return false;
  }
  // To convert a std::byte to a uint16_t using std::to_integer:
  uint16_t length = std::to_integer<uint16_t>(len_bytes[0]) << 8 |
                    std::to_integer<uint16_t>(len_bytes[1]);
  if (length == 0) {
    return false;
  }
  payload.clear();
  payload.resize(length);
  file.read(reinterpret_cast<char *>(payload.data()), length);
  return static_cast<bool>(file);
}

/*
Declare SystemEvent struct
*/
void cast_system_event(SystemEvent &event, std::vector<std::byte> &payload) {
  event.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                       std::to_integer<uint16_t>(payload[2]);
  event.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                          std::to_integer<uint16_t>(payload[4]);
  event.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    event.timestamp =
        event.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  event.event_code = static_cast<EventCode>(std::to_integer<char>(payload[11]));
}

/*
Declare Stock Directory struct
*/
void cast_stock_directory(StockDirectory &stock_directory,
                          std::vector<std::byte> &payload) {
  stock_directory.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                                 std::to_integer<uint16_t>(payload[2]);
  stock_directory.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                                    std::to_integer<uint16_t>(payload[4]);
  stock_directory.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    stock_directory.timestamp = stock_directory.timestamp << 8 |
                                std::to_integer<uint64_t>(payload[i + 5]);
  }
  for (int i = 0; i < 8; ++i) {
    stock_directory.stock[i] = std::to_integer<char>(payload[i + 11]);
  }
  stock_directory.market_category =
      static_cast<MarketCategory>(std::to_integer<char>(payload[19]));
  stock_directory.financial_status =
      static_cast<FinancialStatus>(std::to_integer<char>(payload[20]));
  stock_directory.round_lot_size = 0;
  for (int i = 0; i < 4; ++i) {
    stock_directory.round_lot_size = stock_directory.round_lot_size << 8 |
                                     std::to_integer<uint32_t>(payload[i + 21]);
  }
  stock_directory.round_lots_only =
      static_cast<RoundLotsOnly>(std::to_integer<char>(payload[25]));
  stock_directory.issue_classification = std::to_integer<char>(payload[26]);
  stock_directory.issue_subtype[0] = std::to_integer<char>(payload[27]);
  stock_directory.issue_subtype[1] = std::to_integer<char>(payload[28]);
  stock_directory.authenticity =
      static_cast<Authenticity>(std::to_integer<char>(payload[29]));
  stock_directory.short_sale_threshold =
      static_cast<ShortSaleThreshold>(std::to_integer<char>(payload[30]));
  stock_directory.ipo_flag =
      static_cast<IpoFlag>(std::to_integer<char>(payload[31]));
  stock_directory.luld_tier =
      static_cast<LuldTier>(std::to_integer<char>(payload[32]));
  stock_directory.etp_flag =
      static_cast<EtpFlag>(std::to_integer<char>(payload[33]));
  stock_directory.etp_leverage_factor = 0;
  for (int i = 0; i < 4; ++i) {
    stock_directory.etp_leverage_factor =
        stock_directory.etp_leverage_factor << 8 |
        std::to_integer<uint32_t>(payload[i + 34]);
  }
  stock_directory.inverse_indicator =
      static_cast<InverseIndicator>(std::to_integer<char>(payload[38]));
}

void cast_add_order(AddOrder &order, std::vector<std::byte> &payload) {
  order.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                       std::to_integer<uint16_t>(payload[2]);
  order.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                          std::to_integer<uint16_t>(payload[4]);
  order.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    order.timestamp =
        order.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  order.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    order.order_reference =
        order.order_reference << 8 | std::to_integer<uint64_t>(payload[i + 11]);
  }
  order.side = static_cast<Side>(std::to_integer<char>(payload[19]));
  order.shares = 0;
  for (int i = 0; i < 4; ++i) {
    order.shares =
        order.shares << 8 | std::to_integer<uint32_t>(payload[i + 20]);
  }
  for (int i = 0; i < 8; ++i) {
    order.stock[i] = std::to_integer<char>(payload[i + 24]);
  }
  order.price = 0;
  for (int i = 0; i < 4; ++i) {
    order.price = order.price << 8 | std::to_integer<uint32_t>(payload[i + 32]);
  }
}

void cast_add_order_mpid(AddOrderMpid &order, std::vector<std::byte> &payload) {
  order.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                       std::to_integer<uint16_t>(payload[2]);
  order.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                          std::to_integer<uint16_t>(payload[4]);
  order.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    order.timestamp =
        order.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  order.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    order.order_reference =
        order.order_reference << 8 | std::to_integer<uint64_t>(payload[i + 11]);
  }
  order.side = static_cast<Side>(std::to_integer<char>(payload[19]));
  order.shares = 0;
  for (int i = 0; i < 4; ++i) {
    order.shares =
        order.shares << 8 | std::to_integer<uint32_t>(payload[i + 20]);
  }
  for (int i = 0; i < 8; ++i) {
    order.stock[i] = std::to_integer<char>(payload[i + 24]);
  }
  order.price = 0;
  for (int i = 0; i < 4; ++i) {
    order.price = order.price << 8 | std::to_integer<uint32_t>(payload[i + 32]);
  }
  for (int i = 0; i < 4; ++i) {
    order.attribution[i] = std::to_integer<char>(payload[i + 36]);
  }
}

void cast_order_executed(OrderExecuted &executed,
                         std::vector<std::byte> &payload) {
  executed.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                          std::to_integer<uint16_t>(payload[2]);
  executed.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                             std::to_integer<uint16_t>(payload[4]);
  executed.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    executed.timestamp =
        executed.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  executed.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    executed.order_reference = executed.order_reference << 8 |
                               std::to_integer<uint64_t>(payload[i + 11]);
  }
  executed.executed_shares = 0;
  for (int i = 0; i < 4; ++i) {
    executed.executed_shares = executed.executed_shares << 8 |
                               std::to_integer<uint32_t>(payload[i + 19]);
  }
  executed.match_number = 0;
  for (int i = 0; i < 8; ++i) {
    executed.match_number =
        executed.match_number << 8 | std::to_integer<uint64_t>(payload[i + 23]);
  }
}

void cast_order_executed_with_price(OrderExecutedWithPrice &executed,
                                    std::vector<std::byte> &payload) {
  executed.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                          std::to_integer<uint16_t>(payload[2]);
  executed.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                             std::to_integer<uint16_t>(payload[4]);
  executed.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    executed.timestamp =
        executed.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  executed.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    executed.order_reference = executed.order_reference << 8 |
                               std::to_integer<uint64_t>(payload[i + 11]);
  }
  executed.executed_shares = 0;
  for (int i = 0; i < 4; ++i) {
    executed.executed_shares = executed.executed_shares << 8 |
                               std::to_integer<uint32_t>(payload[i + 19]);
  }
  executed.match_number = 0;
  for (int i = 0; i < 8; ++i) {
    executed.match_number =
        executed.match_number << 8 | std::to_integer<uint64_t>(payload[i + 23]);
  }
  executed.printable =
      static_cast<Printable>(std::to_integer<char>(payload[31]));
  executed.price = 0;
  for (int i = 0; i < 4; ++i) {
    executed.price =
        executed.price << 8 | std::to_integer<uint32_t>(payload[i + 32]);
  }
}

void cast_order_cancel(OrderCancel &cancel, std::vector<std::byte> &payload) {
  cancel.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                        std::to_integer<uint16_t>(payload[2]);
  cancel.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                           std::to_integer<uint16_t>(payload[4]);
  cancel.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    cancel.timestamp =
        cancel.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  cancel.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    cancel.order_reference = cancel.order_reference << 8 |
                             std::to_integer<uint64_t>(payload[i + 11]);
  }
  cancel.cancelled_shares = 0;
  for (int i = 0; i < 4; ++i) {
    cancel.cancelled_shares = cancel.cancelled_shares << 8 |
                              std::to_integer<uint32_t>(payload[i + 19]);
  }
}

void cast_order_delete(OrderDelete &deleted, std::vector<std::byte> &payload) {
  deleted.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                         std::to_integer<uint16_t>(payload[2]);
  deleted.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                            std::to_integer<uint16_t>(payload[4]);
  deleted.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    deleted.timestamp =
        deleted.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  deleted.order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    deleted.order_reference = deleted.order_reference << 8 |
                              std::to_integer<uint64_t>(payload[i + 11]);
  }
}

void cast_order_replace(OrderReplace &replaced,
                        std::vector<std::byte> &payload) {
  replaced.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                          std::to_integer<uint16_t>(payload[2]);
  replaced.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                             std::to_integer<uint16_t>(payload[4]);
  replaced.timestamp = 0;
  for (int i = 0; i < 6; ++i) {
    replaced.timestamp =
        replaced.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
  }
  replaced.original_order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    replaced.original_order_reference =
        replaced.original_order_reference << 8 |
        std::to_integer<uint64_t>(payload[i + 11]);
  }
  replaced.new_order_reference = 0;
  for (int i = 0; i < 8; ++i) {
    replaced.new_order_reference = replaced.new_order_reference << 8 |
                                   std::to_integer<uint64_t>(payload[i + 19]);
  }
  replaced.shares = 0;
  for (int i = 0; i < 4; ++i) {
    replaced.shares =
        replaced.shares << 8 | std::to_integer<uint32_t>(payload[i + 27]);
  }
  replaced.price = 0;
  for (int i = 0; i < 4; ++i) {
    replaced.price =
        replaced.price << 8 | std::to_integer<uint32_t>(payload[i + 31]);
  }
}