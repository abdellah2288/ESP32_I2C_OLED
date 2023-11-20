#include <driver/gpio.h>
#include <driver/i2c.h>
#include "freertos/portmacro.h"
#include "hal/i2c_types.h"
#include <unistd.h>
#include <esp_log.h>
#include <string.h>
#include <math.h>

#ifndef _I2C_OLED_
#define _I2C_OLED_
#define ACK_EN 1
#define OLED_INITIATED 1
#define SEQUENCE_SIZE 26
// DISPLAY DIMENSIONS
#define HEIGHT	64
#define WIDTH	128
// CONTROL BYTES
#define I2C_CONTROL_CMD_STREAM 0x00
#define I2C_CONTROL_CMD_BYTE	0x80
#define I2C_CONTROL_DATA_STREAM 0x40
#define I2C_CONTROL_DATA_BYTE 0xC0
//INIT SEQUENCE COMMANDS
#define I2C_OLED_SET_DISPLAY_OFF 0xAE
#define I2C_OLED_SET_CLK_DIV 0xD5
#define I2C_OLED_SET_MUX_RATIO 0xA8
#define I2C_OLED_SET_DISPLAY_OFFSET 0xD3
#define I2C_OLED_SET_START_LINE_RAM0 0x40
#define I2C_OLED_SET_CHARGE_PUMP_SETTINGS 0x8D
#define I2C_OLED_SET_MEM_ADDR_MODE 0x20
#define I2C_OLED_SET_SEG_REMAP_NONE 0xA1
#define I2C_OLED_SET_COM_SCAN_DIR 0xC8
#define I2C_OLED_SET_COM_CONF 0xDA
#define I2C_OLED_SET_CONTRAST 0x81
#define I2C_OLED_SET_PRECHARGE 0xD9
#define I2C_OLED_SET_VCOMH 0xDB
#define I2C_OLED_RESUME_GDDRAM_MAPPING 0xA4
#define I2C_OLED_SET_DISPLAY_INV_OFF 0xA6
#define I2C_OLED_SET_SCROLL_OFF 0x2E
#define I2C_OLED_SET_DISP_ON 0xAF
//ADDRESSING MODES
#define I2C_OLED_PAGE_ADDRESSING 0x02
#define I2C_OLED_VERTICAL 0x01
#define I2C_OLED_HORIZONTAL_ADDRESSING 0x00
//OTHER COMMANDS
#define I2C_OLED_SET_COL_INTERVAL 0x21
#define I2C_OLED_SET_PAGE_INTERVAL 0x20
/**
 * @brief Struct representing the configuration for an I2C OLED display.
 *
 * This struct encapsulates the parameters and settings needed to initialize and control
 * an I2C OLED display. It includes information such as the I2C port, SDA (data line),
 * SCL (clock line), device address, vertical and horizontal dimensions, initialization flag,
 * a buffer for data, and the last character displayed on the OLED in both lines.
 *
 * @note Ensure proper initialization of the struct members before using it to control an OLED display.
 *
 * @var i2c_port_t i2c_port: The I2C port used for communication with the OLED display.
 * @var int sda: The GPIO pin for the I2C SDA (data) line.
 * @var int scl: The GPIO pin for the I2C SCL (clock) line.
 * @var int address: The I2C address of the OLED display.
 * @var int vertical: Vertical dimension of the display.
 * @var int horizontal: Horizontal dimension of the display.
 * @var int init_flag: Initialization flag to track the state of the OLED display initialization.
 * @var uint8_t* buffer: Buffer for storing data to be sent to the OLED display.
 * @var int last_char[2]: Array to store the last character displayed in each line of the OLED.
 */
typedef struct {
	i2c_port_t i2c_port;
	int sda;
	int scl;
	int address;
	int vertical;
	int horizontal;
	int init_flag;
	uint8_t* buffer;
	int last_char[2];
} i2c_oled_t;

/**
 * @brief Struct representing a Bitmap (BMP) image.
 *
 * This struct holds information related to a Bitmap image, including its horizontal and
 * vertical dimensions, as well as a pointer to the image data.
 *
 * @note Ensure proper initialization of the struct members before using it to handle a BMP image.
 *
 * @var int horizontal: Horizontal dimension of the BMP image.
 * @var int vertical: Vertical dimension of the BMP image.
 * @var uint8_t* image: Pointer to the image data.
 */
typedef struct {
	int horizontal;
	int vertical;
	uint8_t* image;

}bmp_t;

