#ifndef __NJU9103_H__
#define __NJU9103_H__

#include "nju9103_types.h"
#include "serial_wrapper.h"
#include "spi_command.h"
#include "timer_wrapper.h"
#include "adc_data_buffer.h"
#include "dispatcher.h"

/**
 * @brief executed once
 */
NJRC_STATIC void setup();

/**
 * @brief executed consecutively
 */
NJRC_STATIC void loop();

/**
 * @brief UART Rx event handler
 */
NJRC_STATIC void isrRx();

/**
 * @brief watch received packet
 *
 * this function called from timer event
 */
NJRC_STATIC void isrPacketWatch();

/**
 * @brief calculate check sum
 */
NJRC_STATIC uint8_t calculateChkSum(Packet *packet);

/**
 * @brief calculate check sum
 */
NJRC_STATIC uint8_t calculateChkSum(uint8_t *data);

/**
 * @brief send pacekt via UART Tx
 */
NJRC_STATIC void sendPacket(Packet *packet);

/**
 * @brief decode OP Code to Command
 */
NJRC_STATIC void decodeCommand(uint8_t header, uint8_t opcode, Command *cmd);

#endif
