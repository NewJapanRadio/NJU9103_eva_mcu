#include "spi_command.h"
#include "dispatcher.h"

static bool checkAddressRange(uint8_t address);
static bool validateCtrl(uint8_t ctrl);

Dispatcher::Dispatcher() {
    spiCommand = new ::SPICommand();
}

Dispatcher::~Dispatcher() {
    delete spiCommand;
}

void Dispatcher::Dispatch(Command *command, uint8_t *rx_buffer, Packet *packet, ::ADCDataBuffer *adcDataBuffer) {
    packet->Type = OP_UNKNOWN_ERROR;
    packet->Param = 0x00;
    packet->Data0 = 0x00;
    packet->Data1 = 0x00;
    ::SPICommand::Status status;
    if (*command != 0) {
        if (*command & CMD_RESET) {
            *command ^= CMD_RESET;
            status = spiCommand->SPIReset();
            if (status == ::SPICommand::Success) {
                packet->Type = OP_SPI_RESET;
                packet->Param = 0x00;
                packet->Data0 = 0x00;
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_WRITE_8BIT) {
            *command ^= CMD_WRITE_8BIT;
            if (checkAddressRange(rx_buffer[1])) {
                status = spiCommand->RegisterWrite8Bit(rx_buffer[1], rx_buffer[2]);
                if (status == ::SPICommand::Success) {
                    packet->Type = OP_REGISTER_WRITE_8BIT;
                    packet->Param = rx_buffer[1];
                    packet->Data0 = rx_buffer[2];
                    packet->Data1 = 0x00;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_READ_8BIT) {
            *command ^= CMD_READ_8BIT;
            if (checkAddressRange(rx_buffer[1])) {
                uint8_t rd = 0;
                status = spiCommand->RegisterRead8Bit(rx_buffer[1], &rd);
                if (status == ::SPICommand::Success) {
                    packet->Type = OP_REGISTER_READ_8BIT;
                    packet->Param = rx_buffer[1];
                    packet->Data0 = rd;
                    packet->Data1 = 0x00;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_WRITE_16BIT) {
            *command ^= CMD_WRITE_16BIT;
            if (checkAddressRange(rx_buffer[1])) {
                status = spiCommand->RegisterWrite16Bit(rx_buffer[1], rx_buffer[2], rx_buffer[3]);
                if (status == ::SPICommand::Success) {
                    packet->Type = OP_REGISTER_WRITE_16BIT;
                    packet->Param = rx_buffer[1];
                    packet->Data0 = rx_buffer[2];
                    packet->Data1 = rx_buffer[3];
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_READ_16BIT) {
            *command ^= CMD_READ_16BIT;
            if (checkAddressRange(rx_buffer[1])) {
                uint8_t rd0 = 0;
                uint8_t rd1 = 0;
                status = spiCommand->RegisterRead16Bit(rx_buffer[1], &rd0, &rd1);
                if (status == ::SPICommand::Success) {
                    packet->Type = OP_REGISTER_READ_16BIT;
                    packet->Param = rx_buffer[1];
                    packet->Data0 = rd0;
                    packet->Data1 = rd1;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_START_SINGLE) {
            *command ^= CMD_START_SINGLE;
            if (validateCtrl(rx_buffer[1])) {
                uint16_t rd = 0;
                status = spiCommand->StartSingleConversion(rx_buffer[1], &rd);
                if (status == ::SPICommand::Success) {
                    packet->Type = OP_REGISTER_READ_16BIT;
                    packet->Param = rx_buffer[1];
                    packet->Data = rd;
                }
                else if (status == ::SPICommand::Abort) {
                    return;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_START_CONTINUOUS) {
            *command ^= CMD_START_CONTINUOUS;
            if (validateCtrl(rx_buffer[1])) {
                uint32_t length = (uint16_t)((rx_buffer[2] << 8) + rx_buffer[3]);

                uint32_t alignedLength = length + 1;
                int odd = alignedLength % (PACKET_SIZE / 2);
                if (odd != 0) {
                    alignedLength += (PACKET_SIZE / 2) - odd;
                }
                if (adcDataBuffer->Alloc(alignedLength)) {
                    uint16_t resultLength = 0;
                    status = spiCommand->StartContinuousConversion(rx_buffer[1], adcDataBuffer->GetBuffer(), length, &resultLength);
                    if (status == ::SPICommand::Success) {
                        packet->Type = OP_START_CONTINUOUS_CONVERSION;
                        packet->Param = rx_buffer[1];
                        packet->Data0 = (uint8_t)(length >> 8);
                        packet->Data1 = (uint8_t)(length & 0x00FF);
                    }
                    else if (status == ::SPICommand::Abort) {
                        return;
                    }
                }
                else {
                    packet->Type = OP_BUFFER_SIZE_ERROR;
                    packet->Param = OP_START_CONTINUOUS_CONVERSION;
                    packet->Data0 = (uint8_t)(length >> 8);
                    packet->Data1 = (uint8_t)(length & 0x00FF);
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[1];
                packet->Data0 = rx_buffer[2];
                packet->Data1 = 0x00;
            }
        }
        else if (*command & CMD_START_DUMP) {
            *command ^= CMD_START_DUMP;
            packet->Type = OP_START_ADC_DATA_DUMP;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
        }
        else if (*command & CMD_STOP_SINGLE) {
            *command ^= CMD_STOP_SINGLE;
            packet->Type = OP_STOP_SINGLE_CONVERSION;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
        }
        else if (*command & CMD_STOP_CONTINUOUS) {
            *command ^= CMD_STOP_CONTINUOUS;
            packet->Type = OP_STOP_CONTINUOUS_CONVERSION;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
        }
        else if (*command & CMD_STOP_DUMP) {
            *command ^= CMD_STOP_DUMP;
            packet->Type = OP_STOP_ADC_DATA_DUMP;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
        }
        else if (*command & CMD_UNKNOWN) {
            *command ^= CMD_UNKNOWN;
        }
    }
}

void Dispatcher::SetAbortRequest() {
    spiCommand->SetAbortRequest();
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

