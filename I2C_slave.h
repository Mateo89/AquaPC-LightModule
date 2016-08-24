#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

volatile uint8_t buffer_address;
volatile uint8_t txbuffer[20];
volatile uint8_t rxbuffer[20];

void I2C_init(uint8_t address);
void I2C_stop(void);

#endif // I2C_SLAVE_H
