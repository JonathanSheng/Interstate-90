#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

enum class EventCode : char {
  StartOfMessages = 'O',    // First message sent in trading day
  StartOfSystemHours = 'S', // Start of system hours, NASDAQ is accepting orders
  StartOfMarketHours = 'Q', // Start of Market hours
  EndOfMarketHours = 'M',   // End of market hours
  EndOfSystemHours = 'E',   // End of system hours, no more orders accepted
  EndOfMessages = 'C',      // End of messages, last message
};

/*
If message type is 'S', then it's system event message
*/
struct SystemEvent {
  uint16_t stock_locate;    // Always 0
  uint16_t tracking_number; // Nasdaq internal tracking number
  uint64_t timestamp;       // nanoseconds since midnight, 6 byte field
  EventCode event_code;     // See EventCode struct
};

// Market listing
enum class MarketCategory : char {
  // Nasdaq listed instruments
  NasdaqGlobalSelectMarket = 'Q',
  NasdaqGlobalMarket = 'G',
  NasdaqCapitalMarket = 'S',

  // Non Nasdaq listed instruments
  Nyse = 'N',
  NyseAmerican = 'A',
  NyseArca = 'P',
  Bats = 'Z',
  Iex = 'V',
  NotAvailable = ' ',
};

// For nasdaq listed issues, this field indicates when a firm is not in
// compliance with nasdaq
enum class FinancialStatus : char {
  Deficient = 'D',
  Delinquent = 'E',
  Bankrupt = 'Q',
  Suspended = 'S',
  DeficientAndBankrupt = 'G',
  DeficientAndDelinquent = 'H',
  DelinquentAndBankrupt = 'J',
  DeficientDelinquentAndBankrupt = 'K',
  EtpSuspended = 'C',
  Normal = 'N',
  NotAvailable = ' ',
};

// Indicates if NASDAQ system limits order entry
enum class RoundLotsOnly : char {
  Yes = 'Y',
  No = 'N',
};

// denotes if paricipant is in live/production or test
enum class Authenticity : char {
  Production = 'P',
  Test = 'T',
};

// indicates if security is subject to mandatory close out of short sales
enum class ShortSaleThreshold : char {
  Restricted = 'Y',
  NotRestricted = 'N',
  NotAvailable = ' ',
};

// indicates if security is set up for ipo release
enum class IpoFlag : char {
  Yes = 'Y',
  No = 'N',
  NotAvailable = ' ',
};

// indicates which limit up/ limit down price band calculation parameter should
// be used
enum class LuldTier : char {
  Tier1 = '1',
  Tier2 = '2',
  NotAvailable = ' ',
};

// indicates whether the security is an exchange traded product
enum class EtpFlag : char {
  Yes = 'Y',
  No = 'N',
  NotAvailable = ' ',
};

// indicates directional relationship between etp and underlying index
enum class InverseIndicator : char {
  Yes = 'Y',
  No = 'N',
};

// Buy/sell indicator on an add order
enum class Side : char {
  Buy = 'B',
  Sell = 'S',
};

// Whether an execution with price is eligible to print
enum class Printable : char {
  Yes = 'Y',
  No = 'N',
};

/*
Message Type 'R'
*/
struct StockDirectory {
  uint16_t stock_locate;     // Locate code assigned to this symbol for the day
  uint16_t tracking_number;  // Nasdaq internal tracking number
  uint64_t timestamp;        // Nanoseconds since midnight, 6 byte field
  std::array<char, 8> stock; // Symbol, left justified, space padded
  MarketCategory market_category;    // See MarketCategory struct
  FinancialStatus financial_status;  // See FinancialStatus struct
  uint32_t round_lot_size;           // Shares in one round lot, 4 byte field
  RoundLotsOnly round_lots_only;     // See RoundLotsOnly struct
  char issue_classification;         // Issue class, Appendix D
  std::array<char, 2> issue_subtype; // Issue sub-type, Appendix E
  Authenticity authenticity;         // See Authenticity struct
  ShortSaleThreshold short_sale_threshold; // See ShortSaleThreshold struct
  IpoFlag ipo_flag;                        // See IpoFlag struct
  LuldTier luld_tier;                      // See LuldTier struct
  EtpFlag etp_flag;                        // See EtpFlag struct
  uint32_t etp_leverage_factor; // Integer leverage versus the underlying index
  InverseIndicator inverse_indicator; // See InverseIndicator struct
};

/*
Message Type 'A'. Add order, no MPID attribution.
*/
struct AddOrder {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp; // Nanoseconds since midnight, 6 byte field
  uint64_t order_reference;
  Side side; // See Side struct
  uint32_t shares;
  std::array<char, 8> stock; // Symbol, left justified, space padded
  uint32_t price;            // Integer with 4 implied decimals
};

/*
Message Type 'F'. Add order with a 4-byte MPID attribution.
*/
struct AddOrderMpid {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t order_reference;
  Side side; // See Side struct
  uint32_t shares;
  std::array<char, 8> stock;
  uint32_t price;
  std::array<char, 4> attribution;
};

/*
Message Type 'E'. Order executed.
*/
struct OrderExecuted {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t order_reference;
  uint32_t executed_shares;
  uint64_t match_number;
};

/*
Message Type 'C'. Order executed at a given price.
*/
struct OrderExecutedWithPrice {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t order_reference;
  uint32_t executed_shares;
  uint64_t match_number;
  Printable printable; // See Printable struct
  uint32_t price;
};

/*
Message Type 'X'. Partial cancel.
*/
struct OrderCancel {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t order_reference;
  uint32_t cancelled_shares;
};

/*
Message Type 'D'. Whole order deleted.
*/
struct OrderDelete {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t order_reference;
};

/*
Message Type 'U'. Replace assigns a new order reference.
*/
struct OrderReplace {
  uint16_t stock_locate;
  uint16_t tracking_number;
  uint64_t timestamp;
  uint64_t original_order_reference;
  uint64_t new_order_reference;
  uint32_t shares;
  uint32_t price;
};

bool read_record(std::ifstream &file, std::vector<std::byte> &payload);
void cast_system_event(SystemEvent &event, std::vector<std::byte> &payload);
void cast_stock_directory(StockDirectory &stock_directory,
                          std::vector<std::byte> &payload);
void cast_add_order(AddOrder &order, std::vector<std::byte> &payload);
void cast_add_order_mpid(AddOrderMpid &order, std::vector<std::byte> &payload);
void cast_order_executed(OrderExecuted &executed,
                         std::vector<std::byte> &payload);
void cast_order_executed_with_price(OrderExecutedWithPrice &executed,
                                    std::vector<std::byte> &payload);
void cast_order_cancel(OrderCancel &cancel, std::vector<std::byte> &payload);
void cast_order_delete(OrderDelete &deleted, std::vector<std::byte> &payload);
void cast_order_replace(OrderReplace &replaced,
                        std::vector<std::byte> &payload);
