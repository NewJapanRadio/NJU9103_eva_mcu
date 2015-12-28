#include <string.h>

#include "adc_data_buffer.h"

::ADCDataBuffer::ADCDataBuffer() {
    dataLength = 0;
    allocatedSize = 0;
    abortRequest = false;
}

::ADCDataBuffer::~ADCDataBuffer() {
}

bool ::ADCDataBuffer::Alloc(uint16_t size) {
    dataLength = 0;

    uint32_t alignedSize = size;
    int odd = alignedSize % (PACKET_SIZE / 2);
    if (odd != 0) {
        alignedSize += (PACKET_SIZE / 2) - odd;
    }

    if (alignedSize > MAX_BUFFER_SIZE) {
        allocatedSize = 0;
        return false;
    }
    else {
        allocatedSize = alignedSize;
        memset(adcDataBuffer, 0, sizeof(uint16_t) * allocatedSize);
        return true;
    }
}

void ::ADCDataBuffer::Dump(void (*pfSendPacket)(Packet *)) {
    Packet packet;
    packet.Header = DataHeader;
    for (int i = 0; i < allocatedSize; i = i + 4) {
        if (abortRequest) {
            abortRequest = false;
            packet.Header = ResponseHeader;
            packet.OpCode = OP_STOP_ADC_DATA_DUMP;
            packet.Byte1 = 0x00;
            packet.Byte2 = 0x00;
            packet.Byte3 = 0x00;
            packet.Byte4 = 0x00;
            packet.Byte5 = 0x00;
            packet.Byte6 = 0x00;
            packet.Byte7 = 0x00;
            pfSendPacket(&packet);
            return;
        }
        packet.Byte0 = (uint8_t)(adcDataBuffer[i]   >> 8);
        packet.Byte1 = (uint8_t)(adcDataBuffer[i]   & 0x00FF);
        packet.Byte2 = (uint8_t)(adcDataBuffer[i+1] >> 8);
        packet.Byte3 = (uint8_t)(adcDataBuffer[i+1] & 0x00FF);
        packet.Byte4 = (uint8_t)(adcDataBuffer[i+2] >> 8);
        packet.Byte5 = (uint8_t)(adcDataBuffer[i+2] & 0x00FF);
        packet.Byte6 = (uint8_t)(adcDataBuffer[i+3] >> 8);
        packet.Byte7 = (uint8_t)(adcDataBuffer[i+3] & 0x00FF);
        pfSendPacket(&packet);
    }
}

uint16_t * ::ADCDataBuffer::GetBuffer() {
    return adcDataBuffer;
}

uint16_t ::ADCDataBuffer::GetDataLength() {
    return dataLength;
}

void ::ADCDataBuffer::SetDataLength(uint16_t length) {
     dataLength = length;
}

void :: ADCDataBuffer::SetAbortRequest() {
    abortRequest = true;
}
