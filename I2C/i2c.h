#ifndef _I2C_H_
#define _I2C_H_

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t address_length;    // Length of the I2C device address
    uint8_t address;           // I2C device address
    int clock_frequency;       // I2C clock frequency
    i2c_master_dev_handle_t handle; // Handle to the slave device
} i2c_slave_device_t;

typedef struct
{
    gpio_num_t sda_io;         // GPIO number for Master SDA
    gpio_num_t scl_io;         // GPIO number for Master SCL
} i2c_master_device_t;

typedef struct
{
    i2c_master_device_t master; // Master device configuration
    i2c_slave_device_t slave;   // Slave device configuration
    i2c_master_bus_handle_t handle; // Handle to the I2C bus
} i2c_bus_t;

/**
 * @brief Initialize the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure to initialize.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_bus_init(i2c_bus_t *i2c_bus);

/**
 * @brief Deinitialize the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure to deinitialize.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_bus_deinit(i2c_bus_t *i2c_bus);

/**
 * @brief Reset the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure to reset.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_bus_reset(i2c_bus_t *i2c_bus);

/**
 * @brief Scan for I2C devices on the bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @param device_addresses Array to store found device addresses.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_bus_scan(i2c_bus_t *i2c_bus, uint8_t *device_addresses);

/**
 * @brief Add a slave device to the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_slave_add(i2c_bus_t *i2c_bus);

/**
 * @brief Remove a slave device from the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_slave_remove(i2c_bus_t *i2c_bus);

/**
 * @brief Write data to a slave device on the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @param write_buffer Data to write to the slave device.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_write_to_device(i2c_bus_t *i2c_bus, uint8_t *write_buffer);

/**
 * @brief Read data from a slave device on the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @param read_buffer Buffer to store the read data.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_read_from_device(i2c_bus_t *i2c_bus, uint8_t *read_buffer);

/**
 * @brief Read data from a specific register of a slave device on the I2C bus.
 *
 * @param i2c_bus Pointer to the I2C bus structure.
 * @param register_address Register address to read from.
 * @param read_buffer Buffer to store the read data.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t i2c_write_read_from_device(i2c_bus_t *i2c_bus, uint8_t *write_buffer, uint8_t *read_buffer);

#ifdef __cplusplus
}
#endif

#endif
