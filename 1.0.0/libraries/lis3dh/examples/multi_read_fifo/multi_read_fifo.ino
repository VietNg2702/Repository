#include <Arduino.h>
#include <lis3dh.h>

/* Private variables ---------------------------------------------------------*/
static int16_t data_raw_acceleration[3];
static float acceleration_mg[3];
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
  
  /* Check device ID */
  lis3dh_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LIS3DH_ID) {
    while (1) {
      /* manage here device not found */
    }
  }

  /*  Enable Block Data Update */
  lis3dh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set Output Data Rate to 25 hz */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_25Hz);
  /* Set full scale to 2 g */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Set operating mode to high resolution */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);
  /* Set FIFO watermark to 25 samples */
  lis3dh_fifo_watermark_set(&dev_ctx, 25);
  /* Set FIFO mode to Stream mode: Accumulate samples and
   * override old data */
  lis3dh_fifo_mode_set(&dev_ctx, LIS3DH_DYNAMIC_STREAM_MODE);
  /* Enable FIFO */
  lis3dh_fifo_set(&dev_ctx, PROPERTY_ENABLE);

}

void loop() {
  uint8_t flags;
  uint8_t num = 0;
  /* Check if FIFO level over threshold */
  lis3dh_fifo_fth_flag_get(&dev_ctx, &flags);

  if (flags) {
    /* Read number of sample in FIFO */
    lis3dh_fifo_data_level_get(&dev_ctx, &num);

    while (num-- > 0) {
      /* Read XL samples */
      lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
      acceleration_mg[0] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[0]);
      acceleration_mg[1] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[1]);
      acceleration_mg[2] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[2]);
    }
  }
}
