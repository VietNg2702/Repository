extern "C" {
#include "lis3dh.h"
#include "wiring_spi.h"
}

/* Private variables ---------------------------------------------------------*/
static int16_t data_raw_acceleration[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
static float temperature_degC;
static stmdev_ctx_t dev_ctx;

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
  /* Enable Block Data Update. */
  lis3dh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set Output Data Rate to 1Hz. */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_1Hz);
  /* Set full scale to 2g. */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Enable temperature sensor. */
  lis3dh_aux_adc_set(&dev_ctx, LIS3DH_AUX_ON_TEMPERATURE);
  /* Set device in continuous mode with 12 bit resol. */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);

}

void loop() {
  /* Read samples in polling mode (no int) */
  lis3dh_reg_t reg;
  /* Read output only if new value available */
  lis3dh_xl_data_ready_get(&dev_ctx, &reg.byte);

  if (reg.byte) {
    /* Read accelerometer data */
    memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
    lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
    acceleration_mg[0] =
      lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[0]);
    acceleration_mg[1] =
      lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[1]);
    acceleration_mg[2] =
      lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[2]);
  }

  lis3dh_temp_data_ready_get(&dev_ctx, &reg.byte);

  if (reg.byte) {
    /* Read temperature data */
    memset(&data_raw_temperature, 0x00, sizeof(int16_t));
    lis3dh_temperature_raw_get(&dev_ctx, &data_raw_temperature);
    temperature_degC = lis3dh_from_lsb_hr_to_celsius(data_raw_temperature);
  }
}
