#ifndef __NJU9103_H__
#define __NJU9103_H__

#include "types.h"
#include "serial_wrapper.h"
#include "spi_command.h"
#include "timer_wrapper.h"
#include "adc_data_buffer.h"
#include "dispatcher.h"

/**
 * @brief executed once
 */
extern void setup();

/**
 * @brief executed consecutively
 */
extern void loop();

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
NJRC_STATIC void decodeCommand(Packet *args, Command *cmd);

/**
 * @brief dump ADC data buffer
 */
NJRC_STATIC void dumpADCData();

#endif
