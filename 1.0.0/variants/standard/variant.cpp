
// struct {
//       __IOM uint32_t PODR       : 1;            /*!< [0..0] Port Output Data                                                   */
//       __IM  uint32_t PIDR       : 1;            /*!< [1..1] Port Input Data                                                    */
//       __IOM uint32_t PDR        : 1;            /*!< [2..2] Port Direction                                                     */
//             uint32_t            : 1;
//       __IOM uint32_t PUCR       : 1;            /*!< [4..4] Pull-Up Control                                                    */
//             uint32_t            : 1;
//       __IOM uint32_t NCODR      : 1;            /*!< [6..6] N-Channel Open-Drain Control                                       */
//             uint32_t            : 3;
//       __IOM uint32_t DSCR       : 1;            /*!< [10..10] Port Drive Capability                                            */
//             uint32_t            : 3;
//       __IOM uint32_t ISEL       : 1;            /*!< [14..14] IRQ Input Enable                                                 */
//       __IOM uint32_t ASEL       : 1;            /*!< [15..15] Analog Input Enable                                              */
//       __IOM uint32_t PMR        : 1;            /*!< [16..16] Port Mode Control                                                */
//             uint32_t            : 7;
//       __IOM uint32_t PSEL       : 5;            /*!< [28..24] Peripheral Function Select                                       */
//             uint32_t            : 3;
//     } P409PFS_b;
#include "variant.h"
UARTClass Serial(6,8);