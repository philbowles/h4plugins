#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

#include "u8g2_esp32_hal.h"
#include "icons.h"

#define PIN_SDA 5
#define PIN_SCL 4

/* Para o segundo argumento, as seguintes constantes estão disponíveis:

Rotação / Espelho Descrição
U8G2_R0 Sem rotação, paisagem
U8G2_R1 Rotação de 90 graus no sentido horário
U8G2_R2 Rotação de 180 graus no sentido horário
U8G2_R3 Rotação de 270 graus no sentido horário
U8G2_MIRROR Nenhuma rotação, paisagem, conteúdo da tela é espelhado (v2.6.x) */

void app_main() {

// initialize the u8g2 hal na versão I2C
u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
u8g2_esp32_hal.sda = PIN_SDA;
u8g2_esp32_hal.scl = PIN_SCL;
u8g2_esp32_hal_init(u8g2_esp32_hal);

// initialize the u8g2 library
u8g2_t u8g2;
u8g2_Setup_ssd1306_128x64_noname_f( &u8g2, U8G2_R0, u8g2_esp32_i2c_byte_cb, u8g2_esp32_gpio_and_delay_cb);

// set the display address
u8x8_SetI2CAddress(&u8g2.u8x8, 0x3c);

// initialize the display
u8g2_InitDisplay(&u8g2);

// wake up the display
u8g2_SetPowerSave(&u8g2, 0);

// loop
while(1) {

// draw the hourglass animation, full-half-empty
u8g2_ClearBuffer(&u8g2);
u8g2_DrawXBM(&u8g2, 34, 2, 60, 60, hourglass_full);
u8g2_SendBuffer(&u8g2);
vTaskDelay(1000 / portTICK_RATE_MS);

u8g2_ClearBuffer(&u8g2);
u8g2_DrawXBM(&u8g2, 34, 2, 60, 60, hourglass_half);
u8g2_SendBuffer(&u8g2);
vTaskDelay(1000 / portTICK_RATE_MS);

u8g2_ClearBuffer(&u8g2);
u8g2_DrawXBM(&u8g2, 34, 2, 60, 60, hourglass_empty);
u8g2_SendBuffer(&u8g2);
vTaskDelay(1000 / portTICK_RATE_MS);

// set font and write hello world
u8g2_ClearBuffer(&u8g2);
u8g2_SetFont(&u8g2, u8g2_font_timR14_tf);
u8g2_DrawStr(&u8g2, 2,17,"Hello World!");
u8g2_SendBuffer(&u8g2);
vTaskDelay(5000 / portTICK_RATE_MS);
}
}

-----------------------------------------------------

u8g2_esp32_hal.c
****************

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "u8g2_esp32_hal.h"

#define U8G2_USE_DYNAMIC_ALLOC // teste de alocação dinamica de memoria

static const char *TAG = "u8g2_hal";
static const unsigned int I2C_TIMEOUT_MS = 1000;

static spi_device_handle_t handle_spi; // SPI handle.
static i2c_cmd_handle_t handle_i2c; // I2C handle.
static u8g2_esp32_hal_t u8g2_esp32_hal; // HAL state data.

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x) do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

/*
* Initialze the ESP32 HAL.
*/
void u8g2_esp32_hal_init(u8g2_esp32_hal_t u8g2_esp32_hal_param) {
u8g2_esp32_hal = u8g2_esp32_hal_param;
} // u8g2_esp32_hal_init

/*
* HAL callback function as prescribed by the U8G2 library. This callback is invoked
* to handle SPI communications.
*/
uint8_t u8g2_esp32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
ESP_LOGD(TAG, "spi_byte_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);
switch(msg) {
case U8X8_MSG_BYTE_SET_DC:
if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.dc, arg_int);
}
break;

case U8X8_MSG_BYTE_INIT: {
if (u8g2_esp32_hal.clk == U8G2_ESP32_HAL_UNDEFINED ||
u8g2_esp32_hal.mosi == U8G2_ESP32_HAL_UNDEFINED ||
u8g2_esp32_hal.cs == U8G2_ESP32_HAL_UNDEFINED) {
break;
}

spi_bus_config_t bus_config;
bus_config.sclk_io_num = u8g2_esp32_hal.clk; // CLK
bus_config.mosi_io_num = u8g2_esp32_hal.mosi; // MOSI
bus_config.miso_io_num = -1; // MISO
bus_config.quadwp_io_num = -1; // Not used
bus_config.quadhd_io_num = -1; // Not used
//ESP_LOGI(TAG, "... Initializing bus.");
ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));

