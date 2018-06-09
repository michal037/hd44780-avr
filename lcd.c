/*
 * HD44780 LCD Display Library
 *
 * MIT License
 * Copyright (c) from 2018, Michal Kozakiewicz, github.com/michal037
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lcd.h"

/* Simplifying macros */
#define DDR(n)   DDRQ(n)
#define DDRQ(n)  (DDR##n)
#define PORT(n)  PORTQ(n)
#define PORTQ(n) (PORT##n)
#define PIN(n)   PINQ(n)
#define PINQ(n)  (PIN##n)

/* HD44780 Commands */
#define CMD_CLEARDISPLAY 0x01
#define CMD_RETURNHOME 0x02
#define CMD_ENTRYMODESET 0x04
#define CMD_DISPLAYCONTROL 0x08
#define CMD_CURSORSHIFT 0x10
#define CMD_FUNCTIONSET 0x20
#define CMD_SETCGRAMADDR 0x40
#define CMD_SETDDRAMADDR 0x80

/* Flags for Entry Mode Set command */
#define CMD_ENTRYRIGHT 0x02
#define CMD_ENTRYLEFT 0x00
#define CMD_ENTRYSHIFTINCREMENT 0x00
#define CMD_ENTRYSHIFTDECREMENT 0x01

/* Flags for Display ON/OFF Control command */
#define CMD_DISPLAYON 0x04
#define CMD_DISPLAYOFF 0x00
#define CMD_CURSORON 0x02
#define CMD_CURSOROFF 0x00
#define CMD_BLINKON 0x01
#define CMD_BLINKOFF 0x00

/* Flags for Cursor Or Display Shift command */
#define CMD_DISPLAYMOVE 0x08
#define CMD_CURSORMOVE 0x00
#define CMD_MOVERIGHT 0x04
#define CMD_MOVELEFT 0x00

/* Flags for Function Set command */
#define CMD_8BITMODE 0x10
#define CMD_4BITMODE 0x00
#define CMD_2LINE 0x08
#define CMD_1LINE 0x00
#define CMD_5x10DOTS 0x04
#define CMD_5x8DOTS 0x00

/* Busy Flag */
#define CMD_BUSYFLAG 0x80

/* 
 * HD44780 DDRAM Addresses
 * Always check if these addresses are correct for your display!
 * LA - Line Address
 */
#define DDRAM_LA1 0x00  /* 1st char; 1st row */
#define DDRAM_LA2 0x40  /* 1st char; 2nd row */
#if ((HD44780_COLUMNS == 16) && (HD44780_ROWS == 4))
	#define DDRAM_LA3 0x10  /* 1st char; 3rd row */
	#define DDRAM_LA4 0x50  /* 1st char; 4th row */
#endif
#if ((HD44780_COLUMNS == 20) && (HD44780_ROWS == 4))
	#define DDRAM_LA3 0x14  /* 1st char; 3rd row */
	#define DDRAM_LA4 0x54  /* 1st char; 4th row */
#endif
#if ((HD44780_COLUMNS == 40) && (HD44780_ROWS == 4))
	#define DDRAM_LA3 0x00  /* 1st char; 3rd row */
	#define DDRAM_LA4 0x40  /* 1st char; 4th row */
#endif

/* Control pins set/clear */
#define SET_RS PORT(HD44780_IO_RS_PORT) |= (1<<HD44780_IO_RS)
#define CLR_RS PORT(HD44780_IO_RS_PORT) &= ~(1<<HD44780_IO_RS)
#define SET_RW PORT(HD44780_IO_RW_PORT) |= (1<<HD44780_IO_RW)
#define CLR_RW PORT(HD44780_IO_RW_PORT) &= ~(1<<HD44780_IO_RW)
#define SET_E  PORT(HD44780_IO_E_PORT)  |= (1<<HD44780_IO_E)
#define CLR_E  PORT(HD44780_IO_E_PORT)  &= ~(1<<HD44780_IO_E)