typedef bmp_t xbm_t;

/**
 * @brief Initializes the i2c driver for the port used by the i2c oled 
 *
 * @note Ensure this function is only called once and driver isnt already initialized
 *
 * @param oled the target i2c oled
 */
esp_err_t i2c_init_master(const i2c_oled_t* oled);

/**
 * @brief Draws buffer on the i2c oled
 *
 * @param oled the target i2c oled
 */
void i2c_oled_refresh(const  i2c_oled_t* oled);

/**
 * @brief Clears the i2c oled's buffer 
 *
 * @note This function does not refresh the display
 *
 * @param oled the target i2c oled
 */
void i2c_oled_clear(i2c_oled_t* oled);

/**
 * @brief Clears the i2c oled's buffer 
 *
 * @note This function does not refresh the display
 *
 * @param oled the target i2c oled
 */
esp_err_t i2c_oled_init(i2c_oled_t* oled);


/**
 * @brief Draw a pixel on the I2C OLED display at the specified coordinates.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 */
void i2c_oled_draw_pixel(i2c_oled_t* oled, int x, int y);

/**
 * @brief Generate a BMP image from the given array and dimensions, and associate it with the provided BMP struct.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param image Pointer to the BMP struct to be populated.
 * @param array Pointer to the BMP image data array.
 * @param vertical Vertical dimension of the BMP image.
 * @param horizontal Horizontal dimension of the BMP image.
 */
void gen_bmp(const i2c_oled_t* oled, bmp_t* image, const uint8_t* array, int vertical, int horizontal);

/**
 * @brief Draw a BMP image on the I2C OLED display at the specified offset.
 *
 * @param image Pointer to the BMP struct representing the image.
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param x_offset X-coordinate offset for drawing the BMP image.
 * @param y_offset Y-coordinate offset for drawing the BMP image.
 */
void draw_bmp(const bmp_t* image, i2c_oled_t* oled, int x_offset, int y_offset);

/**
 * @brief Generate an XBM image from the given array and dimensions, and associate it with the provided XBM struct.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param image Pointer to the XBM struct to be populated.
 * @param array Pointer to the XBM image data array.
 * @param vertical Vertical dimension of the XBM image.
 * @param horizontal Horizontal dimension of the XBM image.
 */
void gen_xbm(const i2c_oled_t* oled, xbm_t* image, const uint8_t* array, int vertical, int horizontal);

/**
 * @brief Draw an XBM image on the I2C OLED display with optional inversion, centering, and offset.
 *
 * @param image Pointer to the XBM struct representing the image.
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param invert Flag indicating whether to invert the colors.
 * @param center Flag indicating whether to center the image on the display.
 * @param x_offset X-coordinate offset for drawing the XBM image.
 * @param y_offset Y-coordinate offset for drawing the XBM image.
 */
void draw_xbm(const xbm_t* image, i2c_oled_t* oled, int invert, int center, int x_offset, int y_offset);

/**
 * @brief Draw a character on the I2C OLED display.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param character The character to be drawn.
 */
void i2c_oled_draw_char(i2c_oled_t* oled, char character);

/**
 * @brief Move the cursor to the beginning of the next line on the I2C OLED display.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 */
void i2c_oled_text_newline(i2c_oled_t* oled);

/**
 * @brief Invert the colors of a specific line on the I2C OLED display.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param line_num Line number to invert (starting from 0).
 */
void i2c_oled_invert_line(i2c_oled_t* oled, int line_num);

/**
 * @brief Reset the cursor position to the beginning of the line on the I2C OLED display.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 */
void i2c_oled_reset_cols(const i2c_oled_t* oled);

/**
 * @brief Draw a character represented by an XBM struct on the I2C OLED display with optional centering and offset.
 *
 * @param chr Pointer to the XBM struct representing the character.
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param center Flag indicating whether to center the character on the display.
 * @param x_offset X-coordinate offset for drawing the character.
 * @param y_offset Y-coordinate offset for drawing the character.
 */
void draw_char(const xbm_t* chr, i2c_oled_t* oled, int center, int x_offset, int y_offset);

/**
 * @brief Draw a string on the I2C OLED display with optional centering.
 *
 * @param oled Pointer to the I2C OLED configuration struct.
 * @param string Pointer to the string to be drawn.
 * @param string_len Length of the string.
 * @param center Flag indicating whether to center the string on the display.
 */
void i2c_oled_draw_string(i2c_oled_t* oled, char* string, int string_len, int center);

#endif