spi_device_interface_config_t dev_config;
dev_config.address_bits = 0;
dev_config.command_bits = 0;
dev_config.dummy_bits = 0;
dev_config.mode = 0;
dev_config.duty_cycle_pos = 0;
dev_config.cs_ena_posttrans = 0;
dev_config.cs_ena_pretrans = 0;
dev_config.clock_speed_hz = 10000;
dev_config.spics_io_num = u8g2_esp32_hal.cs;
dev_config.flags = 0;
dev_config.queue_size = 200;
dev_config.pre_cb = NULL;
dev_config.post_cb = NULL;
//ESP_LOGI(TAG, "... Adding device bus.");
ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &handle_spi));

break;
}

case U8X8_MSG_BYTE_SEND: {
spi_transaction_t trans_desc;
trans_desc.addr = 0;
trans_desc.cmd = 0;
trans_desc.flags = 0;
trans_desc.length = 8 * arg_int; // Number of bits NOT number of bytes.
trans_desc.rxlength = 0;
trans_desc.tx_buffer = arg_ptr;
trans_desc.rx_buffer = NULL;

//ESP_LOGI(TAG, "... Transmitting %d bytes.", arg_int);
ESP_ERROR_CHECK(spi_device_transmit(handle_spi, &trans_desc));
break;
}
}
return 0;
} // u8g2_esp32_spi_byte_cb

/*
* HAL callback function as prescribed by the U8G2 library. This callback is invoked
* to handle I2C communications.
*/
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
ESP_LOGD(TAG, "i2c_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);

switch(msg) {
case U8X8_MSG_BYTE_SET_DC: {
if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.dc, arg_int);
}
break;
}

case U8X8_MSG_BYTE_INIT: {
if (u8g2_esp32_hal.sda == U8G2_ESP32_HAL_UNDEFINED ||
u8g2_esp32_hal.scl == U8G2_ESP32_HAL_UNDEFINED) {
break;
}

i2c_config_t conf;
conf.mode = I2C_MODE_MASTER;
ESP_LOGI(TAG, "sda_io_num %d", u8g2_esp32_hal.sda);
conf.sda_io_num = u8g2_esp32_hal.sda;
conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
ESP_LOGI(TAG, "scl_io_num %d", u8g2_esp32_hal.scl);
conf.scl_io_num = u8g2_esp32_hal.scl;
conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
ESP_LOGI(TAG, "clk_speed %d", I2C_MASTER_FREQ_HZ);
conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
ESP_LOGI(TAG, "i2c_param_config %d", conf.mode);
ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
ESP_LOGI(TAG, "i2c_driver_install %d", I2C_MASTER_NUM);
ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
break;
}

case U8X8_MSG_BYTE_SEND: {
uint8_t* data_ptr = (uint8_t*)arg_ptr;
ESP_LOG_BUFFER_HEXDUMP(TAG, data_ptr, arg_int, ESP_LOG_VERBOSE);

while( arg_int > 0 ) {
ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, *data_ptr, ACK_CHECK_EN));
data_ptr++;
arg_int--;
}
break;
}

case U8X8_MSG_BYTE_START_TRANSFER: {
uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
handle_i2c = i2c_cmd_link_create();
ESP_LOGD(TAG, "Start I2C transfer to %02X.", i2c_address>>1);
ESP_ERROR_CHECK(i2c_master_start(handle_i2c));
ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, i2c_address | I2C_MASTER_WRITE, ACK_CHECK_EN));
break;
}

case U8X8_MSG_BYTE_END_TRANSFER: {
ESP_LOGD(TAG, "End I2C transfer.");
ESP_ERROR_CHECK(i2c_master_stop(handle_i2c));
ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_NUM, handle_i2c, I2C_TIMEOUT_MS / portTICK_RATE_MS));
i2c_cmd_link_delete(handle_i2c);
break;
}
}
return 0;
} // u8g2_esp32_i2c_byte_cb

/*
* HAL callback function as prescribed by the U8G2 library. This callback is invoked
* to handle callbacks for GPIO and delay functions.
*/
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
ESP_LOGD(TAG, "gpio_and_delay_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);

switch(msg) {
// Initialize the GPIO and DELAY HAL functions. If the pins for DC and RESET have been
// specified then we define those pins as GPIO outputs.
case U8X8_MSG_GPIO_AND_DELAY_INIT: {
uint64_t bitmask = 0;
if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
bitmask = bitmask | (1ull<<u8g2_esp32_hal.dc);
}
if (u8g2_esp32_hal.reset != U8G2_ESP32_HAL_UNDEFINED) {
bitmask = bitmask | (1ull<<u8g2_esp32_hal.reset);
}
if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
bitmask = bitmask | (1ull<<u8g2_esp32_hal.cs);
}

if (bitmask==0) {
break;
}
gpio_config_t gpioConfig;
gpioConfig.pin_bit_mask = bitmask;
gpioConfig.mode = GPIO_MODE_OUTPUT;
gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
gpioConfig.intr_type = GPIO_INTR_DISABLE;
gpio_config(&gpioConfig);
break;
}