/* Data pins set/clear */
#define SET_D4 PORT(HD44780_IO_D4_PORT) |= (1<<HD44780_IO_D4)
#define CLR_D4 PORT(HD44780_IO_D4_PORT) &= ~(1<<HD44780_IO_D4)
#define SET_D5 PORT(HD44780_IO_D5_PORT) |= (1<<HD44780_IO_D5)
#define CLR_D5 PORT(HD44780_IO_D5_PORT) &= ~(1<<HD44780_IO_D5)
#define SET_D6 PORT(HD44780_IO_D6_PORT) |= (1<<HD44780_IO_D6)
#define CLR_D6 PORT(HD44780_IO_D6_PORT) &= ~(1<<HD44780_IO_D6)
#define SET_D7 PORT(HD44780_IO_D7_PORT) |= (1<<HD44780_IO_D7)
#define CLR_D7 PORT(HD44780_IO_D7_PORT) &= ~(1<<HD44780_IO_D7)

/* Data pins read */
#define READ_D4 (PIN(HD44780_IO_D4_PORT) & (1<<HD44780_IO_D4))
#define READ_D5 (PIN(HD44780_IO_D5_PORT) & (1<<HD44780_IO_D5))
#define READ_D6 (PIN(HD44780_IO_D6_PORT) & (1<<HD44780_IO_D6))
#define READ_D7 (PIN(HD44780_IO_D7_PORT) & (1<<HD44780_IO_D7))

/*
 * Read data from display driver
 *
 * Return: (uint8_t) - 8 data bits
 */
#if HD44780_USE_RW_PIN == 1
static uint8_t read8bits(void)
{
	uint8_t result = 0;

	/* Set the pins as inputs */
	DDR(HD44780_IO_D4_PORT) &= ~(1<<HD44780_IO_D4);
	DDR(HD44780_IO_D5_PORT) &= ~(1<<HD44780_IO_D5);
	DDR(HD44780_IO_D6_PORT) &= ~(1<<HD44780_IO_D6);
	DDR(HD44780_IO_D7_PORT) &= ~(1<<HD44780_IO_D7);

	/* Set read pin as HIGH */
	SET_RW;

	/* Read older half */
	SET_E;
	_delay_us(2);
	if(READ_D4) result |= 16;
	if(READ_D5) result |= 32;
	if(READ_D6) result |= 64;
	if(READ_D7) result |= 128;
	CLR_E;

	/* Read younger half */
	SET_E;
	_delay_us(2);
	if(READ_D4) result |= 1;
	if(READ_D5) result |= 2;
	if(READ_D6) result |= 4;
	if(READ_D7) result |= 8;
	CLR_E;

	return result;
} /* read8bits */
#endif

/*
 * Write data to display driver
 *
 * Parameters: (uint8_t data) - 8 data bits
 */
static void write8bits(uint8_t data)
{
	/* Set the pins as outputs */
	DDR(HD44780_IO_D4_PORT) |= (1<<HD44780_IO_D4);
	DDR(HD44780_IO_D5_PORT) |= (1<<HD44780_IO_D5);
	DDR(HD44780_IO_D6_PORT) |= (1<<HD44780_IO_D6);
	DDR(HD44780_IO_D7_PORT) |= (1<<HD44780_IO_D7);
	
#if HD44780_USE_RW_PIN == 1
	/* Set read pin as LOW */
	CLR_RW;
#endif

	/* Send older half */
	if(data & 16)  SET_D4; else CLR_D4;
	if(data & 32)  SET_D5; else CLR_D5;
	if(data & 64)  SET_D6; else CLR_D6;
	if(data & 128) SET_D7; else CLR_D7;
	SET_E; _delay_us(2); CLR_E;

	/* Send younger half */
	if(data & 1) SET_D4; else CLR_D4;
	if(data & 2) SET_D5; else CLR_D5;
	if(data & 4) SET_D6; else CLR_D6;
	if(data & 8) SET_D7; else CLR_D7;
	SET_E; _delay_us(2); CLR_E;

#if HD44780_USE_RW_PIN == 1
	/* Check Busy Flag */
	CLR_RS;
	while(read8bits() & CMD_BUSYFLAG);
#else
	/* Standard delay */
	_delay_us(120);
#endif
} /* write8bits */

/*
 * Write command to display driver
 *
 * Parameters: (uint8_t cmd) - 8 data bits
 */
static void write_cmd(uint8_t cmd)
{
	CLR_RS;
	write8bits(cmd);
} /* write_cmd */

/*
 * Write data to display driver
 *
 * Parameters: (uint8_t data) - 8 data bits
 */
