#ifndef __NJU9103_EVA_H__
#define __NJU9103_EVA_H__

#include "serial_wrapper.h"

#define PACKET_SIZE 4

#define UART_BAUDRATE 115200
#define UART_BITS 8
#define UART_PARITY ::Serial::None
#define UART_STOP 1

/* Operation Code */
#define OP_REGISTER_WRITE_BYTE         0x10
#define OP_REGISTER_READ_BYTE          0x11
#define OP_REGISTER_WRITE_SHORT        0x12
#define OP_REGISTER_READ_SHORT         0x13
#define OP_START_SINGLE_CONVERSION     0x20
#define OP_START_CONTINUOUS_CONVERSION 0x30
#define OP_START_ADC_DATA_DUMP         0x31
#define OP_STOP_CONTINUOUS_CONVERSION  0x40
#define OP_STOP_ADC_DATA_DUMP          0x41

typedef uint8_t ReceiveDataStatus;
/* Receive Data Status Flag */
#define RX_STATUS_CHKSUM_ERROR  0x01
#define RX_STATUS_DATA_RECEIVED 0x02

typedef uint16_t Command;
#define CMD_WRITE_BYTE       0x0001
#define CMD_READ_BYTE        0x0002
#define CMD_WRITE_SHORT      0x0004
#define CMD_READ_SHORT       0x0008
#define CMD_START_SINGLE     0x0010
#define CMD_START_CONTINUOUS 0x0020
#define CMD_START_DUMP       0x0040
#define CMD_STOP_CONTINUOUS  0x0080
#define CMD_STOP_DUMP        0x0100

static uint8_t calculateChkSum(uint8_t *data);
static void isrRx();
static void isrMainProc();
static void isrSubProc();

//static CommandStatus RegisterWriteByte(uint8_t address, uint8_t regData);
//static CommandStatus RegisterReadByte(uint8_t address, uint8_t *regData);
//static CommandStatus RegisterWriteShort(uint8_t address, uint16_t regData);
//static CommandStatus RegisterReadShort(uint8_t address, uint16_t *regData);
//static CommandStatus StartSingleConversion(uint8_t channel, uint16_t *adcData);
//static CommandStatus StartContinuousConversion(uint8_t channel, uint16_t *buf, uint16_t length, uint16_t *resultLength);
//static CommandStatus StartADCDataDump();
//static CommandStatus StopContinuousConversion();
//static CommandStatus StopADCDataDump();

#endif
