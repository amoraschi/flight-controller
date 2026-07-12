import struct
import math

STRUCT_FORMAT = "<HI9f2f2i2fIfBB"
SYNC = 0xCAFE
SYNC_END = 0xBE

TICK_INTERVAL_MS = 10
NUM_RECORDS = 500
DROP_AT = [50, 51, 52, 150, 300, 301]

tick = 0
records = []

for i in range(NUM_RECORDS):
    if i in DROP_AT:
        tick += TICK_INTERVAL_MS * 3
    else:
        tick += TICK_INTERVAL_MS

    t = i * 0.01
    record = struct.pack(
        STRUCT_FORMAT,
        SYNC,
        tick,
        math.sin(t) * 2.0,             # AccelX
        math.cos(t) * 1.5,             # AccelY
        9.81 + math.sin(t * 0.5) * 0.1,# AccelZ
        math.sin(t * 3) * 10.0,        # GyroX
        math.cos(t * 3) * 10.0,        # GyroY
        math.sin(t * 2) * 5.0,         # GyroZ
        200.0 + math.sin(t) * 50.0,    # MagX
        -100.0 + math.cos(t) * 30.0,   # MagY
        400.0 + math.sin(t * 0.3) * 20.0, # MagZ
        101325.0 - i * 0.5,            # PressurePa
        25.0 + math.sin(t) * 2.0,      # TemperatureC
        int(47.3 * 1e7),               # Latitude
        int(8.5 * 1e7),                # Longitude
        100.0 + i * 0.1,               # Altitude
        5.0 + math.sin(t) * 2.0,       # VelocityZ
        0,                              # Flags
        3.7 + math.sin(t * 0.1) * 0.3, # BatteryVoltage
        2,                              # State
        SYNC_END,
    )
    records.append(record)

output = "test_drops.BIN"
with open(output, "wb") as f:
    for r in records:
        f.write(r)

print(f"Wrote {len(records)} records to {output}")
print(f"Drops at indices: {DROP_AT}")
print(f"Record size: {struct.calcsize(STRUCT_FORMAT)} bytes")
print(f"File size: {len(records) * struct.calcsize(STRUCT_FORMAT)} bytes")