static void write_data(uint8_t data)
{
	SET_RS;
	write8bits(data);
} /* write_data */

// /*
//  * Read data from display driver
//  *
//  * Return: (uint8_t) - 8 data bits
//  */
// #if HD44780_USE_RW_PIN == 1
// static uint8_t read_data(void)
// {
// 	SET_RS;
// 	return read8bits();
// } /* read_data */
// #endif

void lcd_init(void)
{
	/* Set the pins as outputs */
	DDR(HD44780_IO_D4_PORT) |= (1<<HD44780_IO_D4);
	DDR(HD44780_IO_D5_PORT) |= (1<<HD44780_IO_D5);
	DDR(HD44780_IO_D6_PORT) |= (1<<HD44780_IO_D6);
	DDR(HD44780_IO_D7_PORT) |= (1<<HD44780_IO_D7);
	DDR(HD44780_IO_RS_PORT) |= (1<<HD44780_IO_RS);
	DDR(HD44780_IO_E_PORT)  |= (1<<HD44780_IO_E);
#if HD44780_USE_RW_PIN == 1
	DDR(HD44780_IO_RW_PORT) |= (1<<HD44780_IO_RW);
#endif

	/* Set the control pins as LOW */
	CLR_RS; CLR_E;
#if HD44780_USE_RW_PIN == 1
	CLR_RW;
#endif

	/* Initialization begin */
	_delay_ms(100);
	
	/* Function Set; 8-bits interface */
	CLR_D7; CLR_D6; SET_D5; SET_D4;

	/* Apply change 3 times; Forcing a software reset */
	for(uint8_t i=0; i<=2; i++) {SET_E; _delay_us(2); CLR_E; _delay_ms(5);}
	
	/* Function Set; 4-bits interface */
	CLR_D4;
	
	/* Apply change */
	SET_E; _delay_us(2); CLR_E; _delay_us(100);

	/* Initial configuration */
	write_cmd(CMD_FUNCTIONSET | CMD_4BITMODE | CMD_2LINE | CMD_5x8DOTS);
	write_cmd(CMD_DISPLAYCONTROL | CMD_DISPLAYOFF | CMD_CURSOROFF | CMD_BLINKOFF);
	write_cmd(CMD_CLEARDISPLAY); _delay_ms(5);
	write_cmd(CMD_ENTRYMODESET | CMD_ENTRYRIGHT | CMD_ENTRYSHIFTINCREMENT);
	/* Initialization end */

	/* Display On */
	write_cmd(CMD_DISPLAYCONTROL | CMD_DISPLAYON | CMD_CURSOROFF | CMD_BLINKOFF);
} /* lcd_init */

void lcd_clear(void)
{
	/* Clear display and wait */
	write_cmd(CMD_CLEARDISPLAY);
	_delay_ms(5);
} /* lcd_clear */

void lcd_home(void)
{
	/* Move cursor to position [0,0] */
	write_cmd(CMD_RETURNHOME);
	_delay_ms(5);
} /* lcd_home */

void lcd_setCursor(uint8_t x, uint8_t y)
{
	/* Switch y to address of y row */
	switch(y)
	{
		default: case 0: y = DDRAM_LA1; break;
		case 1: y = DDRAM_LA2; break;
	#if (HD44780_ROWS == 4)
		case 2: y = DDRAM_LA3; break;
		case 3: y = DDRAM_LA4; break;
	#endif
	}
	
	/* Move cursor to position [x,y] */
	write_cmd(CMD_SETDDRAMADDR + x + y);
} /* lcd_setCursor */

void lcd_displayOnOff(uint8_t onoff)
{
	/* Prepare command */
	if(onoff) onoff = CMD_DISPLAYON; else onoff = CMD_DISPLAYOFF;
	
	/* Turn display ON/OFF */
	write_cmd(CMD_DISPLAYCONTROL | onoff | CMD_CURSOROFF | CMD_BLINKOFF);
} /* displayOnOff */

void lcd_cursorOnOff(uint8_t onoff)
{
	/* Prepare command */
	if(onoff) onoff = CMD_CURSORON; else onoff = CMD_CURSOROFF;
	
	/* Turn display cursor ON/OFF */
	write_cmd(CMD_DISPLAYCONTROL | CMD_DISPLAYON | onoff | CMD_BLINKOFF);
} /* lcd_cursorOnOff */