// Set the GPIO reset pin to the value passed in through arg_int.
case U8X8_MSG_GPIO_RESET:
if (u8g2_esp32_hal.reset != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.reset, arg_int);
}
break;
// Set the GPIO client select pin to the value passed in through arg_int.
case U8X8_MSG_GPIO_CS:
if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.cs, arg_int);
}
break;
// Set the Software I²C pin to the value passed in through arg_int.
case U8X8_MSG_GPIO_I2C_CLOCK:
if (u8g2_esp32_hal.scl != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.scl, arg_int);
// printf("%c",(arg_int==1?'C':'c'));
}
break;
// Set the Software I²C pin to the value passed in through arg_int.
case U8X8_MSG_GPIO_I2C_DATA:
if (u8g2_esp32_hal.sda != U8G2_ESP32_HAL_UNDEFINED) {
gpio_set_level(u8g2_esp32_hal.sda, arg_int);
// printf("%c",(arg_int==1?'D':'d'));
}
break;

// Delay for the number of milliseconds passed in through arg_int.
case U8X8_MSG_DELAY_MILLI:
vTaskDelay(arg_int/portTICK_PERIOD_MS);
break;
}
return 0;
} // u8g2_esp32_gpio_and_delay_cb

-----------------------------------------------------

u8g2_esp32-hal.h
****************

#ifndef U8G2_ESP32_HAL_H_
#define U8G2_ESP32_HAL_H_
#include "u8g2.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"

#define U8G2_USE_DYNAMIC_ALLOC //Teste de alocação dinamica de memoria

#define U8G2_ESP32_HAL_UNDEFINED (-1)

#define I2C_MASTER_NUM I2C_NUM_1 // I2C port number for master dev
#define I2C_MASTER_TX_BUF_DISABLE 0 // I2C master do not need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 // I2C master do not need buffer
#define I2C_MASTER_FREQ_HZ 40000 // I2C master clock frequency
#define ACK_CHECK_EN 0x1 // I2C master will check ack from slave
#define ACK_CHECK_DIS 0x0 // I2C master will not check ack from slave

typedef struct {
gpio_num_t clk;
gpio_num_t mosi;
gpio_num_t sda; // data for I²C
gpio_num_t scl; // clock for I²C
gpio_num_t cs;
gpio_num_t reset;
gpio_num_t dc;
} u8g2_esp32_hal_t ;

#define U8G2_ESP32_HAL_DEFAULT {U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED, U8G2_ESP32_HAL_UNDEFINED }

void u8g2_esp32_hal_init(u8g2_esp32_hal_t u8g2_esp32_hal_param);
uint8_t u8g2_esp32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
#endif /* U8G2_ESP32_HAL_H_ */
-‐---------------------------------------------------

Error Log
*********

rst:0x1 (POWERON_RESET),boot:0x17 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:6836
load:0x40078000,len:14076
load:0x40080400,len:4304
entry 0x400806e8
I (71) boot: Chip Revision: 1
I (71) boot_comm: chip revision: 1, min. bootloader chip revision: 0
I (39) boot: ESP-IDF v4.0-dirty 2nd stage bootloader
I (39) boot: compile time 11:50:23
I (40) boot: Enabling RNG early entropy source...
I (44) boot: SPI Speed : 40MHz
I (48) boot: SPI Mode : DIO
I (52) boot: SPI Flash Size : 4MB
I (57) boot: Partition Table:
I (60) boot: ## Label Usage Type ST Offset Length
I (67) boot: 0 nvs WiFi data 01 02 00009000 00006000
I (75) boot: 1 phy_init RF data 01 01 0000f000 00001000
I (82) boot: 2 factory factory app 00 00 00010000 00100000
I (90) boot: End of partition table
I (94) boot_comm: chip revision: 1, min. application chip revision: 0
I (101) esp_image: segment 0: paddr=0x00010020 vaddr=0x3f400020 size=0x08e2c ( 36396) map
I (123) esp_image: segment 1: paddr=0x00018e54 vaddr=0x3ffbdb60 size=0x021f0 ( 8688) load
I (127) esp_image: segment 2: paddr=0x0001b04c vaddr=0x40080000 size=0x00400 ( 1024) load
0x40080000: _WindowOverflow4 at C:/Users/ISM/ESP32/components/freertos/xtensa_vectors.S:1778

I (131) esp_image: segment 3: paddr=0x0001b454 vaddr=0x40080400 size=0x04bbc ( 19388) load
I (147) esp_image: segment 4: paddr=0x00020018 vaddr=0x400d0018 size=0x17d08 ( 97544) map
0x400d0018: _stext at ??:?

