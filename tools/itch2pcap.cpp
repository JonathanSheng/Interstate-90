#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "itch/decode.hpp"

/*
Binary File Spec:
https://www.nasdaqtrader.com/content/technicalSupport/specifications/dataproducts/binaryfile.pdf

ITCH spec:
https://nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf

MoldUDP64 spec:
https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/moldudp64.pdf

Downstream packet header:
Session             Offset 0        Length 10
Sequence Number     Offset 10       Length 8
Message Count       Offset 18       Length 2

Message Block:
Message Length      Offset Variable (first is 20 from above) Length 2
Message Data        Offset Variable (first is 22)           Length is variable

The packet built here follows build_packet in
https://docs.rs/crate/moldudp/latest/source/src/server.rs
*/

// Append an unsigned integer, most significant byte first.
void append_be(std::vector<std::byte> &out, uint64_t value, int nbytes) {
  for (int shift = (nbytes - 1) * 8; shift >= 0; shift -= 8) {
    out.push_back(static_cast<std::byte>((value >> shift) & 0xff));
  }
}

// Session, sequence, count, then each payload with its 2-byte length in front.
std::vector<std::byte> build_packet(const char session[10], uint64_t sequence,
                                    const std::vector<std::byte> &payload) {
  std::vector<std::byte> packet;

  //push the hardcoded session number
  for (int i = 0; i < 10; ++i) {
    packet.push_back(static_cast<std::byte>(session[i]));
  }

  //push the 8 byte sequence number onto packet
  append_be(packet, sequence, 8);

  //write a single message count, size 2 bytes
  append_be(packet, 1, 2);

  //write the message length, size 2 bytes
  append_be(packet, payload.size(), 2);

  // write the message data, variable length
  packet.insert(packet.end(), payload.begin(), payload.end());
  return packet;
}

int main() {
  std::ifstream file("data/01302019.NASDAQ_ITCH50", std::ios::binary);
  if (!file) {
    return 1;
  }
  std::ofstream out("/tmp/one.mold", std::ios::binary);
  if (!out) {
    return 1;
  }
  // Hardcode session number
  const char session[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '1'};
  uint64_t sequence{1};
  // read_record strips the 2-byte length. build_packet writes it back.
  std::vector<std::byte> payload;
  while (read_record(file, payload)) {

    std::vector<std::byte> packet = build_packet(session, sequence, payload);
    sequence++;

    //Write data + size
    out.write(reinterpret_cast<const char *>(packet.data()),
              static_cast<std::streamsize>(packet.size()));
  }
  return 0;
}
