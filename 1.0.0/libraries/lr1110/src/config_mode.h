/**
 * @file      config_mode.h
 *
 */

#ifndef _CONFIG_MODE_H
#define _CONFIG_MODE_H

// Selector 1 .. TX Mode  0 .. RX Mode
#define TRACKER_TX (0)
//#if (TRACKER_TX == 1)

// Selector 1 ..  SEND GNSS data to SERVER  0 .. Dont Send
#define SEND_TO_SERVER (0)

#define INHIBIT_SSTBY (0)

#define DEBUG_LED (1)

#define USE_MAINOSC (1)

#define USE_ALMANAC_UPDATE (1)

#define FORCE_ALMANAC_UPDATE (0) // Force update regardless of almanac date

#define TX_BOARD_VERSION     (2)
// Version1 .. LR1110 and accelerator always ON
// Version2 .. LR1110 ON/OFF and accelerator always ON

#define OPERATION_MINIMUM_VOLTAGE  (2.3)

#define  GET_RSSI_SNR   (1)


#if (DEBUG_LED == 1)
#endif

#define RTC_ADJUST_INTERVAL  (3600*24)  // RTC must adjust within 24 hours
#define ACK_RECEIVE_INTERVAL (3*1000) // 3sec
#define ACK_WAIT_INTERVAL  (10*1000) // 10sec
#define GNSS_WAIT_INTERVAL (30*1000) // 30sec

#define IRQ_INT_MINUMIM_INTERVAL (600)// 600sec

#endif
