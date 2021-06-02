#ifndef ARDUINO_PINS_H__
#define ARDUINO_PINS_H__

// API compatibility
#include "variant.h"

//Flash Memory Pins
#define QSPI_QSSL_A				6 //P807
#define QSPI_QIO1_A				3 //P810
#define QSPI_QIO2_A				4 //P809
#define QSPI_QIO0_A				2 //P811
#define QSPI_QSPCLK_A			1 //P812
#define QSPI_QIO3_A				5 //P808

//Arduino Pins
#define ARDUINO_IO8				18 //P409
#define ARDUINO_IO9_GTIOC2A_B 	48 //P302
#define ARDUINO_SSLA1_B			92 //P015
#define ARDUINO_MOSIA_B			97 //P010
#define ARDUINO_MISOA_B			98 //P805
#define ARDUINO_RSPCKA_B		96 //P011
#define ARDUINO_SDA0			95 //P012
#define ARDUINO_SCL0			94 //P013

#define ARDUINO_IO0_RXD5_B		45 //P314
#define ARDUINO_IO1_TXD5_B		44 //P315
#define ARDUINO_IO2_IRQ7_B		36 //P204
#define ARDUINO_IO3_IRQ8_C		35 //P205
#define ARDUINO_IO4				53 //P602
#define ARDUINO_IO5_GTIOC5A_B	52 //P603
#define ARDUINO_IO6_GTIOC5B_B	51 //P604
#define ARDUINO_IO7				50 //P300

#define ARDUINO_AN000			89 //P000
#define ARDUINO_AN001			88 //P001
#define ARDUINO_AN002			87 //P002
#define ARDUINO_AN003			86 //P003
#define ARDUINO_AN004			85 //P004
#define ARDUINO_AN005			82 //P005

#endif