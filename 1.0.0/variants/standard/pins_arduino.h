#ifndef ARDUINO_PINS_H__
#define ARDUINO_PINS_H__

#include "RE01_256KB.h"

typedef struct {
      __IOM uint32_t PODR       : 1;            /*!< [0..0] Port Output Data                                                   */
      __IM  uint32_t PIDR       : 1;            /*!< [1..1] Port Input Data                                                    */
      __IOM uint32_t PDR        : 1;            /*!< [2..2] Port Direction                                                     */
            uint32_t            : 1;
      __IOM uint32_t PUCR       : 1;            /*!< [4..4] Pull-Up Control                                                    */
            uint32_t            : 1;
      __IOM uint32_t NCODR      : 1;            /*!< [6..6] N-Channel Open-Drain Control                                       */
            uint32_t            : 3;
      __IOM uint32_t DSCR       : 1;            /*!< [10..10] Port Drive Capability                                            */
            uint32_t            : 3;
      __IOM uint32_t ISEL       : 1;            /*!< [14..14] IRQ Input Enable                                                 */
      __IOM uint32_t ASEL       : 1;            /*!< [15..15] Analog Input Enable                                              */
      __IOM uint32_t PMR        : 1;            /*!< [16..16] Port Mode Control                                                */
            uint32_t            : 7;
      __IOM uint32_t PSEL       : 5;            /*!< [28..24] Peripheral Function Select                                       */
            uint32_t            : 3;
    } PxxPFS_b_t;

//Flash Memory Pins
#define QSPI_QSSL_A				(uint32_t*)(&(PFS->P807PFS_b)) //6 //P807 //IRQ6_A
#define QSPI_QIO1_A				(uint32_t*)(&(PFS->P810PFS_b)) //3 //P810 //IRQ5_B
#define QSPI_QIO2_A				(uint32_t*)(&(PFS->P809PFS_b)) //4 //P809 //IRQ6_B
#define QSPI_QIO0_A				(uint32_t*)(&(PFS->P811PFS_b)) //2 //P811 //None
#define QSPI_QSPCLK_A			(uint32_t*)(&(PFS->P812PFS_b)) //1 //P812 //None
#define QSPI_QIO3_A				(uint32_t*)(&(PFS->P808PFS_b)) //5 //P808 //IRQ2_B

//Arduino Pins
#define ARDUINO_IO8				(uint32_t*)(&(PFS->P409PFS_b)) //18 //P409 //IRQ9_B
#define ARDUINO_IO9_GTIOC2A_B 	(uint32_t*)(&(PFS->P302PFS_b)) //48 //P302 //None
#define ARDUINO_SSLA1_B			(uint32_t*)(&(PFS->P015PFS_b)) //92 //P015 //IRQ5_A
#define ARDUINO_MOSIA_B			(uint32_t*)(&(PFS->P010PFS_b)) //97 //P010 //None
#define ARDUINO_MISOA_B			(uint32_t*)(&(PFS->P815PFS_b)) //98 //P815 //None
#define ARDUINO_RSPCKA_B		(uint32_t*)(&(PFS->P011PFS_b)) //96 //P011 //None
#define ARDUINO_SDA0			(uint32_t*)(&(PFS->P012PFS_b)) //95 //P012 //None
#define ARDUINO_SCL0			(uint32_t*)(&(PFS->P013PFS_b)) //94 //P013 //None

#define ARDUINO_IO0_RXD5_B		(uint32_t*)(&(PFS->P314PFS_b)) //45 //P314 //None
#define ARDUINO_IO1_TXD5_B		(uint32_t*)(&(PFS->P315PFS_b)) //44 //P315 //None
#define ARDUINO_IO2_IRQ7_B		(uint32_t*)(&(PFS->P204PFS_b)) //36 //P204 //IRQ7_B
#define ARDUINO_IO3_IRQ8_C		(uint32_t*)(&(PFS->P205PFS_b)) //35 //P205 //IRQ8_C
#define ARDUINO_IO4				(uint32_t*)(&(PFS->P602PFS_b)) //53 //P602 //None
#define ARDUINO_IO5_GTIOC5A_B	(uint32_t*)(&(PFS->P603PFS_b)) //52 //P603 //None
#define ARDUINO_IO6_GTIOC5B_B	(uint32_t*)(&(PFS->P604PFS_b)) //51 //P604 //IRQ3_C
#define ARDUINO_IO7				(uint32_t*)(&(PFS->P300PFS_b)) //50 //P300 //None

#define ARDUINO_AN000			(uint32_t*)(&(PFS->P000PFS_b)) //89 //P000 //None
#define ARDUINO_AN001			(uint32_t*)(&(PFS->P001PFS_b)) //88 //P001 //None
#define ARDUINO_AN002			(uint32_t*)(&(PFS->P002PFS_b)) //87 //P002 //None
#define ARDUINO_AN003			(uint32_t*)(&(PFS->P003PFS_b)) //86 //P003 //None
#define ARDUINO_AN004			(uint32_t*)(&(PFS->P004PFS_b)) //85 //P004 //None
#define ARDUINO_AN005			(uint32_t*)(&(PFS->P005PFS_b)) //82 //P005 //None


#endif