#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

typedef struct 
{
    spi_device_handle_t handle;
    gpio_num_t mosi;
    gpio_num_t miso;
    gpio_num_t sclk;
    gpio_num_t cs;
    int clock_frequency;
} spi_slave_t;

typedef struct
{
    uint8_t bits_to_modify;
    uint8_t bits_to_write;
} write_buffer;


esp_err_t spi_master_bus_initialize(spi_slave_t *slave);

esp_err_t spi_slave_add(spi_slave_t *slave);

esp_err_t spi_read_reg(spi_slave_t *slave, uint8_t register_address, uint8_t *read_buffer);

esp_err_t spi_write_reg(spi_slave_t *slave, uint8_t register_address, write_buffer *data_to_write);

#ifdef __cplusplus
}
#endif

#endif
