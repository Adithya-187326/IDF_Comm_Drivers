#include "i2c.h"

esp_err_t i2c_bus_init(i2c_bus_t *i2c_bus)
{
    int ret;
    i2c_master_bus_config_t esp_i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags = {
            .enable_internal_pullup = 1,
        },
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_NUM_0,
        .intr_priority = 0,
        .sda_io_num = i2c_bus->master.sda_io,
        .scl_io_num = i2c_bus->master.scl_io,
    };

    ret = i2c_new_master_bus(&esp_i2c_bus_config, &i2c_bus->handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

esp_err_t i2c_bus_deinit(i2c_bus_t *i2c_bus)
{
    return i2c_del_master_bus(i2c_bus->handle);
}

esp_err_t i2c_bus_reset(i2c_bus_t *i2c_bus)
{
    return i2c_master_bus_reset(i2c_bus->handle);
}

esp_err_t i2c_bus_scan(i2c_bus_t *i2c_bus, uint8_t *device_addresses)
{
    uint8_t device_count = 0;

    int ret;
    for (int addr = 0; addr < 255; addr++)
    {
        ret = i2c_master_probe(i2c_bus->handle, addr, 1000);
        switch (ret)
        {
        case ESP_OK:
            if ((device_count + 1) > sizeof(device_addresses))
            {
                ESP_LOGW("I2C", "Too many devices found on bus. Increase device_addresses array size.");
            }
            else
            {
                device_addresses[device_count] = addr;
            }
            ESP_LOGI("I2C", "Bus device found with address: '0x%x'", addr);
            device_count++;
            break;

        case ESP_ERR_NOT_FOUND:
            break;

        case ESP_ERR_TIMEOUT:
            ESP_LOGD("I2C", "Timeout when probing bus device with address: '0x%x'. Check pull-up resistors & device datasheet for NACK/ACK", addr);
            break;
        }
    }

    ESP_LOGD("I2C", "Found %d devices on bus", device_count);

    return ESP_OK;
}

esp_err_t i2c_slave_add(i2c_bus_t *i2c_bus)
{
    int ret;
    i2c_device_config_t i2c_slave_config = {
        .dev_addr_length = i2c_bus->slave.address_length,
        .device_address = i2c_bus->slave.address,
        .flags.disable_ack_check = 0,
        .scl_speed_hz = i2c_bus->slave.clock_frequency,
        .scl_wait_us = 0,
    };

    ret = i2c_master_bus_add_device(i2c_bus->handle, &i2c_slave_config, &i2c_bus->slave.handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

esp_err_t i2c_slave_remove(i2c_bus_t *i2c_bus)
{
    return i2c_master_bus_rm_device(i2c_bus->slave.handle);
}

esp_err_t i2c_write_to_device(i2c_bus_t *i2c_bus, uint8_t *write_buffer)
{
    return i2c_master_transmit(i2c_bus->slave.handle, write_buffer, sizeof(write_buffer), 1000);
}

esp_err_t i2c_read_from_device(i2c_bus_t *i2c_bus, uint8_t *read_buffer)
{
    return i2c_master_receive(i2c_bus->slave.handle, read_buffer, sizeof(read_buffer), 1000);
}

esp_err_t i2c_write_read_from_device(i2c_bus_t *i2c_bus, uint8_t *write_buffer, uint8_t *read_buffer)
{
    return i2c_master_transmit_receive(i2c_bus->slave.handle, write_buffer, sizeof(write_buffer), read_buffer, sizeof(read_buffer), 1000);
}
