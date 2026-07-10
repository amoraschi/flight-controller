# Telemetry and Command Protocol Specification

This document outlines the simplified binary protocol used for communication between the Ground Station / HIL simulation and the STM32 Flight Computer.

## Overview
All packets use Little-Endian byte ordering. 
A packet is identified by a sync header (`0xCAFE`), followed immediately by a `msg_type` byte.
On the wire, both `msg_type` and `command_type` are serialized as single-byte (`uint8_t`) values.

## Message Types

| Value | Identifier    | Description | Payload Size | Total Size |
| ----- |---------------|-------------|-------------|------------|
| 0x01  | `MSG_DATA`    | Telemetry   | 38 Bytes    | 41 Bytes   |
| 0x02  | `MSG_COMMAND` | Command     | 1 Byte      | 4 Bytes    |
---

## Packet Structures

### 1. Data Packet (`MSG_DATA` = 0x01)
*Total Size: 41 Bytes*

| Offset (Bytes) | Field Name     | Type       | Description                       |
| -------------- | -------------- | ---------- |-----------------------------------|
| 0              | `header`       | `uint16_t` | `0xCAFE`                          |
| 2              | `msg_type`     | `uint8_t`  | `0x01`                            |
| 3              | `timestamp_ms` | `uint32_t` | System uptime in milliseconds     |
| 7              | `ax`           | `int16_t`  | Accelerometer X (raw)             |
| 9              | `ay`           | `int16_t`  | Accelerometer Y (raw)             |
| 11             | `az`           | `int16_t`  | Accelerometer Z (raw)             |
| 13             | `gx`           | `int16_t`  | Gyroscope X (raw)                 |
| 15             | `gy`           | `int16_t`  | Gyroscope Y (raw)                 |
| 17             | `gz`           | `int16_t`  | Gyroscope Z (raw)                 |
| 19             | `mx`           | `int16_t`  | Magnetometer X (raw)              |
| 21             | `my`           | `int16_t`  | Magnetometer Y (raw)              |
| 23             | `mz`           | `int16_t`  | Magnetometer Z (raw)              |
| 25             | `press`        | `float`    | Pressure (Pascals)                |
| 29             | `temp`         | `float`    | Temperature (Celsius)             |
| 33             | `lat`          | `int32_t`  | Latitude (Decimal Degrees * 1e7)  |
| 37             | `lon`          | `int32_t`  | Longitude (Decimal Degrees * 1e7) |

### 2. Command Packets (`MSG_COMMAND` = 0x02)
*Total Size: 4 Bytes*

| Offset (Bytes) | Field Name     | Type       | Description                                                          |
|----------------|----------------|------------|----------------------------------------------------------------------|
| 0              | `header`       | `uint16_t` | `0xCAFE`                                                             |
| 2              | `msg_type`     | `uint8_t`  | `0x02`                                                               |
| 3              | `command_type` | `uint8_t`  | `COMMAND_RESET` (0x01), `COMMAND_ABORT` (0x02), or `COMMAND_CALIBRATION` (0x03) |
