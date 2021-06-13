#include <Arduino.h>
#include <lis3dh.h>

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
static stmdev_ctx_t dev_ctx;
lis3dh_ctrl_reg3_t ctrl_reg3;
lis3dh_click_cfg_t click_cfg;

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

  /* Check device ID */
  lis3dh_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LIS3DH_ID) {
    while (1) {
      /* manage here device not found */
    }
  }

  /* Set full scale to 2 g. */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Set click threshold to 12h -> 0.281 g
   * 1 LSB = full scale/128
   *
   * Set TIME_LIMIT to 20h -> 80 ms
   * Set TIME_LATENCY to 20h -> 80 ms
   * Set TIME_WINDOW to 30h -> 120 ms
   * 1 LSB = 1/ODR */
  lis3dh_tap_threshold_set(&dev_ctx, 0x12);
  lis3dh_shock_dur_set(&dev_ctx, 0x20);
  lis3dh_quiet_dur_set(&dev_ctx, 0x20);
  lis3dh_double_tap_timeout_set(&dev_ctx, 0x30);
  /* Enable Click interrupt on INT pin 1 */
  lis3dh_pin_int1_config_get(&dev_ctx, &ctrl_reg3);
  ctrl_reg3.i1_click = PROPERTY_ENABLE;
  lis3dh_pin_int1_config_set(&dev_ctx, &ctrl_reg3);
  lis3dh_int1_gen_duration_set(&dev_ctx, 0);
  /* Enable double click on all axis */
  lis3dh_tap_conf_get(&dev_ctx, &click_cfg);
  click_cfg.xd = PROPERTY_ENABLE;
  click_cfg.yd = PROPERTY_ENABLE;
  click_cfg.zd = PROPERTY_ENABLE;
  lis3dh_tap_conf_set(&dev_ctx, &click_cfg);
  /* Set device in HR mode. */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);
  /* Set Output Data Rate.
   * The recommended accelerometer ODR for single and
   * double-click recognition is 400 Hz or higher. */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_400Hz);

}

void loop() {
  lis3dh_click_src_t src;
  /* Check double tap event */
  lis3dh_tap_source_get(&dev_ctx, &src);

  if (src.dclick) {
    // "d-click detected
  }

}
