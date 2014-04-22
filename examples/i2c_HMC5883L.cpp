#include "maa.h"
#include "math.h"

#define MAX_BUFFER_LENGTH 6
#define HMC5883L_I2C_ADDR 0x1E

//configuration registers
#define HMC5883L_CONF_REG_A 0x00
#define HMC5883L_CONF_REG_B 0x01

//mode register
#define HMC5883L_MODE_REG 0x02

//data register
#define HMC5883L_X_MSB_REG 0
#define HMC5883L_X_LSB_REG 1
#define HMC5883L_Z_MSB_REG 2
#define HMC5883L_Z_LSB_REG 3
#define HMC5883L_Y_MSB_REG 4
#define HMC5883L_Y_LSB_REG 5
#define DATA_REG_SIZE 6

//status register
#define HMC5883L_STATUS_REG 0x09

//ID registers
#define HMC5883L_ID_A_REG 0x0A
#define HMC5883L_ID_B_REG 0x0B
#define HMC5883L_ID_C_REG 0x0C

#define HMC5883L_CONT_MODE 0x00
#define HMC5883L_DATA_REG 0x03

//scales
#define GA_0_88_REG 0x00 << 5
#define GA_1_3_REG 0x01 << 5
#define GA_1_9_REG 0x02 << 5
#define GA_2_5_REG 0x03 << 5
#define GA_4_0_REG 0x04 << 5
#define GA_4_7_REG 0x05 << 5
#define GA_5_6_REG 0x06 << 5
#define GA_8_1_REG 0x07 << 5

//digital resolutions
#define SCALE_0_73_MG 0.73
#define SCALE_0_92_MG 0.92
#define SCALE_1_22_MG 1.22
#define SCALE_1_52_MG 1.52
#define SCALE_2_27_MG 2.27
#define SCALE_2_56_MG 2.56
#define SCALE_3_03_MG 3.03
#define SCALE_4_35_MG 4.35

int
main ()
{
  float direction = 0;
  int16_t x = 0, y = 0, z = 0;
  char rx_tx_buf[MAX_BUFFER_LENGTH];

  maa::I2CSlave i2c(26, 27);

  i2c.address(HMC5883L_I2C_ADDR);
  rx_tx_buf[0] = HMC5883L_CONF_REG_B;
  rx_tx_buf[1] = GA_1_3_REG;
  i2c.write(rx_tx_buf, 2);

  i2c.address(HMC5883L_I2C_ADDR);
  rx_tx_buf[0] = HMC5883L_MODE_REG;
  rx_tx_buf[1] = HMC5883L_CONT_MODE;
  i2c.write(rx_tx_buf, 2);

  for(;;) {
    i2c.address(HMC5883L_I2C_ADDR);
    i2c.write(HMC5883L_DATA_REG);

    i2c.address(HMC5883L_I2C_ADDR);
    i2c.read(rx_tx_buf, DATA_REG_SIZE);

    x = (rx_tx_buf[HMC5883L_X_MSB_REG] << 8 ) | rx_tx_buf[HMC5883L_X_LSB_REG] ;
    z = (rx_tx_buf[HMC5883L_Z_MSB_REG] << 8 ) | rx_tx_buf[HMC5883L_Z_LSB_REG] ;
    y = (rx_tx_buf[HMC5883L_Y_MSB_REG] << 8 ) | rx_tx_buf[HMC5883L_Y_LSB_REG] ;

    //scale and calculate direction
    direction = atan2(y * SCALE_0_92_MG, x * SCALE_0_92_MG);

    //check if the signs are reversed
    if (direction < 0)
      direction += 2 * M_PI;

    printf("Compass scaled data x : %f, y : %f, z : %f\n", x * SCALE_0_92_MG, y * SCALE_0_92_MG, z * SCALE_0_92_MG) ;
    printf("Heading : %f\n", direction * 180/M_PI) ;
  }
}
