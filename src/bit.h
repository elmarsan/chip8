#pragma once

#define U8_CONCAT(hi, lo) ((static_cast<uint8_t>(hi) << 8) | static_cast<uint8_t>(lo))

#define READ_BIT(byte, num) (((byte) >> (num)) & 1)
