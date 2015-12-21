#include <new>

#include "nju9103_eva.h"
#include "serial_wrapper.h"
#include "spi_command.h"
#include "timer_wrapper.h"
#include "adc_data_buffer.h"

static void isrRx();
static void isrPacketWatch();
static void dispatchCommand();
static uint8_t calculateChkSum(Packet *packet);
static uint8_t calculateChkSum(uint8_t *data);
static void returnResponse(Packet *packet);

static uint8_t rx_buffer[PACKET_SIZE];
static ReceiveDataStatus receiveDataStatus;
static Command command;

static ::Serial *uart;
static ::Timer *packetWatchTimer;
static ::SPICommand *dispatcher;
static ::ADCDataBuffer *adcDataBuffer;

void setup() {
    receiveDataStatus = 0;
    command = 0;

    uart = new ::Serial(UART_BAUDRATE, UART_BITS, UART_PARITY, UART_STOP);
    packetWatchTimer = new ::Timer();
    dispatcher = new ::SPICommand();
    adcDataBuffer = new ::ADCDataBuffer;

    // set Rx interrupt
    uart->attach(isrRx, ::Serial::RxIrq);
    packetWatchTimer->attach(isrPacketWatch, 300);
}

void loop() {
    dispatchCommand();
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

static bool checkAddressRange(uint8_t address) {
    return (address & 0xF0) == 0x00;
}

static bool validateCtrl(uint8_t ctrl) {
    bool result;
    switch (ctrl & 0x0F) {
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xC:
        case 0xD:
        case 0xF:
            result = true;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

static void dispatchCommand() {
    Packet packet;
    packet.Type = OP_UNKNOWN_ERROR;
    packet.Param = 0x00;
    packet.Data0 = 0x00;
    packet.Data1 = 0x00;
    ::SPICommand::Status status;
    if (command != 0) {
        if (command & CMD_RESET) {
            command ^= CMD_RESET;
            status = dispatcher->SPIReset();
            if (status == ::SPICommand::Success) {
                packet.Type = OP_SPI_RESET;
                packet.Param = 0x00;
                packet.Data0 = 0x00;
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_WRITE_8BIT) {
            command ^= CMD_WRITE_8BIT;
            if (checkAddressRange(rx_buffer[1])) {
                status = dispatcher->RegisterWrite8Bit(rx_buffer[1], rx_buffer[2]);
                if (status == ::SPICommand::Success) {
                    packet.Type = OP_REGISTER_WRITE_8BIT;
                    packet.Param = rx_buffer[1];
                    packet.Data0 = rx_buffer[2];
                    packet.Data1 = 0x00;
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_READ_8BIT) {
            command ^= CMD_READ_8BIT;
            if (checkAddressRange(rx_buffer[1])) {
                uint8_t rd = 0;
                status = dispatcher->RegisterRead8Bit(rx_buffer[1], &rd);
                if (status == ::SPICommand::Success) {
                    packet.Type = OP_REGISTER_READ_8BIT;
                    packet.Param = rx_buffer[1];
                    packet.Data0 = rd;
                    packet.Data1 = 0x00;
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_WRITE_16BIT) {
            command ^= CMD_WRITE_16BIT;
            if (checkAddressRange(rx_buffer[1])) {
                status = dispatcher->RegisterWrite16Bit(rx_buffer[1], rx_buffer[2], rx_buffer[3]);
                if (status == ::SPICommand::Success) {
                    packet.Type = OP_REGISTER_WRITE_16BIT;
                    packet.Param = rx_buffer[1];
                    packet.Data0 = rx_buffer[2];
                    packet.Data1 = rx_buffer[3];
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_READ_16BIT) {
            command ^= CMD_READ_16BIT;
            if (checkAddressRange(rx_buffer[1])) {
                uint8_t rd0 = 0;
                uint8_t rd1 = 0;
                status = dispatcher->RegisterRead16Bit(rx_buffer[1], &rd0, &rd1);
                if (status == ::SPICommand::Success) {
                    packet.Type = OP_REGISTER_READ_16BIT;
                    packet.Param = rx_buffer[1];
                    packet.Data0 = rd0;
                    packet.Data1 = rd1;
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_START_SINGLE) {
            command ^= CMD_START_SINGLE;
            if (validateCtrl(rx_buffer[1])) {
                uint16_t rd = 0;
                status = dispatcher->StartSingleConversion(rx_buffer[1], &rd);
                if (status == ::SPICommand::Success) {
                    packet.Type = OP_REGISTER_READ_16BIT;
                    packet.Param = rx_buffer[1];
                    packet.Data = rd;
                }
                else if (status == ::SPICommand::Abort) {
                    return;
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_START_CONTINUOUS) {
            command ^= CMD_START_CONTINUOUS;
            if (validateCtrl(rx_buffer[1])) {
                uint32_t length = (uint16_t)((rx_buffer[2] << 8) + rx_buffer[3]);

                uint32_t alignedLength = length + 1;
                int odd = alignedLength % (PACKET_SIZE / 2);
                if (odd != 0) {
                    alignedLength += (PACKET_SIZE / 2) - odd;
                }
                if (adcDataBuffer->Alloc(alignedLength)) {
                    uint16_t resultLength = 0;
                    status = dispatcher->StartContinuousConversion(rx_buffer[1], adcDataBuffer->GetBuffer(), length, &resultLength);
                    if (status == ::SPICommand::Success) {
                        packet.Type = OP_START_CONTINUOUS_CONVERSION;
                        packet.Param = rx_buffer[1];
                        packet.Data0 = (uint8_t)(length >> 8);
                        packet.Data1 = (uint8_t)(length & 0x00FF);
                    }
                    else if (status == ::SPICommand::Abort) {
                        return;
                    }
                }
                else {
                    packet.Type = OP_BUFFER_SIZE_ERROR;
                    packet.Param = OP_START_CONTINUOUS_CONVERSION;
                    packet.Data0 = (uint8_t)(length >> 8);
                    packet.Data1 = (uint8_t)(length & 0x00FF);
                }
            }
            else {
                packet.Type = OP_PARAMETER_ERROR;
                packet.Param = rx_buffer[1];
                packet.Data0 = rx_buffer[2];
                packet.Data1 = 0x00;
            }
        }
        else if (command & CMD_START_DUMP) {
            command ^= CMD_START_DUMP;
            packet.Type = OP_START_ADC_DATA_DUMP;
            packet.Param = 0x00;
            packet.Data0 = 0x00;
            packet.Data1 = 0x00;
        }
        else if (command & CMD_STOP_SINGLE) {
            command ^= CMD_STOP_SINGLE;
            packet.Type = OP_STOP_SINGLE_CONVERSION;
            packet.Param = 0x00;
            packet.Data0 = 0x00;
            packet.Data1 = 0x00;
        }
        else if (command & CMD_STOP_CONTINUOUS) {
            command ^= CMD_STOP_CONTINUOUS;
            packet.Type = OP_STOP_CONTINUOUS_CONVERSION;
            packet.Param = 0x00;
            packet.Data0 = 0x00;
            packet.Data1 = 0x00;
        }
        else if (command & CMD_STOP_DUMP) {
            command ^= CMD_STOP_DUMP;
            packet.Type = OP_STOP_ADC_DATA_DUMP;
            packet.Param = 0x00;
            packet.Data0 = 0x00;
            packet.Data1 = 0x00;
        }
        else if (command & CMD_UNKNOWN) {
            command ^= CMD_UNKNOWN;
        }
        returnResponse(&packet);

        if (packet.Type == OP_START_ADC_DATA_DUMP) {
            adcDataBuffer->Dump(returnResponse);
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
