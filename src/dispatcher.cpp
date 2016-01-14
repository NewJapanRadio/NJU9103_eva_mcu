#include <string.h>

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

::Dispatcher::Status ::Dispatcher::Dispatch(Command *command, Packet *args, Packet *packet, ::ADCDataBuffer *adcDataBuffer) {
    memset(packet, 0, sizeof(Packet));
    packet->Header = ErrorHeader;
    packet->OpCode = OP_UNKNOWN_ERROR;
    Status status = Error;
    ::SPICommand::Status spiStatus;
    if (*command != 0) {
        if (*command & CMD_RESET) {
            spiStatus = spiCommand->SPIReset();
            if (spiStatus == ::SPICommand::Success) {
                packet->Header = ResponseHeader;
                packet->OpCode = OP_SPI_RESET;
            }
            *command ^= CMD_RESET;
        }
        else if (*command & CMD_WRITE_8BIT) {
            if (checkAddressRange(args->Param)) {
                spiStatus = spiCommand->RegisterWrite8Bit(args->Param, args->Data0);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->OpCode = OP_REGISTER_WRITE_8BIT;
                    packet->Param = args->Param;
                    packet->Byte2 = args->Data0;
                    status = Success;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_REGISTER_WRITE_8BIT;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_WRITE_8BIT;
        }
        else if (*command & CMD_READ_8BIT) {
            if (checkAddressRange(args->Param)) {
                uint8_t rd = 0;
                spiStatus = spiCommand->RegisterRead8Bit(args->Param, &rd);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->OpCode = OP_REGISTER_READ_8BIT;
                    packet->Param = args->Param;
                    packet->Data0 = rd;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_REGISTER_READ_8BIT;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_READ_8BIT;
        }
        else if (*command & CMD_WRITE_16BIT) {
            if (checkAddressRange(args->Param)) {
                spiStatus = spiCommand->RegisterWrite16Bit(args->Param, args->Data0, args->Data1);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->OpCode = OP_REGISTER_WRITE_16BIT;
                    packet->Param = args->Param;
                    packet->Data0 = args->Data0;
                    packet->Data1 = args->Data1;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_REGISTER_WRITE_16BIT;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_WRITE_16BIT;
        }
        else if (*command & CMD_READ_16BIT) {
            if (checkAddressRange(args->Param)) {
                uint8_t rd0 = 0;
                uint8_t rd1 = 0;
                spiStatus = spiCommand->RegisterRead16Bit(args->Param, &rd0, &rd1);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->OpCode = OP_REGISTER_READ_16BIT;
                    packet->Param = args->Param;
                    packet->Data0 = rd0;
                    packet->Data1 = rd1;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_REGISTER_READ_16BIT;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_READ_16BIT;
        }
        else if (*command & CMD_START_SINGLE) {
            if (validateCtrl(args->Param)) {
                uint16_t rd = 0;
                spiStatus = spiCommand->StartSingleConversion(args->Param, &rd);
                if (spiStatus == ::SPICommand::Success) {
                    packet->Header = ResponseHeader;
                    packet->OpCode = OP_START_SINGLE_CONVERSION;
                    packet->Param = args->Param;
                    packet->Data0 = (uint8_t)(rd >> 8);
                    packet->Data1 = (uint8_t)(rd & 0x00FF);
                }
                else if (spiStatus == ::SPICommand::Abort) {
                    status = Abort;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_START_SINGLE_CONVERSION;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_START_SINGLE;
        }
        else if (*command & CMD_START_CONTINUOUS) {
            if (validateCtrl(args->Param)) {
                uint16_t length = (uint16_t)((args->Byte2 << 8) + args->Byte3);

                if (adcDataBuffer->Alloc(length + 1)) {
                    uint16_t resultLength = 0;
                    spiStatus = spiCommand->StartContinuousConversion(args->Param, adcDataBuffer->GetBuffer(), length, &resultLength);
                    adcDataBuffer->SetDataLength(resultLength);
                    if (spiStatus == ::SPICommand::Success) {
                        packet->Header = ResponseHeader;
                        packet->OpCode = OP_START_CONTINUOUS_CONVERSION;
                        packet->Param = args->Param;
                        packet->Data0 = args->Byte2;
                        packet->Data1 = args->Byte3;
                    }
                    else if (spiStatus == ::SPICommand::Abort) {
                        status = Abort;
                    }
                }
                else {
                    packet->OpCode = OP_BUFFER_SIZE_ERROR;
                    packet->Byte1 = OP_START_CONTINUOUS_CONVERSION;
                    packet->Byte2 = args->Byte2;
                    packet->Byte3 = args->Byte3;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_START_CONTINUOUS_CONVERSION;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_START_CONTINUOUS;
        }
        else if (*command & CMD_START_INTERMITTENT) {
            if (validateCtrl(args->Param)) {
                uint16_t length = (uint16_t)((args->Byte2 << 8) + args->Byte3);
                uint32_t interval = (uint32_t)((args->Byte4 << 24) + (args->Byte5 << 16) + (args->Byte6 << 8) + args->Byte7);

                if (adcDataBuffer->Alloc(length + 1)) {
                    uint16_t resultLength = 0;
                    spiStatus = spiCommand->StartIntermittentConversion(args->Param, adcDataBuffer->GetBuffer(), interval, length, &resultLength);
                    adcDataBuffer->SetDataLength(resultLength);
                    if (spiStatus == ::SPICommand::Success) {
                        packet->Header = ResponseHeader;
                        packet->OpCode = OP_START_INTERMITTENT_CONVERSION;
                        packet->Param = args->Param;
                        packet->Data0 = args->Byte2;
                        packet->Data1 = args->Byte3;
                        packet->Byte4 = args->Byte4;
                        packet->Byte5 = args->Byte5;
                        packet->Byte6 = args->Byte6;
                        packet->Byte7 = args->Byte7;
                    }
                    else if (spiStatus == ::SPICommand::Abort) {
                        status = Abort;
                    }
                }
                else {
                    packet->OpCode = OP_BUFFER_SIZE_ERROR;
                    packet->Byte1 = OP_START_INTERMITTENT_CONVERSION;
                    packet->Byte2 = args->Byte2;
                    packet->Byte3 = args->Byte3;
                }
            }
            else {
                packet->OpCode = OP_PARAMETER_ERROR;
                packet->Byte1 = OP_START_INTERMITTENT_CONVERSION;
                packet->Byte2 = args->Param;
            }
            *command ^= CMD_START_INTERMITTENT;
        }
        else if (*command & CMD_START_DUMP) {
            uint16_t length = adcDataBuffer->GetDataLength() - 1;
            packet->Header = ResponseHeader;
            packet->OpCode = OP_START_ADC_DATA_DUMP;
            packet->Data0 = (uint8_t)(length >> 8);
            packet->Data1 = (uint8_t)(length & 0x00FF);
        }
        else if (*command & CMD_STOP_SINGLE) {
            packet->Header = ResponseHeader;
            packet->OpCode = OP_STOP_SINGLE_CONVERSION;
            *command ^= CMD_STOP_SINGLE;
        }
        else if (*command & CMD_STOP_CONTINUOUS) {
            packet->Header = ResponseHeader;
            packet->OpCode = OP_STOP_CONTINUOUS_CONVERSION;
            *command ^= CMD_STOP_CONTINUOUS;
        }
        else if (*command & CMD_STOP_INTERMITTENT) {
            packet->Header = ResponseHeader;
            packet->OpCode = OP_STOP_INTERMITTENT_CONVERSION;
            *command ^= CMD_STOP_INTERMITTENT;
        }
        else if (*command & CMD_STOP_DUMP) {
            packet->Header = ResponseHeader;
            packet->OpCode = OP_STOP_ADC_DATA_DUMP;
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

