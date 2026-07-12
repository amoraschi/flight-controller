"""
Decode a framed binary telemetry log into a CSV file.

Each record is 76 bytes (packed, little-endian):

    typedef struct {
        uint16_t Sync;           // 0xCAFE
        uint32_t Tick;
        float    AccelX, AccelY, AccelZ;
        float    GyroX, GyroY, GyroZ;
        float    MagX, MagY, MagZ;
        float    PressurePa, TemperatureC;
        int32_t  Latitude, Longitude;
        float    Altitude, VelocityZ;
        uint32_t Flags;
        float    BatteryVoltage;
        uint8_t  State;
        uint8_t  SyncEnd;
    } FlightData_t;

Usage:
    python3 decode_log.py [LOG.BIN] [output.csv]
"""

import csv
import struct
import sys

# '<' little-endian, packed
# H        = uint16_t Sync (0xCAFE)
# I        = uint32_t Tick
# 9f       = AccelX/Y/Z, GyroX/Y/Z, MagX/Y/Z (all float)
# 2f       = PressurePa, TemperatureC (float)
# 2i       = Latitude, Longitude (int32_t)
# 2f       = Altitude, VelocityZ (float)
# I        = uint32_t Flags
# f        = float BatteryVoltage
# B        = uint8_t State
# B        = uint8_t SyncEnd
STRUCT_FORMAT = "<HI9f2f2i2fIfBB"

FIELDS = [
    "Sync",
    "Tick",
    "AccelX", "AccelY", "AccelZ",
    "GyroX", "GyroY", "GyroZ",
    "MagX", "MagY", "MagZ",
    "PressurePa", "TemperatureC",
    "Latitude", "Longitude",
    "Altitude", "VelocityZ",
    "Flags",
    "BatteryVoltage",
    "State",
    "SyncEnd",
]


SYNC = b"\xFE\xCA"


def decode_log(bin_path, csv_path):
    record_size = struct.calcsize(STRUCT_FORMAT)

    with open(bin_path, "rb") as f:
        data = f.read()

    pos = 0
    records = []
    skipped = 0

    while pos <= len(data) - record_size:
        if data[pos : pos + 2] != SYNC:
            pos += 1
            skipped += 1
            continue

        chunk = data[pos : pos + record_size]
        values = struct.unpack(STRUCT_FORMAT, chunk)
        records.append(values[1:])  # skip Sync
        pos += record_size

    with open(csv_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(FIELDS[1:])
        for values in records:
            writer.writerow(values)

    print(f"Decoded {len(records)} records ({record_size} bytes each) -> {csv_path}")
    if skipped:
        print(f"Skipped {skipped} bytes (resync)", file=sys.stderr)


if __name__ == "__main__":
    bin_path = "LOG_00.BIN"
    csv_path = "decoded_log0.csv"
    decode_log(bin_path, csv_path)
