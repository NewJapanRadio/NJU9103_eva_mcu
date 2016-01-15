#include <string.h>

#include "adc_data_buffer.h"

NewJapanRadio::ADCDataBuffer::ADCDataBuffer() {
    dataLength = 0;
    allocatedSize = 0;
}

NewJapanRadio::ADCDataBuffer::~ADCDataBuffer() {
}

bool NewJapanRadio::ADCDataBuffer::Alloc(uint16_t size) {
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

uint16_t * NewJapanRadio::ADCDataBuffer::GetBuffer() {
    return adcDataBuffer;
}

uint16_t NewJapanRadio::ADCDataBuffer::GetAllocatedSize() {
    return allocatedSize;
}

uint16_t NewJapanRadio::ADCDataBuffer::GetDataLength() {
    return dataLength;
}

void NewJapanRadio::ADCDataBuffer::SetDataLength(uint16_t length) {
     dataLength = length;
}
