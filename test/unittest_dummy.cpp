#include "nju9103.h"

 uint8_t rx_buffer[PACKET_SIZE];
 ReceiveDataStatus receiveDataStatus;
 Command command;

::Serial uart;
::Timer packetWatchTimer;
::Dispatcher dispatcher;
::ADCDataBuffer adcDataBuffer;
