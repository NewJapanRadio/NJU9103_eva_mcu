# mbed program for NJU9103 Evaluation Board

## Target Platform
[NUCLEO-F411RE](https://developer.mbed.org/platforms/ST-Nucleo-F411RE/)

## UART Setting

|           |        |
|-----------|-------:|
| Baud rate | 115200 |
| Data bits | 8      |
| Stop bits | 1      |
| Parity    | None   |

## Packet format

```c
{ Header, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, ChkSum }
```

### Checksum

Checksum is bit inversion of sum of the byte from Header to Byte7.  
`~(Header + Byte0 + ... + Byte7)`

**ex)**
```c
{ 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } -> 0xEF
```

```c
{ 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7 } -> 0x2C
```

### Header Type

| Type     | Value | Description                                |
|----------|------:|--------------------------------------------|
| Command  | 0x10  | Command from master (e.g. your PC)         |
| Response | 0x20  | Response from MCU as slave                 |
| Data     | 0x30  | Data from MCU. It's given at ADC Data Dump |
| Error    | 0xF0  | Error                                      |

### Command List

| Command                     | Value | Description                                        |
|-----------------------------|------:|----------------------------------------------------|
| SPIReset                    |  0x00 | Send SPI Reset command to NJU9103                  |
| RegisterWrite8Bit           |  0x10 | Write 1 byte data to register                      |
| RegisterRead8Bit            |  0x20 | Read 1 byte data from register                     |
| RegisterWrite16Bit          |  0x30 | Write 2 byte data to register                      |
| RegisterRead16Bit           |  0x40 | Read 2 byte data from register                     |
| StartSingleConversion       |  0x50 | Start Single Conversion                            |
| StartContinuousConversion   |  0x60 | Start Continuous Conversion                        |
| StartIntermittentConversion |  0x70 | Start Intermittent Conversion with a time interval |
| StartADCDataDump            |  0x80 | Dump ADC conversion data                           |
| StopSingleConversion        |  0x51 | Stop Single Conversion                             |
| StopContinuousConversion    |  0x61 | Stop Continuous Conversion                         |
| StopIntermittentConversion  |  0x71 | Stop Intermittent Conversion                       |
| StopADCDataDump             |  0x81 | Stop ADC conversion data dump                      |

##### Error

- Checksum Error

```c
{ 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10(ChkSum) }
```

- Command Undefined

```c
{ 0xF0, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13(ChkSum) }
```

### Reset UART State Machine

Consecutive ten **0x00** bytes initialize UART state machine inside the MCU.  
If you lose UART command sending state, UART Reset gets it back to start position.

### Command Reference

#### SPIReset

##### Send Bytes

```c
{ 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF(ChkSum) }
```

##### Response

```c
{ 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF(ChkSum) }
```

#### RegisterWrite8Bit

##### Send Bytes

```c
{ 0x10, 0x10, Address, WriteData, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x10, Address, WriteData, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Register Address

```c
{ 0xF0, 0xFE, 0x10, Address, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### RegisterRead8Bit

##### Send Bytes

```c
{ 0x10, 0x20, Address, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```
{ 0x20, 0x20, Address, ReadData, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Register Address

```
{ 0xF0, 0xFE, 0x20, Address, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### RegisterWrite16Bit

##### Send Bytes

```c
{ 0x10, 0x30, Address, WriteData0, WriteData1, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x30, Address, WriteData0, WriteData1, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Register Address

```c
{ 0xF0, 0xFE, 0x30, Address, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### RegisterRead16Bit

##### Send Bytes

```c
{ 0x10, 0x40, Address, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x40, Address, ReadData0, ReadData1, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Register Address

```
{ 0xF0, 0xFE, 0x40, Address, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### StartSingleConversion

- Control : { CHESE[2:0], MODE[3:0] }  
Forbid Continuous Conversion setting to MODE bits.

##### Send Bytes

```c
{ 0x10, 0x60, Control, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x50, Control, HighByte, LowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Control Value

```
{ 0xF0, 0xFE, 0x50, Control, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### StartContinuousConversion

StartContinuousConversion command acts as blocking.  
Response is returned at end of all conversion.

- Control : { CHESE[2:0], MODE[3:0] }  
Forbid Single Conversion setting to MODE bits.

- Length : (LengthHighByte << 8) + LengthLowByte + 1

##### Send Bytes

```c
{ 0x10, 0x60, Control, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x60, Control, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Control Value

```c
{ 0xF0, 0xFE, 0x60, Control, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### Buffer size Overflow

It's thrown when memory allocation is failed.

```c
{ 0xF0, 0xFD, 0x60, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### StartIntermittentConversion

StartIntermittentConversion command acts as blocking.  
Response is returned at end of all conversion.

- Control : { CHESE[2:0], MODE[3:0] }  
Forbid Single Conversion setting to MODE bits.

- Length : (LengthHighByte << 8) + LengthLowByte + 1

##### Send Bytes

```c
{ 0x10, 0x70, Control, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Response

```c
{ 0x20, 0x70, Control, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

##### Error

###### Invalid Control Value

```c
{ 0xF0, 0xFE, 0x70, Control, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### Buffer size Overflow

It's thrown when memory allocation is failed.

```c
{ 0xF0, 0xFD, 0x70, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

#### StartADCDataDump

##### Send Bytes

```c
{ 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F(ChkSum) }
```

##### Response

```c
{ 0x20, 0x80, 0x00, LengthHighByte, LengthLowByte, 0x00, 0x00, 0x00, 0x00, ChkSum,
  0x30, Data0High, Data0Low, Data1High, Data1Low, Data2High, Data2Low, Data3High, Data3Low, ChkSum,
  0x30, Data4High, Data4Low, Data5High, Data5Low, Data6High, Data6Low, Data7High, Data7Low, ChkSum,
  ...
  0x30, Data(N-1)High, Data(N-1)Low, Data(N)High, Data(N)Low, 0x00, 0x00, 0x00, 0x00, ChkSum }
```

- Length : (LengthHighByte << 8) + LengthLowByte + 1  
The Length is number without zero padding

#### StopSingleConversion

##### Send Bytes

```
{ 0x10, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E(ChkSum) }
```

##### Response

```
{ 0x10, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E(ChkSum) }
```

#### StopContinuousConversion

##### Send Bytes

```c
{ 0x10, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E(ChkSum) }
```

##### Response

```c
{ 0x20, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E(ChkSum) }
```

#### StopIntermittentConversion

##### Send Bytes

```c
{ 0x10, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E(ChkSum) }
```

##### Response

```c
{ 0x20, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E(ChkSum) }
```

#### StopADCDataDump

##### Send Bytes

```c
{ 0x10, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E(ChkSum) }
```

##### Response

```c
{ 0x10, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E(ChkSum) }
```
