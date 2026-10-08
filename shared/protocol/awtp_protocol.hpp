#pragma once
#include <cstdint>
#include <string_view>
#include <array>

namespace airwave::protocol {

// 16-bit Magic: "AW"
constexpr uint16_t AWTP_MAGIC = 0x4157;
constexpr uint8_t  AWTP_VERSION = 0x01;
constexpr uint16_t DEFAULT_MEDIA_PORT = 49152;
constexpr uint16_t DEFAULT_CONTROL_PORT = 49153;
constexpr std::string_view MDNS_SERVICE_TYPE = "_airwave._tcp";

enum class MsgType : uint8_t {
    HandshakeReq      = 0x01,
    HandshakeResp     = 0x02,
    VideoPacket       = 0x10,
    AudioPacket       = 0x20,
    StatsFeedback     = 0x30,
    KeyframeRequest   = 0x31,
    Ping              = 0x40,
    Pong              = 0x41,
    Disconnect        = 0xFF
};

enum class CodecType : uint8_t {
    H264 = 0x01,
    H265 = 0x02,
    Opus = 0x10,
    AAC  = 0x11
};

enum PacketFlags : uint8_t {
    FLAG_NONE       = 0x00,
    FLAG_KEYFRAME   = 0x01,
    FLAG_EOS        = 0x02, // End of Stream
    FLAG_CONFIG_SPS = 0x04, // SPS / PPS in payload
    FLAG_FRAGMENTED = 0x08  // Multi-packet fragment
};

#pragma pack(push, 1)
struct AwtpHeader {
    uint16_t magic{AWTP_MAGIC};        // 2B
    uint8_t  version{AWTP_VERSION};    // 1B
    uint8_t  streamId{0};              // 1B (For multi-device grid, 0..7)
    uint8_t  msgType{0};               // 1B (MsgType)
    uint8_t  flags{0};                 // 1B (PacketFlags)
    uint16_t sequence{0};              // 2B (Sequence number)
    uint32_t timestampUs{0};           // 4B (Microsecond sender timestamp for RTT & Glass-to-glass)
    uint16_t payloadLength{0};         // 2B (Byte size of following payload)
    uint16_t reserved{0};              // 2B
};
static_assert(sizeof(AwtpHeader) == 16, "AwtpHeader must be exactly 16 bytes for zero-overhead parsing");

struct FeedbackStats {
    uint16_t lastReceivedSeq;
    uint32_t senderTimestampEchoUs;
    uint32_t localReceiveTimestampUs;
    uint16_t lostPacketsCount;
    uint8_t  batteryPercent;
    uint8_t  batteryTempCelsius;
};
#pragma pack(pop)

} // namespace airwave::protocol
