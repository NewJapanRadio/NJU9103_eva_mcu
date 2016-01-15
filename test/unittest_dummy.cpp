#include "nju9103.h"

Packet rxPacket;
ReceiveDataStatus receiveDataStatus;
Command command;

NewJapanRadio::Serial uart;
NewJapanRadio::Timer packetWatchTimer;
NewJapanRadio::Dispatcher dispatcher;
NewJapanRadio::ADCDataBuffer adcDataBuffer;

void (*fpIsrRx)(void);
void _attach(void (*fp)(void), NewJapanRadio::Serial::IrqType type) {
    fpIsrRx = fp;
}
