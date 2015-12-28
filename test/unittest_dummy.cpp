#include "nju9103.h"

Packet rxPacket;
ReceiveDataStatus receiveDataStatus;
Command command;

::Serial uart;
::Timer packetWatchTimer;
::Dispatcher dispatcher;
::ADCDataBuffer adcDataBuffer;
