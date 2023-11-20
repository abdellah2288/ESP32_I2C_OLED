# I2C OLED Display Library

This library provides functions and structures for controlling an I2C OLED display using the ESP-IDF framework. It includes features for drawing pixels, characters, BMP images, and XBM images on the OLED display. The library is designed to work with the ESP32 microcontroller.

##    Usage
- Initialization
- Drawing Pixels
-  Drawing Characters
- Drawing BMP Images
- Drawing XBM Images
##    Structures
- i2c_oled_t
- bmp_t
- xbm_t
##    Functions
- i2c_init_master
- i2c_oled_refresh
- i2c_oled_clear
- i2c_oled_init
- i2c_oled_draw_pixel
- gen_bmp
- draw_bmp
- gen_xbm
- draw_xbm
- i2c_oled_draw_char
- i2c_oled_text_newline
- i2c_oled_invert_line
- i2c_oled_reset_cols
- draw_char
- i2c_oled_draw_string

## Example usage
```
#include <stdio.h>
#include <string.h>
#include "I2C_OLED.h"
i2c_oled_t oled =
{
		I2C_NUM_0,
		GPIO_NUM_21,
		GPIO_NUM_22,
		0x3C,
		64,
		128,
		0,
		NULL,
		{-1,-1}
};
void app_main(void)
{
    i2c_init_master(&oled);

    i2c_oled_init(&oled);
    i2c_oled_clear(&oled);
    i2c_oled_draw_string(&oled,"Test",4,1);
    i2c_oled_refresh(&oled);
    while(1)
    {
    printf("Ping !\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
```
## Author
Abdellah R.
Github: [abdellah2288](https://github.com/abdellah2288)
## License
[GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)
