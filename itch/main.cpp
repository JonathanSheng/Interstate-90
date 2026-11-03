#include <array>
#include <cstdint>
#include <filesystem> // C++17
#include <fstream>
#include <iostream>
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

int main() {

  // Open itch data file
  std::string filepath = "data/01302019.NASDAQ_ITCH50";
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    return 1;
  }

  /*
  Binary File Spec:
  https://www.nasdaqtrader.com/content/technicalSupport/specifications/dataproducts/binaryfile.pdf
  2 bytes of payload size (big endian) | Payload
  len_bytes holds 00, 0c for example
  we want
  00 * 256 + 0c * 1
  */
  /*
  https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf
  ITCH spec
  1.1 System Event Message

  Offset 0 Length 1, Message Type Value S
  Offset 1, Length 2
  Data Type: unsigned
  Message Type (1) | Stock Locate (2) | Tracking Number (2) | Timestamp (6) |
  Event Code (1)

  */
  std::byte len_bytes[2];
  file.read(reinterpret_cast<char *>(len_bytes), 2);
  uint16_t length = (std::to_integer<uint16_t>(len_bytes[0]) << 8) |
                    std::to_integer<uint16_t>(len_bytes[1]);

  std::vector<std::byte> payload(length);
  file.read(reinterpret_cast<char *>(payload.data()), length);

  char type = std::to_integer<char>(payload[0]);
  SystemEvent event;
  if (type == 'S') {
    event.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 |
                         std::to_integer<uint16_t>(payload[2]);
    event.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 |
                            std::to_integer<uint16_t>(payload[4]);
    event.timestamp = 0;
    for (int i = 0; i < 6; ++i) {
      event.timestamp =
          event.timestamp << 8 | std::to_integer<uint64_t>(payload[i + 5]);
    }
    event.event_code =
        static_cast<EventCode>(std::to_integer<char>(payload[11]));
  }
  std::cout << event.stock_locate << ' ' << event.tracking_number << ' '
            << event.timestamp << ' ' << static_cast<char>(event.event_code)
            << '\n';

  /*
  Stock Related Messages

  Message Type (1) R | Stock Locate (2) | Tracking Number (2) | Timestamp (6) |
  Stock (8) | Market Category (1)
  */
  
  file.close();
  return 0;
}