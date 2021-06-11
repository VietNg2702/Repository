extern "C" {
#include "lis3dh.h"
#include "wiring_spi.h"
#include "wiring.h"
}

/* Private macro -------------------------------------------------------------*/
/* Self test limits converted from 10bit right-aligned to 16bit left-aligned. */
#define    MIN_ST_LIMIT_LSb     17*64
#define    MAX_ST_LIMIT_LSb    360*64

/* Private variables ---------------------------------------------------------*/
static int16_t data_raw_acceleration[3];
static float acceleration_st_mg[3];
static float acceleration_mg[3];
static uint8_t tx_buffer[1000];
static float test_val_mg[3];
static float max_st_limit_mg;
static float min_st_limit_mg;
static stmdev_ctx_t dev_ctx;
lis3dh_reg_t reg;
uint8_t i, j;

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
  lis3dh_device_id_get(&dev_ctx, &reg.byte);

  if (reg.byte != LIS3DH_ID) {
    while (1) {
      /* manage here device not found */
    }
  }
  /* Enable Block Data Update. */
  lis3dh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set full scale to 2g. */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_2g);
  /* Set device in normal mode. */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_NM_10bit);
  /* Set Output Data Rate to 1Hz. */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_50Hz);
  /* Wait stable output */
  delay(90);
}

void loop() {
  /* Check if new value available */
  do {
    lis3dh_status_get(&dev_ctx, &reg.status_reg);
  } while (!reg.status_reg.zyxda);

  /* Read dummy data and discard it */
  lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
  /* Read 5 sample and get the average vale for each axis */
  memset(acceleration_mg, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lis3dh_status_get(&dev_ctx, &reg.status_reg);
    } while (!reg.status_reg.zyxda);

    /* Read data and accumulate the mg value */
    lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);

    for (j = 0; j < 3; j++) {
      acceleration_mg[j] += lis3dh_from_fs2_nm_to_mg(
                              data_raw_acceleration[j]);
    }
  }

  /* Calculate the mg average values */
  for (i = 0; i < 3; i++) {
    acceleration_mg[i] /= 5.0f;
  }

  /* Enable Self Test positive (or negative) */
  lis3dh_self_test_set(&dev_ctx, LIS3DH_ST_POSITIVE);
  //lis3dh_self_test_set(&dev_ctx, LIS3DH_ST_NEGATIVE);
  /* Wait stable output */
  delay(90);

  /* Check if new value available */
  do {
    lis3dh_status_get(&dev_ctx, &reg.status_reg);
  } while (!reg.status_reg.zyxda);

  /* Read dummy data and discard it */
  lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
  /* Read 5 sample and get the average vale for each axis */
  memset(acceleration_st_mg, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lis3dh_status_get(&dev_ctx, &reg.status_reg);
    } while (!reg.status_reg.zyxda);

    /* Read data and accumulate the mg value */
    lis3dh_acceleration_raw_get(&dev_ctx, data_raw_acceleration);

    for (j = 0; j < 3; j++) {
      acceleration_st_mg[j] += lis3dh_from_fs2_nm_to_mg(
                                 data_raw_acceleration[j]);
    }
  }

  /* Calculate the mg average values */
  for (i = 0; i < 3; i++) {
    acceleration_st_mg[i] /= 5.0f;
  }

  /* Calculate the mg values for self test */
  for (i = 0; i < 3; i++) {
    test_val_mg[i] = fabs((acceleration_st_mg[i] - acceleration_mg[i]));
  }

  min_st_limit_mg = lis3dh_from_fs2_nm_to_mg(MIN_ST_LIMIT_LSb);
  max_st_limit_mg = lis3dh_from_fs2_nm_to_mg(MAX_ST_LIMIT_LSb);

  /* Check self test limit */
  for (i = 0; i < 3; i++) {
    if (( min_st_limit_mg < test_val_mg[i] ) && ( test_val_mg[i] < max_st_limit_mg)) 
    {
      //PASS
    }
    else 
    {
      //FAIL
    }
  }

  /* Disable Self Test */
  lis3dh_self_test_set(&dev_ctx, LIS3DH_ST_DISABLE);
  /* Disable sensor. */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_POWER_DOWN);
  while(1);
}
