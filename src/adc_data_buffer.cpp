#include "adc_data_buffer.h"

::ADCDataBuffer::ADCDataBuffer() {
    allocatedSize = 0;
    abortRequest = false;
}

::ADCDataBuffer::~ADCDataBuffer() {
}

bool ::ADCDataBuffer::Alloc(uint16_t size) {
    if (size > MAX_BUFFER_SIZE) {
        allocatedSize = 0;
        return false;
    }
    else {
        allocatedSize = size;
        memset(adcDataBuffer, 0, sizeof(uint16_t) * size);
        return true;
    }
}

void ::ADCDataBuffer::Dump(void (*pfSendPacket)(Packet *)) {
    Packet packet;
    for (int i = 0; i < allocatedSize; i = i + 2) {
        if (abortRequest) {
            abortRequest = false;
            return;
        }
        packet.ADCData0 = adcDataBuffer[i];
        packet.ADCData1 = adcDataBuffer[i + 1];
        pfSendPacket(&packet);
    }
}

uint16_t * ::ADCDataBuffer::GetBuffer() {
    return adcDataBuffer;
}

void :: ADCDataBuffer::SetAbortRequest() {
    abortRequest = true;
}
