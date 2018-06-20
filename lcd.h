/*
 * HD44780 LCD Display Library
 *
 * MIT License
 * Copyright (c) 2018, Michal Kozakiewicz, github.com/michal037
 */

#ifndef _DISPLAY_LCD_HD44780_H_
#define _DISPLAY_LCD_HD44780_H_

/* Functions */

/* Initialize the display */
void lcd_init(void);

/* Clear the screen */
void lcd_clear(void);

/* Move cursor to position [0,0] */
void lcd_home(void);

/* Move cursor to position [x,y] */
void lcd_setCursor(uint8_t x, uint8_t y);

/*
 * Turn display ON or OFF
 *
 * Parameters:
 *   uint8_t onoff - 1 = ON; 0 = OFF
 */
void lcd_displayOnOff(uint8_t onoff);

/*
 * Turn cursor ON or OFF
 *
 * Parameters:
 *   uint8_t onoff - 1 = ON; 0 = OFF
 */
void lcd_cursorOnOff(uint8_t onoff);

/*
 * Turn cursor blink ON or OFF
 *
 * Parameters:
 *   uint8_t onoff - 1 = ON; 0 = OFF
 */
void lcd_cursorBlinkOnOff(uint8_t onoff);

/*
 * Cursor Or Display Move To Left Or Right
 *
 * Parameters:
 *   uint8_t cod - Move: 0 = Cursor; 1 = Display
 *   uint8_t lor - Move to: 0 = Left; 1 = Right
 */
void lcd_codmtlor(uint8_t cod, uint8_t lor);

/*
 * Entry Mode Left Or Right Decrement Or Increment
 *
 * Parameters:
 *   uint8_t lor - 0 = Left; 1 = Right
 *   uint8_t doi - 0 = Decrement; 1 = Increment
 */
void lcd_emlordoi(uint8_t lor, uint8_t doi);

/*
 * Write one character
 *
 * Parameters:
 *   char c - a character
 */
void lcd_char(char c);

/*
 * Write a string
 *
 * Parameters:
 *   char *str - a string
 */
void lcd_str(char *str);

/*
 * Write a string from FLASH memory
 *
 * Parameters:
 *   const char *str - a string
 */
void lcd_str_P(const char *str);

/*
 * Write a string from EEPROM memory
 * Remember to upload data to the EEPROM memory!
 *
 * Parameters:
 *   char *str - a string
 */
void lcd_str_E(char *str);

/*
 * Write a decimal number
 *
 * Parameters:
 *   int16_t value - number from -32768 to 32767
 */
void lcd_int16(int16_t value);

/*
 * Write a decimal number
 *
 * Parameters:
 *   uint16_t value - number from 0 to 65535
 */
void lcd_uint16(uint16_t value);

/*
 * Write a decimal number
 *
 * Parameters:
 *   int32_t value - number from -2147483648 to 2147483647
 */
void lcd_int32(int32_t value);

/*
 * Write a decimal number
 *
 * Parameters:
 *   uint32_t value - number from 0 to 4294967295
 */
void lcd_uint32(uint32_t value);

/*
 * Write a hexadecimal number
 *
 * Parameters:
 *   uint32_t value - number from 0x0 to 0xFFFFFFFF
 */
void lcd_hex(uint32_t value);

/*
 * Create your own character
 *
 * Parameters:
 *   uint8_t code - character number from 0x80 to 0x87
 *   uint8_t *pattern - 8 bytes of character pattern
 */
void lcd_createChar(uint8_t code, uint8_t *pattern);

/*
 * Create your own character from FLASH memory
 *
 * Parameters:
 *   uint8_t code - character number from 0x80 to 0x87
 *   const uint8_t *pattern - 8 bytes of character pattern
 */
void lcd_createChar_P(uint8_t code, const uint8_t *pattern);

/*
 * Create your own character from EEPROM memory
 * Remember to upload data to the EEPROM memory!
 *
 * Parameters:
 *   uint8_t code - character number from 0x80 to 0x87
 *   uint8_t *pattern - 8 bytes of character pattern
 */
void lcd_createChar_E(uint8_t code, uint8_t *pattern);

/* End of functions */

/* Configuration */

/* The number of rows and columns of your display */
#define HD44780_ROWS    2
#define HD44780_COLUMNS 16

/* 
 * Do you use the RW pin? 
 * 1 - Yes
 * 0 - No
 */
#define HD44780_USE_RW_PIN 1

/* Pins for DATA */
#define HD44780_IO_D4_PORT A
#define HD44780_IO_D4 3
#define HD44780_IO_D5_PORT A
#define HD44780_IO_D5 4
#define HD44780_IO_D6_PORT A
#define HD44780_IO_D6 5
#define HD44780_IO_D7_PORT A
#define HD44780_IO_D7 6

/* Pins for CONTROL */
#define HD44780_IO_RS_PORT A
#define HD44780_IO_RS 0
#define HD44780_IO_RW_PORT A
#define HD44780_IO_RW 1
#define HD44780_IO_E_PORT  A
#define HD44780_IO_E  2

/* End of configuration */

#endif /* _DISPLAY_LCD_HD44780_H_ */