I (183) esp_image: segment 5: paddr=0x00037d28 vaddr=0x40084fbc size=0x06370 ( 25456) load
0x40084fbc: prvInitializeNewRingbuffer at C:/Users/ISM/ESP32/components/esp_ringbuf/ringbuf.c:252

I (201) boot: Loaded app from partition at offset 0x10000
I (201) boot: Disabling RNG early entropy source...
I (201) cpu_start: Pro cpu up.
I (205) cpu_start: Application information:
I (210) cpu_start: Project name: oled-teste
I (215) cpu_start: App version: v4.0-dirty
I (220) cpu_start: Compile time: May 22 2020 11:50:02
I (226) cpu_start: ELF file SHA256: 6d4c41654adcb3a3...
I (232) cpu_start: ESP-IDF: v4.0-dirty
I (238) cpu_start: Starting app cpu, entry point is 0x400811a8
0x400811a8: call_start_cpu1 at C:/Users/ISM/ESP32/components/esp32/cpu_start.c:272

I (0) cpu_start: App cpu up.
I (248) heap_init: Initializing. RAM available for dynamic allocation:
I (255) heap_init: At 3FFAE6E0 len 0000F480 (61 KiB): DRAM
I (261) heap_init: At 3FFC1188 len 0001EE78 (123 KiB): DRAM
I (267) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (274) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (280) heap_init: At 4008B32C len 00014CD4 (83 KiB): IRAM
I (286) cpu_start: Pro cpu start user code
I (304) spi_flash: detected chip: generic
I (305) spi_flash: flash io: dio
I (305) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (313) u8g2_hal: sda_io_num 5
I (313) u8g2_hal: scl_io_num 4
I (323) u8g2_hal: clk_speed 40000
I (323) u8g2_hal: i2c_param_config 1
I (333) u8g2_hal: i2c_driver_install 1
E (633) err: esp_err_t = -1
assertion "0 && "i2c_master_cmd_begin(I2C_MASTER_NUM, handle_i2c, I2C_TIMEOUT_MS / portTICK_RATE_MS)"" failed: file "../main/u8g2_esp32_hal.c", line 158, function: u8g2_esp32_i2c_byte_cb
abort() was called at PC 0x400d8ec3 on core 0
0x400d8ec3: __assert_func at /builds/idf/crosstool-NG/.build/HOST-i686-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/stdlib/assert.c:62 (discriminator 8)

ELF file SHA256: 6d4c41654adcb3a3d620cc0f570ab5d15dae3edfde17de3a0bd8acfafa2599e7

Backtrace: 0x40086169:0x3ffb1c30 0x400864dd:0x3ffb1c50 0x400d8ec3:0x3ffb1c70 0x400d4302:0x3ffb1ca0 0x400d4b9b:0x3ffb1ce0 0x400d4b1d:0x3ffb1d00 0x400d4f79:0x3ffb1d30 0x400e788d:0x3ffb1d50 0x400d400e:0x3ffb1d70 0x400d0ea6:0x3ffb1e50 0x40088971:0x3ffb1e70
0x40086169: invoke_abort at C:/Users/ISM/ESP32/components/esp32/panic.c:155

0x400864dd: abort at C:/Users/ISM/ESP32/components/esp32/panic.c:172

0x400d8ec3: __assert_func at /builds/idf/crosstool-NG/.build/HOST-i686-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/stdlib/assert.c:62 (discriminator 8)

0x400d4302: u8g2_esp32_i2c_byte_cb at c:\users\ism\esp32\ism-tcc\oled-teste\build/../main/u8g2_esp32_hal.c:158 (discriminator 5)

0x400d4b9b: u8x8_cad_001 at c:\users\ism\esp32\ism-tcc\oled-teste\build/../components/u8g2/csrc/u8x8_cad.c:309

0x400d4b1d: u8x8_cad_SendSequence at c:\users\ism\esp32\ism-tcc\oled-teste\build/../components/u8g2/csrc/u8x8_cad.c:183

0x400d4f79: u8x8_d_ssd1306_128x64_noname at c:\users\ism\esp32\ism-tcc\oled-teste\build/../components/u8g2/csrc/u8x8_d_ssd1306_128x64_noname.c:322

0x400e788d: u8x8_InitDisplay at c:\users\ism\esp32\ism-tcc\oled-teste\build/../components/u8g2/csrc/u8x8_display.c:100

0x400d400e: app_main at c:\users\ism\esp32\ism-tcc\oled-teste\build/../main/main.c:41

0x400d0ea6: main_task at C:/Users/ISM/ESP32/components/esp32/cpu_start.c:554

0x40088971: vPortTaskWrapper at C:/Users/ISM/ESP32/components/freertos/port.c:143

Rebooting...