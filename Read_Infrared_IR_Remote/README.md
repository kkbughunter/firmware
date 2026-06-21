# ESP32 IR Remote Reader

This project reads commands from a typical **38 kHz, three-pin infrared receiver module** (for example, VS1838B, TSOP38238, or a compatible module). It prints the decoded protocol, address, command, raw value, repeat status, and raw pulse timings to the PlatformIO serial monitor.

## Parts

- ESP32 Dev Module
- 3.3 V-compatible IR receiver or receiver module
- Infrared remote control
- Three jumper wires

## Wiring diagram

```text
        ESP32 Dev Module                         IR receiver module
      +--------------------+                    +------------------+
      |                    |                    |                  |
      | 3V3                +--------------------+ VCC / +          |
      |                    |                    |                  |
      | GND                +--------------------+ GND / -          |
      |                    |                    |                  |
      | GPIO27             +--------------------+ OUT / S / SIGNAL |
      |                    |                    |                  |
      +--------------------+                    +------------------+

                    Remote  - - - IR light - - ->  Receiver dome
```

| IR receiver | ESP32 |
|---|---|
| `VCC` / `+` | `3V3` |
| `GND` / `-` | `GND` |
| `OUT` / `S` | `GPIO27` |

> [!WARNING]
> Receiver pin order is **not standardized**. Some bare receivers use `OUT-GND-VCC`, while others use a different order. Read the labels on the module or its datasheet before applying power. Reversing VCC and GND can permanently damage it.

> [!WARNING]
> Do not feed a 5 V output into an ESP32 GPIO. Power a compatible receiver module from 3.3 V, or verify from its datasheet that its output is safe at 3.3 V logic levels.

## Build, upload, and read a remote

1. Turn off or unplug the ESP32 while making connections.
2. Wire the receiver according to the table and verify its exact pin order.
3. Build and upload the project in PlatformIO.
4. Open **PlatformIO: Serial Monitor** at **115200 baud**.
5. Point the remote at the receiver and press a button.

Example output:

```text
--- IR signal received ---
Protocol=NEC Address=0x0 Command=0x45 Raw-Data=0xBA45FF00 32 bits LSB first
Protocol: NEC
Address:  0x0
Command:  0x45
Raw data: 0xBA45FF00
Repeat:   no
rawData[68]:
 -8950, 4450 ...
```

The `Command` value normally identifies the pressed button. Holding a button may produce messages with `Repeat: yes`. Unknown protocols can still be investigated using the printed raw mark/space timings.

## Changing the signal pin

Edit this line in `src/main.cpp`:

```cpp
constexpr uint8_t IR_RECEIVE_PIN = 27;
```

Use an ESP32 GPIO that is available as a digital input. Avoid GPIO 6 through 11 because they are normally connected to the module's flash memory.

## Troubleshooting

- **No output:** check VCC/GND pin order, use a known working remote, move closer, and confirm the monitor is at 115200 baud.
- **Random readings:** shorten the wires and keep the sensor away from direct sunlight or bright fluorescent/LED lighting.
- **Buffer overflow:** increase `RAW_BUFFER_LENGTH` before including `IRremote.hpp` if the remote sends unusually long frames.
- **Only repeats appear:** release the key, press it once, and ensure the receiver has a clear line of sight.
