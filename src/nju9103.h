#ifndef __NJU9103_H__
#define __NJU9103_H__

#include "nju9103_types.h"
#include "serial_wrapper.h"
#include "spi_command.h"
#include "timer_wrapper.h"
#include "adc_data_buffer.h"
#include "dispatcher.h"

NJRC_STATIC void setup();
NJRC_STATIC void loop();

NJRC_STATIC void isrRx();
NJRC_STATIC void isrPacketWatch();
NJRC_STATIC uint8_t calculateChkSum(Packet *packet);
NJRC_STATIC uint8_t calculateChkSum(uint8_t *data);
NJRC_STATIC void sendPacket(Packet *packet);
NJRC_STATIC void decodeCommand(uint8_t header, uint8_t opcode, Command *cmd);

#endif
