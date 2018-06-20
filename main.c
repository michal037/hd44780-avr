/*
 * HD44780 LCD Display Library
 *
 * MIT License
 * Copyright (c) 2018, Michal Kozakiewicz, github.com/michal037
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "lcd.h"

/* Strings */
char strA[] = "HD44780";
const char PROGMEM strB[] = "FLASH";
char EEMEM strC[] = "EEPROM";

/* User defined characters */
uint8_t degrees[] = {4,10,4,32,32,32,32,32};
const uint8_t PROGMEM thermometer[] = {4,10,10,10,17,31,31,14};
uint8_t EEMEM upArrow[] = {32,4,14,31,4,4,4,4};

/*
 * Example of use
 *
 * Remember to define F_CPU= for each file!
 * Remember to configure pins in lcd.h file!
 * Remember to upload data to the EEPROM memory after flashing!
 */
int main(void)
{
	/* Initialize the display */
	lcd_init();

	/* Create custom characters */
	lcd_createChar(0x80, degrees);
	lcd_createChar_P(0x81, thermometer);
	lcd_createChar_E(0x82, upArrow);
		
	/* Move cursor to position [0,0] */
	lcd_home();
	
	/* Strings from RAM memory */
	lcd_str(strA);
	lcd_str(" \x80\x81\x82 ");
	
	/* Write the number from RAM memory */
	lcd_int16(2018);
	
	/* Move cursor to position [0,1] */
	lcd_setCursor(0, 1);
	
	/* Strings from FLASH memory */
	lcd_str_P(strB);
	lcd_str_P(PSTR(" "));
	
	/* String from EEPROM memory */
	lcd_str_E(strC);
} /* main */
