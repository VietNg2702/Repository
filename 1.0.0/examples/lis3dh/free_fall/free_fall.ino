extern "C" {
#include "lis3dh.h"
#include "wiring_spi.h"
}

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
static stmdev_ctx_t dev_ctx;
static lis3dh_ctrl_reg3_t ctrl_reg3;
static lis3dh_int1_cfg_t int1_cfg;

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
 
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{

}

void setup() {

  spi0_initialize();
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;

  /* Check device ID */
  lis3dh_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LIS3DH_ID) {
    while (1) {
      /* manage here device not found */
    }
  }

  /* Set full scale to 2 g */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Enable AOI1 interrupt on INT pin 1 */
  lis3dh_pin_int1_config_get(&dev_ctx, &ctrl_reg3);
  ctrl_reg3.i1_ia1 = PROPERTY_ENABLE;
  lis3dh_pin_int1_config_set(&dev_ctx, &ctrl_reg3);
  /* Enable Interrupt 1 pin latched */
  lis3dh_int1_pin_notification_mode_set(&dev_ctx, LIS3DH_INT1_LATCHED);
  /* Set threshold to 16h -> 350mg
   * Set Duration to 03h -> minimum event duration ~= 30 msec
   * If acceleration an all axis is below the threshold for more
   * than 30 ms than device is falling down */
  lis3dh_int1_gen_threshold_set(&dev_ctx, 0x16);
  lis3dh_int1_gen_duration_set(&dev_ctx, 0x02);
  /* Configure free-fall recognition
   *
   * Set interrupt condition (AND) for x, y, z acc. data below
   * threshold */
  lis3dh_int1_gen_conf_get(&dev_ctx, &int1_cfg);
  int1_cfg.aoi = PROPERTY_ENABLE;
  int1_cfg.zlie = PROPERTY_ENABLE;
  int1_cfg.ylie = PROPERTY_ENABLE;
  int1_cfg.xlie = PROPERTY_ENABLE;
  lis3dh_int1_gen_conf_set(&dev_ctx, &int1_cfg);
  /* Set device in HR mode */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);
  /* Set Output Data Rate to 100 Hz */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_100Hz);

}

void loop() {
  /* Check INT 1 pin or read source register */
    lis3dh_reg_t all_source;
    lis3dh_int1_gen_source_get(&dev_ctx, &all_source.int1_src);

    if (!(all_source.byte & 0x3f)) {
      // freefall detected
    }
}
