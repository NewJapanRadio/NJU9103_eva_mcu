#include <new>

#include "nju9103_eva.h"
#include "serial_wrapper.h"
#include "spi_command.h"
#include "timer_wrapper.h"
#include "adc_data_buffer.h"
#include "dispatcher.h"

static void isrRx();
static void isrPacketWatch();
static uint8_t calculateChkSum(Packet *packet);
static uint8_t calculateChkSum(uint8_t *data);
static void returnResponse(Packet *packet);

static uint8_t rx_buffer[PACKET_SIZE];
static ReceiveDataStatus receiveDataStatus;
static Command command;

static ::Serial *uart;
static ::Timer *packetWatchTimer;
static ::Dispatcher *dispatcher;
static ::ADCDataBuffer *adcDataBuffer;

void setup() {
    receiveDataStatus = 0;
    command = 0;

    uart = new ::Serial(UART_BAUDRATE, UART_BITS, UART_PARITY, UART_STOP);
    packetWatchTimer = new ::Timer();
    dispatcher = new ::Dispatcher();
    adcDataBuffer = new ::ADCDataBuffer;

    // set Rx interrupt
    uart->attach(isrRx, ::Serial::RxIrq);
    packetWatchTimer->attach(isrPacketWatch, 300);
}

void loop() {
    Packet packet;

    if (command != 0) {
        dispatcher->Dispatch(&command, rx_buffer, &packet, adcDataBuffer);

        returnResponse(&packet);
        if (packet.Type == OP_START_ADC_DATA_DUMP) {
            adcDataBuffer->Dump(returnResponse);
        }
    }
}

static void isrRx() {
    static char buf[PACKET_SIZE];
    static uint8_t count = 0;
    while (uart->readable()) {
        if (count < sizeof(buf)) {
            buf[count] = uart->read();
        }
        else if (count == sizeof(buf)) {
            receiveDataStatus = RX_STATUS_DATA_RECEIVED;
            if (uart->read() == calculateChkSum((uint8_t*)buf)) {
                memcpy(rx_buffer, buf, sizeof(uint8_t) * PACKET_SIZE);
            }
            else {
                receiveDataStatus |= RX_STATUS_CHKSUM_ERROR;
            }
        }
        count = (count + 1) % (sizeof(buf) + 1);
    }
}

static void isrPacketWatch() {
    if (receiveDataStatus & RX_STATUS_DATA_RECEIVED) {
        receiveDataStatus ^= RX_STATUS_DATA_RECEIVED;
        if ((receiveDataStatus & RX_STATUS_CHKSUM_ERROR) == 0) {
            switch (rx_buffer[0]) {
                case OP_SPI_RESET :
                    command |= CMD_RESET;
                    break;
                case OP_REGISTER_WRITE_8BIT :
                    command |= CMD_WRITE_8BIT;
                    break;
                case OP_REGISTER_READ_8BIT :
                    command |= CMD_READ_8BIT;
                    break;
                case OP_REGISTER_WRITE_16BIT :
                    command |= CMD_WRITE_16BIT;
                    break;
                case OP_REGISTER_READ_16BIT :
                    command |= CMD_READ_16BIT;
                    break;
                case OP_START_SINGLE_CONVERSION :
                    command |= CMD_START_SINGLE;
                    break;
                case OP_START_CONTINUOUS_CONVERSION :
                    command |= CMD_START_CONTINUOUS;
                    break;
                case OP_START_ADC_DATA_DUMP :
                    command |= CMD_START_DUMP;
                    break;
                case OP_STOP_SINGLE_CONVERSION :
                    dispatcher->SetAbortRequest();
                    command |= CMD_STOP_SINGLE;
                    break;
                case OP_STOP_CONTINUOUS_CONVERSION :
                    dispatcher->SetAbortRequest();
                    command |= CMD_STOP_CONTINUOUS;
                    break;
                case OP_STOP_ADC_DATA_DUMP :
                    adcDataBuffer->SetAbortRequest();
                    command |= CMD_STOP_DUMP;
                    break;
                default :
                    command |= CMD_UNKNOWN;
                    break;
            }
        }
        else {
            receiveDataStatus ^= RX_STATUS_CHKSUM_ERROR;

            Packet packet;
            packet.Type = OP_CHKSUM_ERROR;
            packet.Param = 0x00;
            packet.Data0 = 0x00;
            packet.Data1 = 0x00;
            returnResponse(&packet);
        }
    }
}

static uint8_t calculateChkSum(Packet *packet) {
    uint8_t chksum = 0;
    chksum = (chksum + packet->Type) & 0xFF;
    chksum = (chksum + packet->Param) & 0xFF;
    chksum = (chksum + packet->Data0) & 0xFF;
    chksum = (chksum + packet->Data1) & 0xFF;
    return ~chksum;
}

static uint8_t calculateChkSum(uint8_t *data) {
    uint8_t chksum = 0;
    for (int i = 0; i < PACKET_SIZE; i++) {
        chksum = (chksum + *data++) & 0xFF;
    }
    return ~chksum;
}

static void returnResponse(Packet *packet) {
    uart->write(packet->Type);
    uart->write(packet->Param);
    uart->write(packet->Data0);
    uart->write(packet->Data1);
    uart->write(calculateChkSum(packet));
}
