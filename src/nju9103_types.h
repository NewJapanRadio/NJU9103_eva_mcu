#ifndef __NJU9103_TYPES_H__
#define __NJU9103_TYPES_H__

#ifdef UNIT_TEST
#   define NJRC_STATIC
#else
#   define NJRC_STATIC static
#endif

#include <stdint.h>

#define MAX_BUFFER_SIZE 0x4000

#define PACKET_SIZE 9 // w/o Header

enum PacketType {
    CommandHeader  = 0x10,
    ResponseHeader = 0x20,
    DataHeader     = 0x30,
    ErrorHeader    = 0xF0
};

typedef struct {
    uint8_t Header;
    union {
        uint16_t ADCData0;
        struct {
            uint8_t Type;
            uint8_t Param;
        };
        struct {
            uint8_t Byte0;
            uint8_t Byte1;
        };
    };
    union {
        uint16_t ADCData1;
        uint16_t Data;
        struct {
            uint8_t Data0;
            uint8_t Data1;
        };
        struct {
            uint8_t Byte2;
            uint8_t Byte3;
        };
    };
    union {
        uint16_t ADCData2;
        struct {
            uint8_t Byte4;
            uint8_t Byte5;
        };
    };
    union {
        uint16_t ADCData3;
        struct {
            uint8_t Byte6;
            uint8_t Byte7;
        };
    };
} Packet;

#define UART_BAUDRATE 115200
#define UART_BITS 8
#define UART_PARITY ::Serial::None
#define UART_STOP 1

/* Operation Code */
#define OP_SPI_RESET                   0x00
#define OP_REGISTER_WRITE_8BIT         0x10
#define OP_REGISTER_READ_8BIT          0x20
#define OP_REGISTER_WRITE_16BIT        0x30
#define OP_REGISTER_READ_16BIT         0x40
#define OP_START_SINGLE_CONVERSION     0x50
#define OP_START_CONTINUOUS_CONVERSION 0x60
#define OP_START_ADC_DATA_DUMP         0x70
#define OP_STOP_SINGLE_CONVERSION      0x51
#define OP_STOP_CONTINUOUS_CONVERSION  0x61
#define OP_STOP_ADC_DATA_DUMP          0x71

#define OP_CHKSUM_ERROR                0xFF
#define OP_PARAMETER_ERROR             0xFE
#define OP_BUFFER_SIZE_ERROR           0xFD
#define OP_UNKNOWN_ERROR               0xFC

typedef uint8_t ReceiveDataStatus;
/* Receive Data Status Flag */
#define RX_STATUS_CHKSUM_ERROR  0x01
#define RX_STATUS_DATA_RECEIVED 0x02

typedef uint16_t Command;
#define CMD_RESET            0x1000
#define CMD_WRITE_8BIT       0x0001
#define CMD_READ_8BIT        0x0002
#define CMD_WRITE_16BIT      0x0004
#define CMD_READ_16BIT       0x0008
#define CMD_START_SINGLE     0x0010
#define CMD_START_CONTINUOUS 0x0020
#define CMD_START_DUMP       0x0040
#define CMD_STOP_SINGLE      0x0080
#define CMD_STOP_CONTINUOUS  0x0100
#define CMD_STOP_DUMP        0x0200
#define CMD_UNKNOWN          0x8000

#endif
