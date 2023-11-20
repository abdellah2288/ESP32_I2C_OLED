#include "I2C_OLED.h"
#include "font.h"

esp_err_t i2c_init_master(const i2c_oled_t *oled)
{
	i2c_config_t conf = {
	    .mode = I2C_MODE_MASTER,
	    .sda_io_num = oled->sda,         // select GPIO specific to your project
	    .sda_pullup_en = GPIO_PULLUP_ENABLE,
	    .scl_io_num = oled->scl,         // select GPIO specific to your project
	    .scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 400000,                       // you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
	};
    i2c_param_config(oled->i2c_port, &conf);

    return i2c_driver_install(oled->i2c_port, conf.mode, 0, 0, 0);
}

esp_err_t i2c_oled_init(i2c_oled_t* oled)
{
	oled->buffer = (uint8_t*) malloc(1024);

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(oled->address << 1) | I2C_MASTER_WRITE,ACK_EN);
	i2c_master_write_byte(cmd,I2C_CONTROL_CMD_STREAM,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_DISPLAY_OFF,ACK_EN);


	i2c_master_write_byte(cmd,I2C_OLED_SET_CLK_DIV,ACK_EN);
	i2c_master_write_byte(cmd,0x80,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_MUX_RATIO,ACK_EN);
	i2c_master_write_byte(cmd,0x3F,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_DISPLAY_OFFSET,ACK_EN);
	i2c_master_write_byte(cmd,0x00,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_START_LINE_RAM0,ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_SET_CHARGE_PUMP_SETTINGS,ACK_EN);
	i2c_master_write_byte(cmd, 0x14,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_MEM_ADDR_MODE ,ACK_EN);
	i2c_master_write_byte(cmd,I2C_OLED_PAGE_ADDRESSING ,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_SEG_REMAP_NONE,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_COM_SCAN_DIR,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_COM_CONF,ACK_EN);
	i2c_master_write_byte(cmd, 0x12, ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_SET_PRECHARGE, ACK_EN);
	i2c_master_write_byte(cmd, 0xF1, ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_SET_CONTRAST, ACK_EN);
	i2c_master_write_byte(cmd, 0xFF, ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_SET_VCOMH, ACK_EN);
	i2c_master_write_byte(cmd, 0x40, ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_RESUME_GDDRAM_MAPPING, ACK_EN);

	i2c_master_write_byte(cmd, I2C_OLED_SET_DISPLAY_INV_OFF, ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_SCROLL_OFF,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_DISP_ON,ACK_EN);

	i2c_master_write_byte(cmd,I2C_OLED_SET_COL_INTERVAL,ACK_EN);
	i2c_master_write_byte(cmd,0x00,ACK_EN);
	i2c_master_write_byte(cmd,0x7F,ACK_EN);

	i2c_master_stop(cmd);
	esp_err_t err = i2c_master_cmd_begin(oled->i2c_port,cmd,500/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	return err;
}

