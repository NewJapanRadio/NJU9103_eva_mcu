#include "nju9103.h"

Packet rxPacket;
ReceiveDataStatus receiveDataStatus;
Command command;

::Serial uart;
::Timer packetWatchTimer;
::Dispatcher dispatcher;
::ADCDataBuffer adcDataBuffer;

void (*fpIsrRx)(void);
void _attach(void (*fp)(void), ::Serial::IrqType type) {
    fpIsrRx = fp;
}
