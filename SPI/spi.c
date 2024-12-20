#include "spi.h"

esp_err_t spi_master_bus_initialize(spi_slave_t *slave)
{
    spi_bus_config_t master_bus_config = {
        .miso_io_num = slave->miso,
        .mosi_io_num = slave->mosi,
        .sclk_io_num = slave->sclk,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 16};

    int ret = spi_bus_initialize(SPI2_HOST, &master_bus_config, SPI_DMA_CH_AUTO);

    switch (ret)
    {
    case ESP_OK:
        ESP_LOGI("SPI", "Bus initialized successfully.");
        return ESP_OK;
        break;
    
    default:
        ESP_LOGE("SPI", "Failed to initialize SPI bus.");
        return ret;
        break;
    }

    return ret;
}

esp_err_t spi_slave_add(spi_slave_t *slave)
{
    gpio_config_t spi_cs_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << slave->cs),
        .pull_down_en = 0,
        .pull_up_en = 1};

    gpio_config(&spi_cs_config);

    ESP_ERROR_CHECK(gpio_set_level(slave->cs, 1));

    spi_device_interface_config_t slave_config = {
        .clock_speed_hz = slave->clock_frequency,
        .mode = 0,
        .spics_io_num = slave->cs,
        .queue_size = 7,
        .flags = 0,
        .command_bits = 0,
        .address_bits = 0, // Register address is the first byte in MOSI
        .dummy_bits = 0};

    int ret = spi_bus_add_device(SPI2_HOST, &slave_config, &slave->handle);

    switch (ret)
    {
    case ESP_OK:
        ESP_LOGI("SPI", "Slave device added successfully with chip select pin: %d.", slave->cs);
        return ESP_OK;
        break;
    
    default:
        ESP_LOGE("SPI", "Failed to add slave device to SPI bus.");
        return ret;
        break;
    }

    return ret;
}

esp_err_t spi_read_reg(spi_slave_t *slave, uint8_t register_address, uint8_t *read_buffer)
{
    uint8_t instruction_to_read_data[2] = {0x80 | register_address, 0xFF};
    spi_transaction_t trans_desc = {
        .flags = SPI_TRANS_USE_RXDATA,
        .tx_buffer = instruction_to_read_data,
        .rxlength = 8,
        .length = 8};

    gpio_set_level(slave->cs, 0);

    int ret = spi_device_polling_transmit(slave->handle, &trans_desc);
    
    gpio_set_level(slave->cs, 1);
    
    switch (ret)
    {
    case ESP_OK:
        *read_buffer = trans_desc.rx_data[0];
        ESP_LOGV("SPI", "Read operation success at register 0x%x. Read buffer: 0x%x", register_address, *read_buffer);
        return ESP_OK;
        break;
    
    default:
        ESP_LOGE("SPI", "Read operation failed at register 0x%x.", register_address);
        return ret;
        break;
    }

    return ret;
}

esp_err_t spi_write_reg(spi_slave_t *slave, uint8_t register_address, write_buffer *data_to_write)
{
    uint8_t register_data;
    spi_read_reg(slave, register_address, &register_data)
    uint8_t data = (((register_data) & (&data_to_write.bits_to_modify)) | (&data_to_write.bits_to_write));
    spi_transaction_t trans_desc = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {register_address, data},
        .length = 16};

    gpio_set_level(slave->cs, 0);

    int ret = spi_device_polling_transmit(slave->handle, &trans_desc);

    gpio_set_level(slave->cs, 1);
    
    switch (ret)
    {
    case ESP_OK:
        ESP_LOGV("SPI", "Write operation success at register 0x%x. Write buffer: 0x%x", register_address, data_to_write);
        return ESP_OK;
        break;
    
    default:
        ESP_LOGE("SPI", "Write operation failed at register 0x%x.", register_address);
        return ret;
        break;
    }

    return ret;
}
