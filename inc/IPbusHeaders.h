
#ifndef IPBUSHEADERS_H
#define IPBUSHEADERS_H

#include <cstdint>
#include <cstring>
#include <string>
#include <climits>

namespace ipbus
{

enum class IPbusMode {Master, Slave};

template <typename T>
T reverseBytes(T u)
{
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

  union {
    T u;
    unsigned char u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); k++)
    dest.u8[k] = source.u8[sizeof(T) - k - 1];

  return dest.u;
}

const uint8_t wordSize = sizeof(uint32_t); // 4 bytes


enum PacketType { Control = 0,
                  Status = 1,
                  Resend = 2 };


struct PacketHeader {
  uint32_t packetType : 4,
    byteOrder : 4,
    packetID : 16,
    rsvd : 4,
    protocolVersion : 4;

  /// @brief Creates a `PacketHeader` for a packet of type `t` and ID = `id`, with byte order `0xf` and protocol version 2
  PacketHeader(enum PacketType t = Status, uint16_t id = 0)
  {
    packetType = t;
    byteOrder = 0xf;
    packetID = id;
    rsvd = 0;
    protocolVersion = 2;
  }

  /// @brief Constructs the packet header from a raw `uint32_t` value
  PacketHeader(const uint32_t& word) { memcpy(this, &word, wordSize); }

  /// @brief Converts the header to a raw `uint32_t` value
  operator uint32_t() const { return *reinterpret_cast<const uint32_t*>(this); }
};

enum TransactionType {
  DataRead = 0,
  DataWrite = 1,
  NonIncrementingRead = 2,
  NonIncrementingWrite = 3,
  RMWbits = 4,
  RMWsum = 5,
  ConfigurationRead = 6,
  ConfigurationWrite = 7
};

struct TransactionHeader {
  uint32_t infoCode : 4,
    typeID : 4,
    words : 8,
    transactionID : 12,
    protocolVersion : 4;
  TransactionHeader(TransactionType t, uint8_t nWords, uint16_t id = 0, uint8_t code = 0xf)
  {
    infoCode = code;
    typeID = t;
    words = nWords;
    transactionID = id;
    protocolVersion = 2;
  }
  TransactionHeader(const uint32_t& word) { memcpy(this, &word, wordSize); }
  operator uint32_t() { return *reinterpret_cast<uint32_t*>(this); }
  std::string infoCodeString()
  {
    switch (infoCode) {
      case 0x0:
        return "Successful request";
      case 0x1:
        return "Bad header";
      case 0x4:
        return "IPbus read error";
      case 0x5:
        return "IPbus write error";
      case 0x6:
        return "IPbus read timeout";
      case 0x7:
        return "IPbus write timeout";
      case 0xf:
        return "Outbound request";
      default:
        return "Unknown info code";
    }
  }
};

/** @brief A struct containing full information about a single transaction
 *  @details  is represented within the IPbus packet by three components: transaction header (1 word), Address of the memory location on which the operation will be performed (1 word)
 * and a block of data (if any data is required). Data layout is speficic for each kind of transaction. Transaction stores information about request, also after
 * the response is received pointer to the response header will be stored in field responseHeader.
 */
struct Transaction {
  TransactionHeader
    /** Request transaction header describes  */
    *requestHeader,
    /** Address to the response header will be saved here */
    *responseHeader;

  uint32_t
    /** Address of the memory location on which the operation will be performed */
    *address,
    /** Address of the block of data used in the transaction*/
    *data;
};

/** @brief A struct containing definition of the packet used to check the connection
 */
struct StatusPacket {
  PacketHeader header = reverseBytes<uint32_t>(uint32_t(PacketHeader(Status))); // 0x200000F1: {0xF1, 0, 0, 0x20} -> {0x20, 0, 0, 0xF1}
  uint32_t mtu = 0,
           nResponseBuffers = 0,
           nextPacketID = 0;
  uint8_t trafficHistory[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  uint32_t controlHistory[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
};

} // namespace ipbus

#endif // IPBUSHEADERS_H