void i2c_oled_test(const i2c_oled_t* oled)
{
	for(int i=0xB0;i<0xB8;i++)
	{
		uint8_t sequence[]={I2C_CONTROL_CMD_BYTE,i};
		i2c_master_write_to_device(oled->i2c_port,oled->address,sequence,2,500/portTICK_PERIOD_MS);

		sequence[0]=	I2C_CONTROL_DATA_BYTE;
		sequence[1] = i%2 ? 0xFF:0x00;
		for(int j = 0;j<128;j++)
		{
			i2c_master_write_to_device(oled->i2c_port,oled->address,sequence,2,500/portTICK_PERIOD_MS);
			vTaskDelay(7 / portTICK_PERIOD_MS);
		}
	}

	for(int i=0xB0;i<0xB8;i++)
	{
		uint8_t sequence[]={I2C_CONTROL_CMD_BYTE,i};
		i2c_master_write_to_device(oled->i2c_port,oled->address,sequence,2,500/portTICK_PERIOD_MS);

		sequence[0]=	I2C_CONTROL_DATA_BYTE;

		for(int j = 0;j<128;j++)
		{
			sequence[1] = j%2 ? 0xFF:0x00;
			i2c_master_write_to_device(oled->i2c_port,oled->address,sequence,2,500/portTICK_PERIOD_MS);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}
}

void i2c_oled_clear( i2c_oled_t* oled)
{
	oled->last_char[0] = -8;
	oled->last_char[1] = -8;
	memset(oled->buffer,0,1024);
}

void i2c_oled_reset_cols(const  i2c_oled_t* oled)
{
	uint8_t reset_sequence[] = {I2C_OLED_SET_COL_INTERVAL,0x00,0x7F};
	i2c_master_write_to_device(oled->i2c_port,oled->address,reset_sequence,3,500/portTICK_PERIOD_MS);
}

void i2c_oled_refresh(const  i2c_oled_t* oled)
{
	int index = 0;
	uint8_t page_select[] = {I2C_CONTROL_CMD_BYTE,0xB0};
	uint8_t data_byte[] = {I2C_CONTROL_DATA_BYTE,0x00};
	for(int i=0xB0; i<0xB8 ;i++)
	{
		page_select[1] = i;
		index = i - 0xB0;
		i2c_master_write_to_device(oled->i2c_port,oled->address,page_select,2,500/portTICK_PERIOD_MS);

		for(int j =0; j < 128;j++)
		{
			data_byte[1] = oled->buffer[j+index*127];
			i2c_master_write_to_device(oled->i2c_port,oled->address,data_byte,2,500/portTICK_PERIOD_MS);
		}
	}
	i2c_oled_reset_cols(oled);
}

void i2c_oled_draw_pixel(i2c_oled_t* oled,int x,int y)
{
	int page = y / 8;
	int _shift = y % 8;
	oled->buffer[x+page*127] = oled->buffer[x+page*127] | (1 << _shift);
}

void gen_bmp(const i2c_oled_t* oled,bmp_t* image,const uint8_t* array ,int vertical,int horizontal)
{
	horizontal = horizontal <= oled->horizontal ? horizontal : oled->horizontal - 1 ; //clamp horizontal
	vertical = vertical <= oled->vertical ? vertical : oled->vertical ; //clamp vertical

	int size = vertical * horizontal / 8;

	image->image = (uint8_t*) malloc(size);
	image->vertical = vertical;
	image->horizontal = horizontal;

	memcpy(image->image,array,size);
}

void gen_xbm(const i2c_oled_t* oled, xbm_t* image,const uint8_t* array ,int vertical,int horizontal)
{
	horizontal = horizontal <= oled->horizontal ? horizontal : oled->horizontal  ; //clamp horizontal
	vertical = vertical <= oled->vertical ? vertical : oled->vertical ; //clamp vertical

	int size = vertical * horizontal / 8;

	image->image = (uint8_t*) malloc(size);
	image->vertical = vertical;
	image->horizontal = horizontal;

	memcpy(image->image,array,size);
}

void draw_bmp(const bmp_t* image,i2c_oled_t* oled,int x_offset,int y_offset)
{
	int index = 0;
	int _shift = 0;
	int section =  0;
	for(int i=0;i < image->vertical;i++)
	{
	index = i*image->horizontal/8;
	for(int j=0;j < image->horizontal;j++)
	{
			_shift = 7 - j % 8;
			section = j / 8;
			if(image->image[section + index ] & (1<< _shift) )i2c_oled_draw_pixel(oled,x_offset + j, y_offset + i);
	}
	}
}

void draw_xbm(const xbm_t* image,i2c_oled_t* oled, int invert,int center,int x_offset,int y_offset)
{
	int _shift = 0;
	int section =  0;
	int index = 0;
	int x_start = center ? oled->horizontal/2 - image->horizontal / 2 : 0;
	for(int i=0;i < image->vertical;i++)
	{
	index = i*image->horizontal /8;
	for(int j=0 ;j < image->horizontal;j++)
	{
		_shift =  j % 8;
		section = j / 8;
		if((image->image[section + index ] & (1<< _shift)) == invert) i2c_oled_draw_pixel(oled,j+x_start + x_offset, i + y_offset);
	}
	}
}

void draw_char(const xbm_t* chr,i2c_oled_t* oled,int center,int x_offset,int y_offset)
{
	int _shift = 0;
	int section =  0;
	int index = 0;
	if(center) x_offset += oled->horizontal/2 - 8 ;
	for(int i=0;i < chr->vertical;i++)
	{
	index = i*chr->horizontal/8;
	for(int j=0 ;j < chr->horizontal;j++)
	{
		_shift =  j % 8;
		section = j / 8;
		if((chr->image[section + index ] & (1<< _shift))) i2c_oled_draw_pixel(oled,j + x_offset, i + y_offset);
	}
	}
}

void i2c_oled_draw_char(i2c_oled_t* oled,char character)
{
	int index = character;
	if(oled->last_char[0] >= 0 || oled->last_char[1] >= 0)
	{
		if(oled->last_char[0] == oled->horizontal)
		{
			oled->last_char[1] += 8;
			oled->last_char[0] = 0;

		}

		else oled->last_char[0] += 8;

		if(oled->last_char[1] == oled->vertical) oled->last_char[1] = 0;

		xbm_t character_bmp;
		gen_xbm(oled,&character_bmp,font8x8_basic[index],8,8);
		draw_char(&character_bmp,oled,0,oled->last_char[0],oled->last_char[1]);
	}
	else
	{
		oled->last_char[0] = 0;
		oled->last_char[1] = 0;
		xbm_t character_bmp;
		gen_xbm(oled,&character_bmp,font8x8_basic[index],8,8);
		draw_char(&character_bmp,oled,0,0,0);
	}
}

void i2c_oled_draw_string(i2c_oled_t* oled,char* string,int string_len, int center)
{
	if(string_len<0)
	{
		string_len = 0;
		do
		{
			string_len ++;
		}
		while(*string++) ;

	}
	int _center = (oled->horizontal / 2 ) > (string_len / 2) ? oled->horizontal / 2 - string_len /2 : 0 ;
	if(center)
	{
	if(oled->last_char[0] < oled->horizontal / 2) oled->last_char[0] = _center;
	else
	{
		oled->last_char[1] = oled->last_char[1] < oled->vertical - 8 ? oled->last_char[1] + 8 : 0;
		oled->last_char[0] = _center;
	}
	}
	for(int i = 0; i < string_len ; i++)
	{
		i2c_oled_draw_char(oled,string[i]);
	}
}
void i2c_oled_text_newline(i2c_oled_t* oled)
{
	oled->last_char[1] = oled->last_char[1] == oled->vertical ? 0 : oled->last_char[1] + 8;
	oled->last_char[0] = -8;
}

void i2c_oled_invert_line(i2c_oled_t* oled,int line_num)
{
	for(int i = 1; i < oled->horizontal - 1; i ++ ) oled->buffer[line_num * (oled->horizontal - 1) + i ] = ~oled->buffer[line_num * (oled->horizontal - 1) + i];
	i2c_oled_refresh(oled);
}
