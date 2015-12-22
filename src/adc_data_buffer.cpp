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
            return;
        }
        packet.ADCData0 = adcDataBuffer[i];
        packet.ADCData1 = adcDataBuffer[i + 1];
        packet.ADCData2 = adcDataBuffer[i + 2];
        packet.ADCData3 = adcDataBuffer[i + 3];
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