void lcd_cursorBlinkOnOff(uint8_t onoff)
{
	/* Prepare command */
	if(onoff) onoff = CMD_BLINKON; else onoff = CMD_BLINKOFF;
	
	/* Turn display cursor blink ON/OFF */
	write_cmd(CMD_DISPLAYCONTROL | CMD_DISPLAYON | CMD_CURSORON | onoff);
} /* lcd_cursorBlinkOnOff */

void lcd_codmtlor(uint8_t cod, uint8_t lor)
{
	/* Cursor Or Display move */
	if(cod) cod = CMD_DISPLAYMOVE; else cod = CMD_CURSORMOVE;
	
	/* Move to Left Or Right */
	if(lor) lor = CMD_MOVERIGHT; else lor = CMD_MOVELEFT;
	
	write_cmd(CMD_CURSORSHIFT | cod | lor);
} /* lcd_codmtlor */

void lcd_emlordoi(uint8_t lor, uint8_t doi)
{
	/* Left Or Right */
	if(lor) lor = CMD_ENTRYRIGHT; else lor = CMD_ENTRYLEFT;
	
	/* Decrement Or Increment */
	if(doi) doi = CMD_ENTRYSHIFTINCREMENT; else doi = CMD_ENTRYSHIFTDECREMENT;
	
	write_cmd(CMD_ENTRYMODESET | lor | doi);
} /* lcd_emlordoi */

void lcd_char(char c)
{
	/* Mask bits for user defined characters */
	if((c >= 0x80) && (c <= 0x87)) c &= 0x07;
	
	/* Write character */
	write_data(c);
} /* lcd_char */

void lcd_str(char *str)
{
	while(*str != '\0') lcd_char(*str++);
} /* lcd_str */

void lcd_str_P(const char *str)
{
	char c;
	
	while((c = pgm_read_byte(str++)) != '\0') lcd_char(c);
} /* lcd_str_P */

void lcd_str_E(char *str)
{
	char c;
	
	while(1)
	{
		c = eeprom_read_byte((uint8_t *)(str++));
		if((c == '\0') || (c == 0xFF)) break;
		lcd_char(c);
	}
} /* lcd_str_E */

void lcd_int16(int16_t value)
{
	char b[7]; /* [from -32768 to 32767] + NUL char */
	
	lcd_str(itoa(value, b, 10));
} /* lcd_int16 */

void lcd_uint16(uint16_t value)
{
	char b[6]; /* [from 0 to 65535] + NUL char */
	
	lcd_str(utoa(value, b, 10));
} /* lcd_uint16 */

void lcd_int32(int32_t value)
{
	char b[12]; /* [from -2147483648 to 2147483647] + NUL char */
	
	lcd_str(ltoa(value, b, 10));
} /* lcd_int32 */

void lcd_uint32(uint32_t value)
{
	char b[11]; /* [from 0 to 4294967295] + NUL char */
	
	lcd_str(ultoa(value, b, 10));
} /* lcd_uint32 */

void lcd_hex(uint32_t value)
{
	char b[9]; /* [from 0 to FFFFFFFF] + NUL char */
	
	lcd_str(ltoa(value, b, 16));
} /* lcd_hex */

void lcd_createChar(uint8_t code, uint8_t *pattern)
{
	/* Send CGRAM address */
	write_cmd(CMD_SETCGRAMADDR | ((code & 0x07) << 3));
	
	/* Send character pattern */
	for(uint8_t i=0; i<=7 ;i++) write_data(*pattern++);
} /* lcd_createChar */

void lcd_createChar_P(uint8_t code, const uint8_t *pattern)
{
	/* Send CGRAM address */
	write_cmd(CMD_SETCGRAMADDR | ((code & 0x07) << 3));
	
	/* Send character pattern */
	for(uint8_t i=0; i<=7 ;i++) write_data(pgm_read_byte(pattern++));
} /* lcd_createChar */

void lcd_createChar_E(uint8_t code, uint8_t *pattern)
{
	/* Send CGRAM address */
	write_cmd(CMD_SETCGRAMADDR | ((code & 0x07) << 3));
	
	/* Send character pattern */
	for(uint8_t i=0; i<=7 ;i++) write_data(eeprom_read_byte(pattern++));
} /* lcd_createChar */
