#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem> // C++17


enum class EventCode : char {
    StartOfMessages = 'O', //First message sent in trading day
    StartOfSystemHours = 'S', //Start of system hours, NASDAQ is accepting orders
    StartOfMarketHours = 'Q', //Start of Market hours
    EndOfMarketHours = 'M', //End of market hours
    EndOfSystemHours = 'E', //End of system hours, no more orders accepted
    EndOfMessages = 'C', //End of messages, last message
};

/*
If message type is 'S', then it's system event message
*/
struct SystemEvent {
    uint16_t stock_locate; //Always 0
    uint16_t tracking_number; //Nasdaq internal tracking number
    uint64_t timestamp; //nanoseconds since midnight, 6 byte field
    EventCode event_code; 
};

int main() {

    //Open itch data file
    std::string filepath = "data/01302019.NASDAQ_ITCH50";
    std::ifstream file(filepath, std::ios::binary);
    if (!file){
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
    Message Type (1) | Stock Locate (2) | Tracking Number (2) | Timestamp (6) | Event Code (1)

    */
    std::byte len_bytes[2];
    file.read(reinterpret_cast<char*>(len_bytes), 2);
    uint16_t length = (std::to_integer<uint16_t>(len_bytes[0]) << 8) | std::to_integer<uint16_t>(len_bytes[1]);
    
    std::vector<std::byte> payload(length);
    file.read(reinterpret_cast<char*>(payload.data()), length);

    char type = std::to_integer<char>(payload[0]);
    SystemEvent event;
    if (type == 'S'){
        event.stock_locate = std::to_integer<uint16_t>(payload[1]) << 8 | std::to_integer<uint16_t>(payload[2]);
        event.tracking_number = std::to_integer<uint16_t>(payload[3]) << 8 | std::to_integer<uint16_t>(payload[4]);
        event.timestamp = 0;
        for (int i = 0; i < 6; ++i){
            event.timestamp = event.timestamp << 8 | std::to_integer<uint64_t>(payload[i+5]);
        }
        event.event_code = static_cast<EventCode>(std::to_integer<char>(payload[11]));
    }
    std::cout << event.stock_locate << ' '
          << event.tracking_number << ' '
          << event.timestamp << ' '
          << static_cast<char>(event.event_code) << '\n';
    file.close();
    return 0;
}