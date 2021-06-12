extern "C" {
#include "lis3dh.h"
#include "wiring_spi.h"
}

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
static stmdev_ctx_t dev_ctx;


static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  uint8_t address = LIS3DH_READ | LIS3DH_MS | reg;
  uint8_t data[16];
  spi_transfer((ARM_DRIVER_SPI*)handle, &address, data, len+1);
  *bufp = data[1];

  return 0;
}

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
  uint8_t address = LIS3DH_WRITE | reg;

  spi_send((ARM_DRIVER_SPI*)handle, &address, 1);
  spi_send((ARM_DRIVER_SPI*)handle, (uint8_t *)bufp, len);

  return 0;
}


void setup() {
  spi_initialize((ARM_DRIVER_SPI *)ARDUINO_SPI0);
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = (void *)ARDUINO_SPI0;
  lis3dh_int1_cfg_t _6d_cfg;
  lis3dh_ctrl_reg3_t ctrl_reg3;
 
  /*  Check device ID */
  lis3dh_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LIS3DH_ID) {
    while (1) {
      /* manage here device not found */
    }
  }

  /* Set Output Data Rate to 25 Hz */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_25Hz);
  /* Set full scale to 2 g */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Set interrupt threshold to 0x12 -> 288 mg */
  lis3dh_int1_gen_threshold_set(&dev_ctx, 0x12);
  /* Enable AOI1 on int1 pin */
  lis3dh_pin_int1_config_get(&dev_ctx, &ctrl_reg3);
  ctrl_reg3.i1_ia1 = PROPERTY_ENABLE;
  lis3dh_pin_int1_config_set(&dev_ctx, &ctrl_reg3);
  /* Set no time duration. */
  lis3dh_int1_gen_duration_set(&dev_ctx, 0);
  /* Configure for movement recognition on all axis
   *
   * Follow a short definition of movement recognition
   * and direction recognition supported by device:
   *
   * AOI-6D = ‘01’ is movement recognition:
   *     An interrupt is generated when the orientation
   *     moves from an unknown zone to a known zone.
   *     The interrupt signal remains for a duration ODR.
   *
   * AOI-6D = ‘11’ is direction recognition:
   *     An interrupt is generated when the orientation is
   *     inside a known zone.
   *     The interrupt signal remains while the orientation
   *     is inside the zone. */
  lis3dh_int1_gen_conf_get(&dev_ctx, &_6d_cfg);
  _6d_cfg._6d = PROPERTY_ENABLE;
  _6d_cfg.aoi = PROPERTY_DISABLE;
  _6d_cfg.xlie = PROPERTY_ENABLE;
  _6d_cfg.xhie = PROPERTY_ENABLE;
  _6d_cfg.ylie = PROPERTY_ENABLE;
  _6d_cfg.yhie = PROPERTY_ENABLE;
  _6d_cfg.zlie = PROPERTY_ENABLE;
  _6d_cfg.zhie = PROPERTY_ENABLE;
  lis3dh_int1_gen_conf_set(&dev_ctx, &_6d_cfg);
  /* Set device in HR mode */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);

}

void loop() {
  lis3dh_reg_t all_source;
  /* Read INT pin 1 in polling mode */
  lis3dh_int1_gen_source_get(&dev_ctx, &all_source.int1_src);

  /* Check 6D Orientation */
  switch (all_source.byte & 0x3f) {
    case 0x01:
      //6D Or. position XL
      break;

    case 0x02:
      //6D Or. position XH
      break;

    case 0x04:
      //6D Or. position YL
      break;

    case 0x08:
      //6D Or. position YH
      break;

    case 0x10:
      //6D Or. position ZL
      break;

    case 0x20:
      //6D Or. position ZH
      break;

    default:
      break;
  }
}
