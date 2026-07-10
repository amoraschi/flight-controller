# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

Embedded **rocket flight computer** on an **STM32H723VGTx** (Cortex-M7). It reads three sensors, runs a flight-phase state machine under FreeRTOS, logs telemetry to an SD card, and talks to a ground station over USART1.

> Note: although the part is rated to 550 MHz, the current CubeMX config runs SYSCLK from **HSI at 64 MHz** (voltage scale 3, FLASH latency 1). TIM2 is a free-running **1 µs** counter (prescaler 63) used for velocity timing.

## Build

Primary build is STM32CubeIDE (make-based). Output: `flight-controller.elf`.
```sh
make -j16 all
```

Alternative CMake + Ninja build (may have stale paths after a CubeMX regen — prefer CubeIDE):
```sh
cmake --preset Debug
cmake --build build/Debug
```

Toolchain: `arm-none-eabi-gcc`, C11, `-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard`.

## Critical rules

- **Always consult the relevant datasheet in `Datasheets/` before changing sensor, MCU, or peripheral code AND before answering design questions or recommending calibration/configuration approaches** (register addresses, bit fields, init/power sequences, timing, scale/range tables, application notes, hardware features). Verify register semantics against the datasheet rather than guessing -- a wrong bit or range silently corrupts telemetry, and a software workaround for something the hardware already provides wastes cycles and code. Available: `BMP581.pdf` (barometer), `IIM-42653.pdf` (IMU), `IIS2MDC.pdf` (magnetometer), `STM32H723VG-ZG.pdf` (MCU), `ZOE-M8Q.pdf` (GPS), plus `BMP280.pdf` and `LSM6DSL.pdf` (alternate/legacy parts).
- **Never edit `Drivers/` or `Middlewares/`** -- generated/managed by STM32CubeMX. Peripheral and pin config is in `flight-controller.ioc`; regenerate from there if peripherals change.
- **Scale factors are coupled to sensor range config**: if you change accel/gyro FS in `IIM42653Handler.c`, update `ACCEL_SCALE` / `GYRO_SCALE` in `IIM42653.c`. Pressure/temperature scales (`PRESSURE_SCALE`, `TEMPERATURE_SCALE`) live in `Barometer.c`. Always use precalculated `const float` reciprocals and multiply -- **never runtime division**.
- **IIM42653 requires bank-switching** before register access -- always go through `IIM42653_SelectBank()`.
- **BMP581 needs reset + ~100 ms delay** before first use; init checks chip ID, NVM-ready, and the POR bit (`INT_STATUS` 0x27).
- **`SensorData_t` layout must match `Graphs/decode_log.py`** -- if struct fields change type/order, update `STRUCT_FORMAT` (currently `"<I9f2f2i2f"`, 64 bytes/record).
- **Wire-protocol structs in `shared.h`** use `#pragma pack(push, 1)` -- don't add padding or reorder fields.
- **DMA RX buffer must stay in the `.dma_buffer` section** (D-cache coherency on Cortex-M7).
- **FreeRTOS ISR ceiling is 5** (`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`): any ISR calling a FreeRTOS API must be at NVIC priority >= 5 (numerically). DMA1_Stream0 is set to 5.
- **After adding/renaming source files**, update `target_sources()` in `CMakeLists.txt` (CubeIDE's make build auto-detects from `.project`, but the CMake build does not).

## Directory layout

```
Core/
  Inc/ {States, Sensors, SensorsHandlers, Tasks, Utils}/   # headers
  Src/
    States/          # per-state entry + handler (7 states)
    Sensors/         # drivers: BMP581, IIM42653, IIS2MDCTR (+ BMP280, unused)
    SensorsHandlers/ # *_Mode_Idle / *_Mode_Performance init sequences
    Tasks/           # 4 FreeRTOS tasks
    Callbacks/       # USART1/DMA ISR callbacks
    Utils/           # Altitude, Velocity, Barometer, HIL, SD, IMU, Magnetometer
Drivers/ Middlewares/ FATFS/   # generated (do not hand-edit)
Graphs/              # Python log analysis (decode_log.py, parse_log.py)
Datasheets/ cmake/   # references, toolchain files
flight-controller.ioc        # CubeMX project = source of truth for peripherals
PROTOCOL.md          # binary wire protocol spec
```

## Key files to read first

| File | Purpose |
|------|---------|
| `Core/Inc/Utils/shared.h` | All cross-module types (`SystemState_t`, `SensorData_t`, `StateEvent_t`, `SensorPayload_t`), packed wire-protocol structs, extern queue handles |
| `Core/Inc/Utils/configuration.h` | Tunable constants: thresholds, filter alpha, calibration sample counts, `HIL_MODE`, `EXTERNAL_COMMANDS` |
| `Core/Src/Tasks/vStateMachineTask.c` | Central state dispatcher; processes events from `xStateEventQueue` |
| `Core/Src/Tasks/vSensorTask.c` | Sensor poll loop (2 ms), altitude/velocity computation |
| `Core/Src/Utils/HIL.c` | Binary packet parser (0xCAFE sync); routes sensor data or commands |
| `PROTOCOL.md` | Binary wire-protocol specification |

## FreeRTOS architecture

Tasks created with `xTaskCreate()` in `main.c` after `osKernelInitialize()`:

| Task | Priority | Stack (words) | Role |
|------|----------|---------------|------|
| `vTelemetryTask` | IDLE+2 | 256 | USART1 DMA-to-idle RX, calls HIL parser |
| `vSensorTask` | IDLE+2 | 256 | Polls sensors every 2 ms, computes altitude/velocity, enqueues `StateEvent` |
| `vStateMachineTask` | IDLE+3 | 256 | Highest -- dequeues events, transitions states, enqueues SD-log events |
| `vSDLoggingTask` | IDLE+2 | 1024 | Double-buffered SD writes (100 records/flush, `f_sync` after each flush) |

The CubeMX `defaultTask` immediately `vTaskDelete(NULL)`s itself. `printf` is retargeted to USART1 (blocking) via `_write()`.

**Queues** (created in `main.c`):

| Handle | Length | Item | Notes |
|--------|--------|------|-------|
| `xStateEventQueue` | 3 | `StateEvent_t` | Commands inserted at front via `xQueueSendToFront()` |
| `xSensorModeQueue` | 1 | `SensorMode_t` | Always-latest via `xQueueOverwrite()` |
| `xSDLoggingQueue` | 3 | `SDLoggingEvent_t` | |
| `xHILModeQueue` | 3 | `SensorPayload_t` | Created only when `HIL_MODE=1` |

**ISR handoff:** `HAL_UARTEx_RxEventCallback` (USART1, in `Callbacks/vTelemetryTaskCallback.c`) → `xTaskNotifyFromISR()` (eSetValueWithOverwrite, carries byte count) → `vTelemetryTask` wakes via `xTaskNotifyWait()`, parses, re-arms `HAL_UARTEx_ReceiveToIdle_DMA`.

**Data flow:** Sensors → `vSensorTask` (compute altitude/velocity) → `xStateEventQueue` → `vStateMachineTask` (transition logic) → `xSDLoggingQueue` → `vSDLoggingTask` (write to SD).

## State machine

States (`SystemState_t` in `shared.h`):

```
STATE_IDLE → STATE_CALIBRATION → STATE_PRELAUNCH → STATE_BURN → STATE_APOGEE → STATE_PARACHUTE → STATE_LANDED
                                ↘ (abort) ↘ (abort)
                      GROUND_ABORT   ASCENT_ABORT   DESCENT_ABORT
```

`STATE_PRELAUNCH` exists but is unused.

Dispatch order in `vStateMachineTask.c` per event:
1. **Global commands first** (`xHandleGlobalCommand`): `COMMAND_RESET`→IDLE, `COMMAND_ABORT`→GROUND_ABORT, `COMMAND_CALIBRATION`→CALIBRATION (only from IDLE).
2. Otherwise the current state handler runs: `xXxxStateHandler(ctx, event, rx_status)` returns the next state.
3. The IDLE and CALIBRATION handlers transition to GROUND_ABORT when `xSystemFaultFlags.Flags != 0`.

**Transition thresholds** (`configuration.h`):
- PRELAUNCH→BURN: `VelocityZ > 10.0` m/s
- BURN→APOGEE: `VelocityZ < 5.0` m/s
- APOGEE→PARACHUTE: `Altitude < 500.0` m
- PARACHUTE→LANDED: `|VelocityZ| < 5.0` m/s

Each state has a `vXxxStateEntry()` (runs once on entry) + `xXxxStateHandler()` (runs per event) pair. CALIBRATION entry resets the velocity window and pressure-calibration accumulators and switches sensors to PERFORMANCE; once the reference pressure becomes valid it sets `SDLoggingEnabled = true`.

> Test-code caveat: `xIdleStateHandler` currently auto-advances IDLE→CALIBRATION as soon as `SensorsIdleFinished` is set (set by `vApplyMode` after idle init). Both spots are marked "test code, remove later".

## Sensors

`vSensorTask` polls every 2 ms (`vTaskDelay`, **not** `vTaskDelayUntil` — `xLastWakeTime` is declared but unused). Modes `SENSOR_MODE_IDLE` (low power) vs `SENSOR_MODE_PERFORMANCE` (high rate) are switched by posting to `xSensorModeQueue`; each sensor has `*_Mode_Idle()` / `*_Mode_Performance()` in its `*Handler.c`.

| Sensor | Bus | Addr / CS | WHO_AM_I | Output |
|--------|-----|-----------|----------|--------|
| BMP581 (barometer) | I2C2 (`hi2c2`) | `0x46<<1` | — | pressure (float Pa), temperature (float °C) |
| IIM42653 (IMU) | SPI2 (`hspi2`) | CS = PD8 (soft NSS) | 0x56 @ reg 0x75 | accel XYZ (float g) + gyro XYZ (float dps), 12-byte burst from `ACCEL_DATA_X1` (0x1F) |
| IIS2MDCTR (magnetometer) | I2C1 (`hi2c1`) | `0x1E<<1` | 0x40 @ reg 0x4F | mag XYZ (raw int16, 6-byte read) |

BMP280 (`0x76<<1`) remains in the tree but is commented out — replaced by BMP581.

**Scale factors (`IIM42653.c`):**
- Accel: **±32 G**, `ACCEL_SCALE = 1/1024` (1024 LSB/g)
- Gyro: **±2000 DPS**, `GYRO_SCALE = 1/16.4` (16.4 LSB/(°/s))
- Both at **500 Hz ODR**, Low-Noise mode, UI filter order 1, BW setting 16
- Sensor data output is **big-endian** (matches byte assembly in `IIM42653_GetData`)

**Scale factors (`Barometer.c`):**
- Pressure: `PRESSURE_SCALE = 1/64` (6 fractional bits)
- Temperature: `TEMPERATURE_SCALE = 1/65536` (16 fractional bits)
- BMP581 performance: OSR ×128 pressure / ×8 temperature, IIR coeff ×1, continuous mode

The magnetometer output is forwarded raw (no scale) at 20 Hz continuous (BDU, LPF, offset cancellation, temp comp).

**DATA_RDY gating (was a known bug, now mitigated):** the IMU previously showed gyro spikes (±600–1000 dps stationary) from register tearing — reading at 500 Hz while the sensor updated internally. `vSensorTask` now polls `IIM42653_CheckDataReady()` (INT_STATUS 0x2D bit 3, enabled via `IIM42653_EnableDataReady` on `INT_SOURCE0`) and only does the burst read when new data is ready, otherwise reusing the last (`static`) sample. **BMP581 and IIS2MDCTR are still read unconditionally every cycle** (no DRDY gating).

## Altitude & velocity

`Core/Src/Utils/Altitude.c` and `Velocity.c`:
- **Reference pressure** (`Barometer.c::CalibratePressure`): discards first `PRESSURE_CALIBRATION_DISCARD_SAMPLES` (100) samples, then averages the next `PRESSURE_CALIBRATION_SAMPLES` (200). Runs during CALIBRATION.
- **Altitude:** hypsometric equation `(GAS_CONSTANT·T_K / GRAV_CONSTANT)·ln(P_ref / P)`, with `GAS_CONSTANT = 287.0`, `GRAV_CONSTANT = 9.80665` (`Altitude.h`). Returns 0 until the reference is valid.
- **Filtered altitude:** IIR, `ALTITUDE_IIR_FILTER_ALPHA = 0.1`; first valid sample initializes the filter (no cold-start lag).
- **Velocity Z:** slope over a sliding window of the last `VELOCITY_Z_WINDOW_SIZE` (10) altitude samples, timed with the TIM2 µs counter.

## Wire protocol

Full spec in `PROTOCOL.md`. Little-endian. Sync header `0xCAFE` (`0xFE 0xCA` on wire) + 1-byte `msg_type`:
- **MSG_DATA (0x01):** 41 bytes total (38-byte `SensorPayload_t`): accel/gyro/mag raw int16, pressure/temp float, lat/lon int32 (×1e7).
- **MSG_COMMAND (0x02):** 4 bytes; `command_type` 0x01=RESET, 0x02=ABORT, 0x03=CALIBRATION.

`HIL.c` runs a byte-level parser (IDLE→HEADER_1→HEADER_2→PAYLOAD) with resync on a bad second header byte. Commands are pushed to the front of `xStateEventQueue` (gated by `EXTERNAL_COMMANDS`). USART1 is 115200 8N1, DMA RX to-idle, 128-byte buffer in `.dma_buffer`.

## SD logging

`Core/Src/Tasks/vSDLoggingTask.c` + `Utils/SD.c`:
- `MountAndOpen()` runs at task start (after the kernel is up), opens `log.bin` (`FA_CREATE_ALWAYS`).
- Double-buffered `BufferA`/`BufferB`, each `SD_LOGGING_RECORDS_PER_BUFFER` (100) `SensorData_t` records. On full, swap and write via FatFs `f_write`, then `f_sync` after every flush.
- Logging only accumulates when `SDLoggingEnabled` (set during CALIBRATION once reference pressure is valid).
- **Known gap:** final partial-buffer flush on STATE_LANDED is commented out (`vSDLoggingTask.c:51-58`).

## Log analysis (`Graphs/`)

- **`decode_log.py`** — decodes a binary log of `SensorData_t` into `decoded_log.csv`. `STRUCT_FORMAT = "<I9f2f2i2f"` (must match `SensorData_t`). Note: it currently hardcodes the input path (`LOG_bad.BIN`) and regenerates timestamps at 2 ms steps rather than using the stored `TimestampMS`; its docstring mislabels pressure/temperature as int32 (the format string and struct are float).
- **`parse_log.py`** — reads `decoded_log.csv` and plots accel/gyro/mag/pressure/temperature/altitude/velocity.

## HIL (hardware-in-the-loop) mode

Set `#define HIL_MODE 1` in `configuration.h` (currently `0`). When enabled, `vSensorTask` reads simulated `SensorPayload_t` from `xHILModeQueue` (fed by ground-station `MSG_DATA` packets) instead of real I2C/SPI; altitude/velocity/state-machine logic is unchanged.

## Known issues & active TODOs

| Location | Issue |
|----------|-------|
| `BMP581Handler.c:10` | Reset write sometimes doesn't ACK -- currently ignored, may mask init failure |
| `Barometer.c:21-26` | `dbg_ref_*` debug variables left in production code -- TODO: Remove |
| `xBurnStateHandler.c:11` | Burn→Apogee threshold needs hysteresis / acceleration-based detection |
| `vSDLoggingTask.c:51-58` | Landing buffer flush commented out (incomplete) |
| `xIdleStateHandler.c:22-25` + `vSensorTask.c::vApplyMode` | Test code auto-advances IDLE→CALIBRATION -- remove later |
| `vSensorTask.c` | BMP581 / IIS2MDCTR still read without a DATA_RDY check (IMU is now gated) |
| `vSensorTask.c:77` | `xLastWakeTime` declared but unused (uses `vTaskDelay`) |
| `IIM42653Handler.c:54` | Performance-mode timeout needs revision |
| All `vXxxStateEntry()` | Marked `// TODO: Refine` -- work but not fully validated |

## Coding conventions

- Types: `_t` suffix. Global extern handles: `x` prefix. Task-locals: CamelCase. Macros: UPPER_CASE.
- Per-state pattern: `vXxxStateEntry()` + `xXxxStateHandler()` pair.
- Sensor drivers: register addresses/bit positions as macros at the top of each driver file.
- Bit helpers: `BIT(n)`, `BITS_SET(value, mask)`, `BITS_CLEAR(value, mask)`.
- I2C/SPI timeouts: explicit **100 ms** everywhere (`HAL_MAX_DELAY` is not used for sensor I/O).
- Sensor scaling: precalculated `const float` reciprocals, multiply (never divide at runtime).
