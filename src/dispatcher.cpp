#include "spi_command.h"
#include "dispatcher.h"

static bool checkAddressRange(uint8_t address);
static bool validateCtrl(uint8_t ctrl);

::Dispatcher::Dispatcher() {
    spiCommand = new ::SPICommand();
}

::Dispatcher::~Dispatcher() {
    delete spiCommand;
}

::Dispatcher::Status ::Dispatcher::Dispatch(Command *command, uint8_t *rx_buffer, Packet *packet, ::ADCDataBuffer *adcDataBuffer) {
    memset(packet, 0, sizeof(Packet));
    packet->Header = ErrorHeader;
    packet->Type = OP_UNKNOWN_ERROR;
    Status status = Error;
    ::SPICommand::Status spiStatus;
    if (*command != 0) {
        if (*command & CMD_RESET) {
            spiStatus = spiCommand->SPIReset();
            if (spiStatus == ::SPICommand::Success) {
                packet->Header = ResponseHeader;
                packet->Type = OP_SPI_RESET;
                packet->Param = 0x00;
                packet->Data0 = 0x00;
                packet->Data1 = 0x00;
            }
            *command ^= CMD_RESET;
        }
        else if (*command & CMD_WRITE_8BIT) {
            if (checkAddressRange(rx_buffer[2])) {
                spiStatus = spiCommand->RegisterWrite8Bit(rx_buffer[2], rx_buffer[3]);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->Type = OP_REGISTER_WRITE_8BIT;
                    packet->Param = rx_buffer[2];
                    packet->Data0 = rx_buffer[3];
                    packet->Data1 = 0x00;
                    status = Success;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_WRITE_8BIT;
        }
        else if (*command & CMD_READ_8BIT) {
            if (checkAddressRange(rx_buffer[2])) {
                uint8_t rd = 0;
                spiStatus = spiCommand->RegisterRead8Bit(rx_buffer[2], &rd);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->Type = OP_REGISTER_READ_8BIT;
                    packet->Param = rx_buffer[2];
                    packet->Data0 = rd;
                    packet->Data1 = 0x00;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_READ_8BIT;
        }
        else if (*command & CMD_WRITE_16BIT) {
            if (checkAddressRange(rx_buffer[2])) {
                spiStatus = spiCommand->RegisterWrite16Bit(rx_buffer[2], rx_buffer[3], rx_buffer[4]);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->Type = OP_REGISTER_WRITE_16BIT;
                    packet->Param = rx_buffer[2];
                    packet->Data0 = rx_buffer[3];
                    packet->Data1 = rx_buffer[4];
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_WRITE_16BIT;
        }
        else if (*command & CMD_READ_16BIT) {
            if (checkAddressRange(rx_buffer[2])) {
                uint8_t rd0 = 0;
                uint8_t rd1 = 0;
                spiStatus = spiCommand->RegisterRead16Bit(rx_buffer[2], &rd0, &rd1);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->Type = OP_REGISTER_READ_16BIT;
                    packet->Param = rx_buffer[2];
                    packet->Data0 = rd0;
                    packet->Data1 = rd1;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_READ_16BIT;
        }
        else if (*command & CMD_START_SINGLE) {
            if (validateCtrl(rx_buffer[2])) {
                uint16_t rd = 0;
                spiStatus = spiCommand->StartSingleConversion(rx_buffer[2], &rd);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->Type = OP_REGISTER_READ_16BIT;
                    packet->Param = rx_buffer[2];
                    packet->Data = rd;
                }
                else if (spiStatus == ::SPICommand::Abort) {
                    status = Abort;
                }
            }
            else {
                packet->Type = OP_PARAMETER_ERROR;
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_START_SINGLE;
        }
        else if (*command & CMD_START_CONTINUOUS) {
            if (validateCtrl(rx_buffer[2])) {
                uint32_t length = (uint16_t)((rx_buffer[3] << 8) + rx_buffer[4]);

                if (adcDataBuffer->Alloc(length + 1)) {
                    uint16_t resultLength = 0;
                    spiStatus = spiCommand->StartContinuousConversion(rx_buffer[2], adcDataBuffer->GetBuffer(), length, &resultLength);
                    adcDataBuffer->SetDataLength(resultLength);
                    if (spiStatus == ::SPICommand::Success) {
                        packet->Header = ResponseHeader;
                        packet->Type = OP_START_CONTINUOUS_CONVERSION;
                        packet->Param = rx_buffer[2];
                        packet->Data0 = (uint8_t)(length >> 8);
                        packet->Data1 = (uint8_t)(length & 0x00FF);
                    }
                    else if (spiStatus == ::SPICommand::Abort) {
                        status = Abort;
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
                packet->Param = rx_buffer[2];
                packet->Data0 = rx_buffer[3];
                packet->Data1 = 0x00;
            }
            *command ^= CMD_START_CONTINUOUS;
        }
        else if (*command & CMD_START_DUMP) {
            uint16_t length = adcDataBuffer->GetDataLength() - 1;
            packet->Header = ResponseHeader;
            packet->Type = OP_START_ADC_DATA_DUMP;
            packet->Data0 = (uint8_t)(length >> 8);
            packet->Data1 = (uint8_t)(length & 0x00FF);
            *command ^= CMD_START_DUMP;
        }
        else if (*command & CMD_STOP_SINGLE) {
            packet->Header = ResponseHeader;
            packet->Type = OP_STOP_SINGLE_CONVERSION;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
            *command ^= CMD_STOP_SINGLE;
        }
        else if (*command & CMD_STOP_CONTINUOUS) {
            packet->Header = ResponseHeader;
            packet->Type = OP_STOP_CONTINUOUS_CONVERSION;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
            *command ^= CMD_STOP_CONTINUOUS;
        }
        else if (*command & CMD_STOP_DUMP) {
            packet->Header = ResponseHeader;
            packet->Type = OP_STOP_ADC_DATA_DUMP;
            packet->Param = 0x00;
            packet->Data0 = 0x00;
            packet->Data1 = 0x00;
            *command ^= CMD_STOP_DUMP;
        }
        else if (*command & CMD_UNKNOWN) {
            *command ^= CMD_UNKNOWN;
        }
    }
    return status;
}

void ::Dispatcher::SetAbortRequest() {
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

