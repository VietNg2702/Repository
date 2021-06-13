#include <Arduino.h>
#include <lis3dh.h>

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
stmdev_ctx_t dev_ctx;
lis3dh_int1_cfg_t int1_cfg;
lis3dh_ctrl_reg3_t ctrl_reg3;
uint8_t dummy;

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
  /* High-pass filter enabled on interrupt activity 1 */
  lis3dh_high_pass_int_conf_set(&dev_ctx, LIS3DH_ON_INT1_GEN);
  /* Enable HP filter for wake-up event detection
   *
   * Use this setting to remove gravity on data output */
  lis3dh_high_pass_on_outputs_set(&dev_ctx, PROPERTY_ENABLE);
  /* Enable AOI1 on int1 pin */
  lis3dh_pin_int1_config_get(&dev_ctx, &ctrl_reg3);
  ctrl_reg3.i1_ia1 = PROPERTY_ENABLE;
  lis3dh_pin_int1_config_set(&dev_ctx, &ctrl_reg3);
  /* Interrupt 1 pin latched */
  lis3dh_int1_pin_notification_mode_set(&dev_ctx,
                                        LIS3DH_INT1_LATCHED);
  /* Set full scale to 2 g */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Set interrupt threshold to 0x10 -> 250 mg */
  lis3dh_int1_gen_threshold_set(&dev_ctx, 0x10);
  /* Set no time duration */
  lis3dh_int1_gen_duration_set(&dev_ctx, 0);
  /* Dummy read to force the HP filter to current acceleration value. */
  lis3dh_filter_reference_get(&dev_ctx, &dummy);
  /* Configure wake-up interrupt event on all axis */
  lis3dh_int1_gen_conf_get(&dev_ctx, &int1_cfg);
  int1_cfg.zhie = PROPERTY_ENABLE;
  int1_cfg.yhie = PROPERTY_ENABLE;
  int1_cfg.xhie = PROPERTY_ENABLE;
  int1_cfg.aoi = PROPERTY_DISABLE;
  lis3dh_int1_gen_conf_set(&dev_ctx, &int1_cfg);
  /* Set device in HR mode */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);
  /* Set Output Data Rate to 100 Hz */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_100Hz);

}

void loop() {
  lis3dh_int1_src_t src;
  /* Read INT pin 1 in polling mode
    * or read src status register
    */
  lis3dh_int1_gen_source_get(&dev_ctx, &src);

  if (src.xh || src.yh || src.zh) {
    lis3dh_int1_gen_source_get(&dev_ctx, &src);
    //"wake-up detected 
  }
}
